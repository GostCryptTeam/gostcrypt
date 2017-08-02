#include "CoreRoot.h"
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
				params->password->fill('*');
				params->protectionPassword->fill('*');
				break;
			} while(0);

            if(params->isDevice)
			{
				if(volume->GetFile()->GetDeviceSectorSize() != volume->GetSectorSize())
                    throw IncorrectSectorSizeException();
                /* GostCrypt suport only 512 sector size, other sector sizes can be use only with kernel crypto */
                if(volume->GetSectorSize() != 512)
                    throw IncorrectSectorSizeException();
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

        QSharedPointer<CreateVolumeResponse> CoreRoot::createVolume(QSharedPointer<CreateVolumeParams> params)
		{
            QSharedPointer<CreateVolumeResponse> response(new CreateVolumeResponse);

            quint64 hostSize = 0;
            fstream volumefile;

            /*  Steps:
                    write headers directly in file.
                    mount with no-filesystem
                    format the loop device
                    unmount
            */

            if(!params)
                throw MissingParamException("params");

            // this is the CoreRoot class. we are assuming that it is launched as root.

            // opening file (or device)
            volumefile.open(params->path.absoluteFilePath(), ios::in | ios::out | ios::binary);
            if(!volumefile.is_open())
                throw /* TODO add exception here */;

            // making sure the file is of the right size
            volumefile.seekp(containersize - 1, std::ios_base::beg);
            volumefile.put('\0');

            /*
             * WRITING HEADERS
             */

            // getting the outer volume layout to write the header
            QSharedPointer<VolumeLayout> outerlayout;
            outerlayout.reset(new VolumeLayoutV2Normal());

            writeHeaderToFile(volumefile, params->outerVolume, outerlayout, params->size);

            if(params->type == VolumeType::Hidden){ // writing the inner volume headers if any
                QSharedPointer<VolumeLayout> innerlayout;
                innerlayout.reset(new VolumeLayoutV2Hidden());

                writeHeaderToFile(volumefile, params->innerVolume, innerlayout, params->size);
            }

            /*
             * MOUNTING THE VOLUME WITH no-filesystem (bc there is no filesystem yet)
             */

            QSharedPointer<MountVolumeResponse> mountresponse;
            QSharedPointer<MountVolumeParams> mountparams(new MountVolumeParams());
            mountparams->keyfiles = params->outerVolume->keyfiles;
            mountparams->noFileSystem = true;
            mountparams->password = params->outerVolume->password;
            mountparams->path = params->path;

            try {
                mountresponse = mountVolume(mountparams);
            } catch (CoreException &e){
                throw e; // TODO : create a new exception here
            }



		}

		QSharedPointer<CreateKeyFileResponse> CoreRoot::createKeyFile(QSharedPointer<CreateKeyFileParams> params)
		{

		}

		QSharedPointer<ChangeVolumePasswordResponse> CoreRoot::changeVolumePassword(QSharedPointer<ChangeVolumePasswordParams> params)
		{

        }

	}
}
