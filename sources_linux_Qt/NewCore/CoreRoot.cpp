#include <QDir>
#include "CoreRoot.h"
#include "FuseDriver/FuseService.h"

namespace GostCrypt {
	namespace NewCore {
		CoreRoot::CoreRoot()
		{
		}

		QSharedPointer<MountVolumeResponse> CoreRoot::mountVolume(QSharedPointer<MountVolumeParams> params)
		{
			QSharedPointer<MountVolumeResponse> response(new MountVolumeResponse);

			if(!params)
				throw MissingParamException("params");

			if(isVolumeMounted(params->path))
                throw VolumeAlreadyMountedException(params->path);

			QSharedPointer<Volume> volume(new Volume);
			do {
				try {
					VolumePath path(params->path->canonicalFilePath().toStdWString());
                    shared_ptr<KeyfileList> keyfiles(new KeyfileList(*params->keyfiles));
                    shared_ptr<KeyfileList> protectionKeyfiles(new KeyfileList(*params->protectionKeyfiles));
                    volume->Open(
						path,
						params->preserveTimestamps,
						SharedPtr<VolumePassword>(new VolumePassword(params->password->constData(), params->password->size())),
                        keyfiles,
						params->protection,
						SharedPtr<VolumePassword>(new VolumePassword(params->protectionPassword->constData(), params->protectionPassword->size())),
                        protectionKeyfiles,
						params->useBackupHeaders
					);
				} catch(GostCrypt::SystemException &e) {
					// In case of permission issue try again in read-only
					if(params->protection != VolumeProtection::ReadOnly && (e.GetErrorCode() == EROFS || e.GetErrorCode() == EACCES || e.GetErrorCode() == EPERM))
					{
						params->protection = VolumeProtection::ReadOnly;
						response->writeDisabled = true;
						continue;
					}
                    throw FailedOpenVolumeException(params->path);
				}
				params->password->fill('\0');
				params->protectionPassword->fill('\0');
				break;
			} while(0);

            try {
				if(params->isDevice)
				{
					if(volume->GetFile()->GetDeviceSectorSize() != volume->GetSectorSize())
						throw IncorrectSectorSizeException();
					/* GostCrypt suport only 512 sector size, other sector sizes can be use only with kernel crypto */
					if(volume->GetSectorSize() != 512)
						throw IncorrectSectorSizeException();
				}

				QSharedPointer<QFileInfo> fuseMountPoint = getFreeFuseMountPoint();
				if(!QDir(fuseMountPoint->canonicalFilePath()).mkdir(QStringLiteral(".")))
						throw FailedCreateFuseMountPointException(fuseMountPoint);

				try {
					// TODO recode fuse
					SharedPtr<Volume> vol(new Volume(*volume));
					FuseService::Mount (vol, (VolumeSlotNumber)1, fuseMountPoint->canonicalFilePath().toStdString());
				} catch (...) {
					QDir(fuseMountPoint->canonicalFilePath()).rmdir(QStringLiteral("."));
					throw;
				}
			} catch (...) {
				volume->Close();
				throw;
			}

			bool mountDirCreated = false;
			if(params->doMount) {
				QDir mountpoint(params->mountPoint->canonicalFilePath());
				if(!mountpoint.exists())
					mountpoint.mkdir(QStringLiteral("."));
			}

			return response;
		}

		QSharedPointer<DismountVolumeResponse> CoreRoot::dismountVolume(QSharedPointer<DismountVolumeParams> params)
		{

		}

