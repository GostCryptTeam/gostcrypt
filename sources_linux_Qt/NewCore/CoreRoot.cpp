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
                    VolumePath path(params->path->absoluteFilePath().toStdWString());
                    shared_ptr<KeyfileList> keyfiles;
                    shared_ptr<KeyfileList> protectionKeyfiles;
                    shared_ptr<VolumePassword> password;
                    shared_ptr<VolumePassword> protectionPassword;

                    if(!params->keyfiles.isNull())
                       keyfiles.reset(new KeyfileList(*params->keyfiles));
                    if(!params->protectionKeyfiles.isNull())
                        protectionKeyfiles.reset(new KeyfileList(*params->protectionKeyfiles));
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
                        keyfiles,
						params->protection,
                        protectionPassword,
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
                if(!params->protectionPassword.isNull())
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
                QString fuseMountPointPath = fuseMountPoint->absoluteFilePath();
                QDir fuseMountPointDir(fuseMountPointPath);
                if(!fuseMountPointDir.mkdir(QStringLiteral(".")))
						throw FailedCreateFuseMountPointException(fuseMountPoint);

				try {
					// TODO recode fuse
					SharedPtr<Volume> vol(new Volume(*volume));
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
			if(params->doMount) {
                QDir mountpoint(params->mountPoint->absoluteFilePath());
				if(!mountpoint.exists())
					mountpoint.mkdir(QStringLiteral("."));
			}




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
