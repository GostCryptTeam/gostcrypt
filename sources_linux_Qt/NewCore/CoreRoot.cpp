#include <QDir>
#include "CoreRoot.h"
#include "FuseDriver/FuseService.h"
#include "LoopDeviceManager.h"
#include "MountFilesystemManager.h"

namespace GostCrypt {
	namespace NewCore {

		CoreRoot::CoreRoot(QObject *parent): CoreBase(parent)
		{
            realGroupId = static_cast <gid_t>(0);
            realUserId = static_cast <gid_t>(0);

            const char *envSudoGID = getenv ("SUDO_GID");
            if (envSudoGID) {
                realGroupId =  static_cast <gid_t> (QString(envSudoGID).toLong());
            }
            const char *envSudoUID = getenv ("SUDO_UID");
            if (envSudoUID) {
                realUserId =  static_cast <gid_t> (QString(envSudoUID).toLong());
			}
		}

		void CoreRoot::request(QVariant r)
		{
			//TODO add all requests
			if(r.canConvert<QSharedPointer<GetMountedVolumesParams>>()) {
				QSharedPointer<GetMountedVolumesParams> request;
				QSharedPointer<GetMountedVolumesResponse> response;
				request = r.value<QSharedPointer<GetMountedVolumesParams>>();
				response = getMountedVolumes(request);
				emit sendGetMountedVolumes(response);
			} else if(r.canConvert<QSharedPointer<MountVolumeParams>>()) {
				QSharedPointer<MountVolumeParams> request;
				QSharedPointer<MountVolumeResponse> response;
				request = r.value<QSharedPointer<MountVolumeParams>>();
				response  = mountVolume(request);
				emit sendMountVolume(response);
			} else if(r.canConvert<QSharedPointer<DismountVolumeParams>>()) {
				QSharedPointer<DismountVolumeParams> request;
				QSharedPointer<DismountVolumeResponse> response;
				request = r.value<QSharedPointer<DismountVolumeParams>>();
				response = dismountVolume(request);
				emit sendDismountVolume(response);
			} else if(r.canConvert<QSharedPointer<CreateVolumeParams>>()) {
				QSharedPointer<CreateVolumeParams> request;
				QSharedPointer<CreateVolumeResponse> response;
				request = r.value<QSharedPointer<CreateVolumeParams>>();
				response = createVolume(request);
				emit sendCreateVolume(response);
			} else {
				qDebug() << "Unknow request : " << r.typeName();
			}
		}

		void CoreRoot::receiveSudoPassword(QSharedPointer<QByteArray> password)
		{
				password->fill('\0');
		}