        void CoreRoot::writeHeaderToFile(fstream &file, QSharedPointer<CreateVolumeParams::VolumeParams> params, QSharedPointer<VolumeLayout> layout, quint64 containersize)
        {
            // getting the volume header to fill it
            QSharedPointer<VolumeHeader> header (layout->GetHeader());
            QSharedPointer<EncryptionAlgorithm> ea (getEncryptionAlgorithm(params->encryptionAlgorithm));
            QSharedPointer<Pkcs5Kdf> Kdf (getDerivationKeyFunction(params->volumeHeaderKdf));

            if(!ea || !Kdf)
                throw /* TODO AlgorithmNotFoundException */;

            VolumeHeaderCreationOptions options;
            options.EA = ea;
            options.Kdf = Kdf;
            options.Type = layout->GetType();
            options.SectorSize = 512; // TODO : ALWAYS 512 !

            if(options.Type == VolumeType::Hidden){
                options.VolumeDataStart = containersize - layout->GetHeaderSize() * 2 - params->size;
            }else{
                options.VolumeDataStart = layout->GetHeaderSize() * 2;
            }

            if(params->size > layout->GetMaxDataSize(containersize)) // we have to leave space for the headers
                    throw /* TODO SizeTooLargeException */;
            options.VolumeDataSize = params->size; // unlike truecrypt, we let the user set its own size

            GostCrypt::SecureBuffer masterkey;  // decrypts the whole filesystem
            GostCrypt::SecureBuffer salt;       // salt to encrypt the header.
            GostCrypt::SecureBuffer headerkey;  // fusion of the password and the keyfiles, derived

            SecureBuffer headerBuffer (layout->GetHeaderSize()); // our header we'll write to the container

            // Master data key
            masterkey.Allocate (options.EA->GetKeySize() * 2);
            RandomNumberGenerator::GetData (MasterKey);
            options.DataKey = MasterKey;

            // PKCS5 salt
            salt.Allocate (VolumeHeader::GetSaltSize());
            RandomNumberGenerator::GetData (salt);
            options.Salt = salt;

            // Header key
            headerkey.Allocate (VolumeHeader::GetLargestSerializedKeySize());
            shared_ptr <VolumePassword> passwordkey = Keyfile::ApplyListToPassword (options->Keyfiles, options->Password); // TODO : no shared_ptr please
            options->VolumeHeaderKdf->DeriveKey (headerKey, *passwordkey, salt);
            options.HeaderKey = headerKey;

            header->Create (headerBuffer, headerOptions); // header created !

            // Write The Header
            if (layout->GetHeaderOffset() >= 0)
                file.seekp(layout->GetHeaderOffset(), std::ios_base::beg);
            else
                file.seekp(containersize + layout->GetHeaderOffset(), std::ios_base::beg); // TODO : check if headeroffset > headersize so it doesnt affects the containersize ?
            file.write(headerBuffer.Ptr(), headerBuffer.Size()); // writing header

            if(!layout->HasBackupHeader())
                return;

            // Write The Backup Header if any

            RandomNumberGenerator::GetData (salt); // getting new salt
            options->VolumeHeaderKdf->DeriveKey (headerKey, *passwordkey, salt);
            options.HeaderKey = headerKey;
            header->Create (headerBuffer, headerOptions); // creating new header

            if (layout->GetBackupHeaderOffset() >= 0)
                file.seekp(layout->GetBackupHeaderOffset(), std::ios_base::beg);
            else
                file.seekp(containersize + layout->GetBackupHeaderOffset(), std::ios_base::beg);
            file.write(headerBuffer.Ptr(), headerBuffer.Size()); // writing backup header crypted with new salt

        }

        void CoreRoot::formatVolume(QSharedPointer<QFileInfo> volume, QSharedPointer<VolumePassword> password, QSharedPointer<KeyfileList> keyfiles, QString filesystem)
        {
            QString formatter;
            QStringList convertFS[2] = {
                {"ext2",      "ext3",      "ext4",      "hfs"        "ufs"   },
                {"mkfs.ext2", "mkfs.ext3", "mkfs.ext4", "newfs_hfs", "newfs" }
            };

            uint32 index = convertFS[0].indexOf(QRegExp(filesystem), Qt::CaseInsensitive); // trying to find the filesystem
            if (index == -1)
                throw /* TODO filesystemnotfound */;
            formatter = convertFS[1][index];

            QSharedPointer<MountVolumeResponse> mountresponse;
            QSharedPointer<MountVolumeParams> mountparams(new MountVolumeParams());
            mountparams->keyfiles = keyfiles;
            mountparams->noFileSystem = true;
            mountparams->password = password;
            mountparams->path = volume;

            try {
                mountresponse = mountVolume(mountparams); // TODO don't forget to unmount it if something goes bad
            } catch (CoreException &e){
                throw e; // TODO : create a new exception here
            }

            QStringList arguments;
            arguments << mountresponse->volumeInfo->LoopDevice;

            QProcess *formatProcess = new QProcess();
            formatProcess->start(formatter, arguments); // TODO check if failed ?

            if (!formatProcess.waitForFinished())
                throw /* TODO : processErrorException */;

            QSharedPointer<DismountVolumeParams> dismountparams(new DismountVolumeParams());
            dismountparams->volumepath = volume;

            try {
                dismountVolume(dismountparams); // finally dismounting the volume
            } catch (CoreException &e){
                throw e; // TODO : create a new exception here
            }
        }

