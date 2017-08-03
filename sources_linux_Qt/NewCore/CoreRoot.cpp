#include <QDir>
#include "CoreRoot.h"
#include "FuseDriver/FuseService.h"
#include "LoopDeviceManager.h"
#include "MountFilesystemManager.h"

namespace GostCrypt {
	namespace NewCore {
		CoreRoot::CoreRoot()
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

                fuseMountPoint = getFreeFuseMountPoint();
                QDir fuseMountPointDir(fuseMountPoint->absoluteFilePath());
                if(!fuseMountPointDir.exists() && !fuseMountPointDir.mkdir(fuseMountPoint->absoluteFilePath()))
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
            try {

                QSharedPointer<QFileInfo> virtualDevice;
                QSharedPointer<QFileInfo> imageFile(new QFileInfo(fuseMountPoint->absoluteFilePath() + FuseService::GetVolumeImagePath()));
                virtualDevice = LoopDeviceManager::attachLoopDevice(imageFile, params->protection == VolumeProtection::ReadOnly);

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
                    MountFilesystemManager::MountFilesystem(virtualDevice, params->mountPoint, params->fileSystemType, params->protection == VolumeProtection::ReadOnly, realUserId, realGroupId, params->fileSystemOptions);
                }
            } catch(...) {
                QSharedPointer<DismountVolumeParams> dismountParams(new DismountVolumeParams);
                dismountParams->volumepath = params->path;
                dismountVolume(dismountParams);
                if(mountDirCreated)
                    QDir(params->mountPoint->absoluteFilePath()).rmdir(params->mountPoint->absoluteFilePath());
                throw;
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