		QSharedPointer<MountVolumeResponse> CoreRoot::mountVolume(QSharedPointer<MountVolumeParams> params)
		{
			QSharedPointer<MountVolumeResponse> response(new MountVolumeResponse);

			if(!params)
				throw MissingParamException("params");

			if(isVolumeMounted(params->path))
                throw VolumeAlreadyMountedException(params->path);

			QSharedPointer<Volume> volume(new Volume);
            QSharedPointer<QFileInfo> fuseMountPoint;

			do {
				try {
                    VolumePath path(params->path->absoluteFilePath().toStdWString());
                    shared_ptr<VolumePassword> password;
                    shared_ptr<VolumePassword> protectionPassword;

                    if(!params->password.isNull() && !params->password->isNull())
                        password.reset(new VolumePassword(params->password->constData(), params->password->size()));
                    else
                        throw MissingParamException("password");
                    if(!params->protectionPassword.isNull() && !params->protectionPassword->isNull())
                        protectionPassword.reset(new VolumePassword(params->protectionPassword->constData(), params->protectionPassword->size()));

                    volume->Open(
						path,
						params->preserveTimestamps,
                        password,
                        params->keyfiles,
						params->protection,
                        protectionPassword,
                        params->protectionKeyfiles,
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
                } catch(GostCrypt::PasswordException &e) {
                    throw ExceptionFromVolumeException("Incorrect password\n")
                } catch(GostCrypt::Exception &e) {
                    throw ExceptionFromVolumeException(e.what());
                }

				params->password->fill('\0');
                if(!params->protectionPassword.isNull())
                    params->protectionPassword->fill('\0');
				break;
			} while(0);

            try {
                if(isDevice(params->path->canonicalFilePath()))
				{
					if(volume->GetFile()->GetDeviceSectorSize() != volume->GetSectorSize())
						throw IncorrectSectorSizeException();
					/* GostCrypt suport only 512 sector size, other sector sizes can be use only with kernel crypto */
					if(volume->GetSectorSize() != 512)
						throw IncorrectSectorSizeException();
				}

                fuseMountPoint = getFreeFuseMountPoint();
                QDir fuseMountPointDir(fuseMountPoint->absoluteFilePath());
                if(!fuseMountPointDir.exists() && !fuseMountPointDir.mkdir(fuseMountPoint->absoluteFilePath()))
						throw FailedCreateFuseMountPointException(fuseMountPoint);

				try {
					// TODO recode fuse
                    shared_ptr<Volume> vol(new Volume(*volume));
                    FuseService::Mount (vol, (VolumeSlotNumber)1, fuseMountPoint->absoluteFilePath().toStdString());
				} catch (...) {
                    QDir(fuseMountPoint->absoluteFilePath()).rmdir(QStringLiteral("."));
					throw;
				}
			} catch (...) {
				volume->Close();
				throw;
			}

            bool mountDirCreated = false;
            try {

                QSharedPointer<QFileInfo> virtualDevice;
                QSharedPointer<QFileInfo> imageFile(new QFileInfo(fuseMountPoint->absoluteFilePath() + FuseService::GetVolumeImagePath()));
                virtualDevice = LoopDeviceManager::attachLoopDevice(imageFile, params->protection == VolumeProtection::ReadOnly);

                try {
                    DirectoryPath fmp(fuseMountPoint->absoluteFilePath().toStdWString());
                    DirectoryPath vd(virtualDevice->absoluteFilePath().toStdWString());
                    FuseService::SendAuxDeviceInfo(fmp, vd, vd);
                } catch(...) {
                    LoopDeviceManager::detachLoopDevice(virtualDevice);
                    throw;
                }

                if(params->doMount) {
                    if(params->mountPoint.isNull() || params->mountPoint->absoluteFilePath().isEmpty()) {
                        params->mountPoint = getFreeDefaultMountPoint(realUserId);
                    }

                    QDir mountpoint(params->mountPoint->absoluteFilePath());
                    if(!mountpoint.exists()) {
                        if(!mountpoint.mkpath(params->mountPoint->absoluteFilePath()))
                            throw FailedCreateDirectoryException(params->mountPoint->absoluteFilePath());
                        mountDirCreated = true;
                    }
                    QStringList possibleFilesystemTypes;
                    if(params && !params->fileSystemType.isEmpty())
						possibleFilesystemTypes.append(params->fileSystemType);
					else {
						QSharedPointer<GetFileSystemsTypesSupportedResponse> getFileSystemsTypesSupportedResponse;
						getFileSystemsTypesSupportedResponse = getFileSystemsTypesSupported();
						possibleFilesystemTypes = getFileSystemsTypesSupportedResponse->filesystems;
					}
                    MountFilesystemManager::mountFilesystem(virtualDevice, params->mountPoint, possibleFilesystemTypes, params->protection == VolumeProtection::ReadOnly, realUserId, realGroupId, params->fileSystemOptions);
                }
            } catch(...) {
                QSharedPointer<DismountVolumeParams> dismountParams(new DismountVolumeParams);
                dismountParams->volumepath = params->path;
                dismountVolume(dismountParams);
                if(mountDirCreated)
                    QDir(params->mountPoint->absoluteFilePath()).rmdir(params->mountPoint->absoluteFilePath());
                throw;
            }

            QSharedPointer<GetMountedVolumesParams> getMountedVolumesParams(new GetMountedVolumesParams);
            QSharedPointer<GetMountedVolumesResponse> getMountedVolumesResponse(new GetMountedVolumesResponse);
            getMountedVolumesParams->volumePath = params->path;

            getMountedVolumesResponse = getMountedVolumes(getMountedVolumesParams);
            response->volumeInfo = getMountedVolumesResponse->volumeInfoList.first();

			return response;
		}

		QSharedPointer<DismountVolumeResponse> CoreRoot::dismountVolume(QSharedPointer<DismountVolumeParams> params)
		{
            QSharedPointer<DismountVolumeResponse> response(new DismountVolumeResponse);

            /* Get mounted volume infos */
            QList<QSharedPointer<VolumeInfo>> mountedVolumes;
            {
                QSharedPointer<GetMountedVolumesParams> getMountedVolumesParams(new GetMountedVolumesParams);
                QSharedPointer<GetMountedVolumesResponse> getMountedVolumesResponse(new GetMountedVolumesResponse);
                getMountedVolumesParams->volumePath = params->volumepath;
                getMountedVolumesResponse = getMountedVolumes(getMountedVolumesParams);
                if(getMountedVolumesResponse->volumeInfoList.isEmpty())
                    throw VolumeNotMountedException(params->volumepath);
                mountedVolumes = getMountedVolumesResponse->volumeInfoList;
            }
            for (QSharedPointer<VolumeInfo> mountedVolume : mountedVolumes) {
                /* Unmount filesystem */
                if(!mountedVolume->MountPoint.IsEmpty()) {
                    MountFilesystemManager::dismountFilesystem(QSharedPointer<QFileInfo>(new QFileInfo(QString::fromStdWString(wstring(mountedVolume->MountPoint)))), params->force);
                }

                /* Detach loop device */
                if(!mountedVolume->LoopDevice.IsEmpty()) {
                    LoopDeviceManager::detachLoopDevice(QSharedPointer<QFileInfo>(new QFileInfo(QString::fromStdWString(wstring(mountedVolume->LoopDevice)))));
                }

                // Probably not necessary to update mountedVolume

                /* Unmount Fuse filesystem */
                MountFilesystemManager::dismountFilesystem(QSharedPointer<QFileInfo>(new QFileInfo(QString::fromStdWString(wstring(mountedVolume->AuxMountPoint)))), params->force);

                /* Delete fuse mount point directory */
                 QDir(QString::fromStdWString(wstring(mountedVolume->AuxMountPoint))).rmdir(QString::fromStdWString(wstring(mountedVolume->AuxMountPoint)));
            }
            return response;
        }

        void CoreRoot::writeHeaderToFile(fstream &file, QSharedPointer<CreateVolumeParams::VolumeParams> params, QSharedPointer<VolumeLayout> layout, quint64 containersize)
        {
            // getting the volume header to fill it
            QSharedPointer<VolumeHeader> header (layout->GetHeader());
            QSharedPointer<GostCrypt::EncryptionAlgorithm> ea (getEncryptionAlgorithm(params->encryptionAlgorithm));
            QSharedPointer<Pkcs5Kdf> Kdf (getDerivationKeyFunction(params->volumeHeaderKdf));

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

            if(params->size > 1.0 || params->size <= 0.0) // a percentage not in [0, 1]
                throw ContentSizeInvalidException(params->size);

            options.VolumeDataSize = (quint64)params->size*layout->GetMaxDataSize(containersize); // unlike truecrypt, we let the user set its own size

            GostCrypt::SecureBuffer masterkey;  // decrypts the whole filesystem
            GostCrypt::SecureBuffer salt;       // salt to encrypt the header.
            GostCrypt::SecureBuffer headerkey;  // fusion of the password and the keyfiles, derived

            SecureBuffer headerBuffer (layout->GetHeaderSize()); // our header we'll write to the container

            // Master data key
            masterkey.Allocate (options.EA->GetKeySize() * 2);
            RandomNumberGenerator::GetData (masterkey);
            options.DataKey = masterkey;

            // PKCS5 salt
            salt.Allocate (VolumeHeader::GetSaltSize());
            RandomNumberGenerator::GetData (salt);
            options.Salt = salt;

            // Header key
            headerkey.Allocate (VolumeHeader::GetLargestSerializedKeySize());
            QSharedPointer <VolumePassword> passwordkey = Keyfile::ApplyListToPassword (params->keyfiles, params->password);
            options.Kdf->DeriveKey (headerkey, *passwordkey, salt);
            options.HeaderKey = headerkey;

            header->Create (headerBuffer, options); // header created !

            // Write The Header
            if (layout->GetHeaderOffset() >= 0){
                file.seekp(layout->GetHeaderOffset(), std::ios_base::beg);
            }else{
                if(layout->GetHeaderSize() + layout->GetHeaderOffset() < 0)
                    throw InvalidHeaderOffsetException(layout->GetHeaderOffset(), layout->GetHeaderSize());
                file.seekp(containersize + layout->GetHeaderOffset(), std::ios_base::beg);
            }
            file.write((char*)headerBuffer.Ptr(), headerBuffer.Size()); // writing header

            if(!layout->HasBackupHeader())
                return;

            // Write The Backup Header if any

            RandomNumberGenerator::GetData (salt); // getting new salt
            options.Kdf->DeriveKey (headerkey, *passwordkey, salt);
            options.HeaderKey = headerkey;
            header->Create (headerBuffer, options); // creating new header

            if (layout->GetBackupHeaderOffset() >= 0)
                file.seekp(layout->GetBackupHeaderOffset(), std::ios_base::beg);
            else
                file.seekp(containersize + layout->GetBackupHeaderOffset(), std::ios_base::beg);
            file.write((char *)headerBuffer.Ptr(), headerBuffer.Size()); // writing backup header crypted with new salt

        }

        void CoreRoot::formatVolume(QSharedPointer<QFileInfo> volume, QSharedPointer<VolumePassword> password, QSharedPointer<KeyfileList> keyfiles, QString filesystem)
        {
            QString formatter = "mkfs."+filesystem;

            QSharedPointer<MountVolumeResponse> mountresponse;
            QSharedPointer<MountVolumeParams> mountparams(new MountVolumeParams());
            mountparams->keyfiles = keyfiles;
            mountparams->doMount = false;
            mountparams->password.reset(new QByteArray((char*)password->DataPtr(), password->Size()));
            mountparams->path = volume;

            QSharedPointer<DismountVolumeParams> dismountparams(new DismountVolumeParams());
            dismountparams->volumepath = volume;

            try {
                mountresponse = mountVolume(mountparams);
            } catch (CoreException &e){
                throw FormattingSubExceptionException(e);
            }

            QStringList arguments;
            arguments << QString::fromStdWString(wstring(mountresponse->volumeInfo->LoopDevice));

            QProcess *formatProcess = new QProcess();
            formatProcess->start(formatter, arguments);

            if (!formatProcess->waitForFinished() || formatProcess->exitStatus() != QProcess::NormalExit){
                try {
                    dismountVolume(dismountparams);
                } catch (CoreException &e){
                    throw FormattingSubExceptionException(e);
                }
                throw ProcessFailedException();
            }

            if (formatProcess->exitCode() == 127){ // command not found
                try {
                    dismountVolume(dismountparams);
                } catch (CoreException &e){
                    throw FormattingSubExceptionException(e);
                }
                throw FilesystemNotSupportedException(filesystem);
            }

            try {
                dismountVolume(dismountparams); // finally dismounting the volume
            } catch (CoreException &e){
                throw FormattingSubExceptionException(e);
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
             * WRITING RANDOM DATA ACROSS THE WHOLE VOLUME
             */

            createRandomFile(params->path, params->size, params->outerVolume->encryptionAlgorithm, false); // no random to create the file faster.

            // opening file (or device)
            volumefile.open(params->path->absoluteFilePath().toStdString(), ios::in | ios::out | ios::binary);
            if(!volumefile.is_open())
                throw FailedOpenVolumeException(params->path);

            /*
             * WRITING HEADERS
             */

            // getting the outer volume layout to write the header
            QSharedPointer<VolumeLayout> outerlayout;
            outerlayout.reset(new VolumeLayoutV2Normal()); // we only use the V2

            writeHeaderToFile(volumefile, params->outerVolume, outerlayout, params->size);

            QSharedPointer<VolumeLayout> innerlayout;
            innerlayout.reset(new VolumeLayoutV2Hidden()); // we ALWAYS have a hidden volume header, it can just be a fake one

            if(params->type == VolumeType::Hidden){ // writing the inner volume headers if any
                writeHeaderToFile(volumefile, params->innerVolume, innerlayout, params->size);
            } else { // writing random data to the hidden headers location
                QSharedPointer<CreateVolumeParams::VolumeParams> randomparams(new CreateVolumeParams::VolumeParams());
                randomparams->size = 0.5;
                randomparams->encryptionAlgorithm = params->outerVolume->encryptionAlgorithm;
                randomparams->filesystem = params->outerVolume->filesystem;
                randomparams->volumeHeaderKdf = params->outerVolume->volumeHeaderKdf;
                // creating a completely random password for a non-existent hidden volume
                SecureBuffer pass;
                pass.Allocate(VolumePassword::MaxSize);
                RandomNumberGenerator::GetData(pass);
                randomparams->password.reset(new VolumePassword(pass.Ptr(), pass.Size()));
                writeHeaderToFile(volumefile, randomparams, innerlayout, params->size);
            }

            /*
             * FORMATTING THE VOLUME
             */

            formatVolume(params->path, params->outerVolume->password, params->outerVolume->keyfiles, params->outerVolume->filesystem);

            if(params->type == VolumeType::Hidden)
                formatVolume(params->path, params->innerVolume->password, params->innerVolume->keyfiles, params->innerVolume->filesystem);

            return response;
		}

		QSharedPointer<ChangeVolumePasswordResponse> CoreRoot::changeVolumePassword(QSharedPointer<ChangeVolumePasswordParams> params)
		{

        }

	}
}