        QSharedPointer<CreateVolumeResponse> CoreRoot::createVolume(QSharedPointer<CreateVolumeParams> params)
		{
            QSharedPointer<CreateVolumeResponse> response(new CreateVolumeResponse);

            fstream volumefile;

            /*  Steps:
                    write headers directly in file.
                    mount with no-filesystem
                    format the loop device
                    unmount
            */

            if(!params)
                throw MissingParamException("params");
            if(!params->path)
                throw MissingParamException("params->path");
            if(!params->outerVolume)
                throw MissingParamException("params->outervolume");

            // this is the CoreRoot class. we are assuming that it is launched as root.

            /*
             * WRITING RANDOM DATA ACROSS THE WHOLE VOLUME // TODO since it's relative to the layouts, maybe change this
             */

             createRandomFile(params->path, params->size, params->outerVolume->encryptionAlgorithm);

            // opening file (or device)
            volumefile.open(params->path->absoluteFilePath(), ios::in | ios::out | ios::binary);
            if(!volumefile.is_open())
                throw /* TODO add exception here */;

            /*
             * WRITING HEADERS
             */

            // getting the outer volume layout to write the header
            QSharedPointer<VolumeLayout> outerlayout;
            outerlayout.reset(new VolumeLayoutV2Normal());

            writeHeaderToFile(volumefile, params->outerVolume, outerlayout, params->size);

            QSharedPointer<VolumeLayout> innerlayout;
            innerlayout.reset(new VolumeLayoutV2Hidden());

            if(params->type == VolumeType::Hidden){ // writing the inner volume headers if any
                writeHeaderToFile(volumefile, params->innerVolume, innerlayout, params->size);
            } else { // writing random data to the hidden headers location
                QSharedPointer<CreateVolumeParams::VolumeParams> randomparams(new CreateVolumeParams::VolumeParams());
                randomparams->size = params->size / 2;
                randomparams->encryptionAlgorithm = params->outerVolume->encryptionAlgorithm;
                randomparams->filesystem = params->outerVolume->filesystem;
                randomparams->filesystemClusterSize = params->outerVolume->filesystemClusterSize;
                randomparams->volumeHeaderKdf = params->outerVolume->volumeHeaderKdf;
                // creating a completely random password for a non-existent hidden volume
                SecureBuffer pass;
                pass.Allocate(VolumePassword::MaxSize);
                RandomNumberGenerator::GetData(pass);
                randomparams->password = QSharedPointer<VolumePassword>(new VolumePassword(pass.Ptr(), pass.Size()));
                writeHeaderToFile(volumefile, randomparams, innerlayout, params->size);
            }

            /*
             * FORMATTING THE VOLUME
             */

            formatVolume(params->path, params->outerVolume->password, params->outerVolume->keyfiles, params->outerVolume->filesystem);

            if(params->type == VolumeType::Hidden)
                formatVolume(params->path, params->innerVolume->password, params->innerVolume->keyfiles, params->innerVolume->filesystem);

		}

		QSharedPointer<CreateKeyFileResponse> CoreRoot::createKeyFile(QSharedPointer<CreateKeyFileParams> params)
		{

		}

		QSharedPointer<ChangeVolumePasswordResponse> CoreRoot::changeVolumePassword(QSharedPointer<ChangeVolumePasswordParams> params)
		{

        }

	}
}
