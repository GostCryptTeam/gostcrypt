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
				throw VolumeAlreadyMountedException(*params->path);

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
					throw;
				}
				params->password->fill('*');
				params->protectionPassword->fill('*');
				break;
			} while(0);

			/*
            if(params->isDevice)
			{
				if(volume->GetFile()->GetDeviceSectorSize() != volume->GetSectorSize())
					throw;
			}//*/






			return response;
		}

		QSharedPointer<DismountVolumeResponse> CoreRoot::dismountVolume(QSharedPointer<DismountVolumeParams> params)
		{

		}

		QSharedPointer<CreateVolumeResponse> CoreRoot::createVolume(QSharedPointer<DismountVolumeParams> params)
		{

		}

		QSharedPointer<CreateKeyFileResponse> CoreRoot::createKeyFile(QSharedPointer<CreateKeyFileParams> params)
		{

		}

		QSharedPointer<ChangeVolumePasswordResponse> CoreRoot::changeVolumePassword(QSharedPointer<ChangeVolumePasswordParams> params)
		{

		}
	}
}
