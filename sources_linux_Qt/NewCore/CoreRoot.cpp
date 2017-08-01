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

            // getting the outer volume layout to write the header
            QSharedPointer<VolumeLayout> outerlayout;
            outerlayout.reset(new VolumeLayoutV2Normal());
            // getting the outer volume header to fill it
            QSharedPointer<VolumeHeader> outerheader (Layout->GetHeader()); // TODO


		}

		QSharedPointer<CreateKeyFileResponse> CoreRoot::createKeyFile(QSharedPointer<CreateKeyFileParams> params)
		{

		}

		QSharedPointer<ChangeVolumePasswordResponse> CoreRoot::changeVolumePassword(QSharedPointer<ChangeVolumePasswordParams> params)
		{

		}
	}
}
