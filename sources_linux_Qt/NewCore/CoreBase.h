#ifndef COREBASE_H
#define COREBASE_H

#include <QObject>
#include "CoreParams.h"
#include "CoreResponse.h"
#include "CoreException.h"

#define GOSTCRYPT_FUSE_MOUNTPOINT_PREFIX "/tmp/.gostcrypt_aux_mnt"

namespace GostCrypt {
	namespace NewCore {
		class CoreBase : public QObject
		{
			Q_OBJECT
		public:
			CoreBase();

			virtual QSharedPointer<MountVolumeResponse> mountVolume(QSharedPointer<MountVolumeParams> params) = 0;
			virtual QSharedPointer<DismountVolumeResponse> dismountVolume(QSharedPointer<DismountVolumeParams> params) = 0;
			virtual QSharedPointer<CreateVolumeResponse> createVolume(QSharedPointer<DismountVolumeParams> params) = 0;
			virtual QSharedPointer<CreateKeyFileResponse> createKeyFile(QSharedPointer<CreateKeyFileParams> params) = 0;
			virtual QSharedPointer<ChangeVolumePasswordResponse> changeVolumePassword(QSharedPointer<ChangeVolumePasswordParams> params) = 0;
			QSharedPointer<GetHostDevicesResponse> getHostDevices(QSharedPointer<GetHostDevicesParams> params = QSharedPointer<GetHostDevicesParams>());
			QSharedPointer<GetMountedVolumesResponse> getMountedVolumes(QSharedPointer<GetMountedVolumesParams> params = QSharedPointer<GetMountedVolumesParams>());
            QSharedPointer<GetFileSystemsTypesSupportedResponse> getFileSystemsTypesSupported(QSharedPointer<GetFileSystemsTypesSupportedParams> params = QSharedPointer<GetFileSystemsTypesSupportedParams>());
		protected:
			QList<QSharedPointer<MountedFilesystem>> getMountedFilesystems(const QFileInfo &devicePath = QFileInfo(), const QFileInfo &mountPoint = QFileInfo());
            QSharedPointer<QFileInfo> getDeviceMountPoint(const QSharedPointer<QFileInfo> &devicePath);
			bool isVolumeMounted(QSharedPointer<QFileInfo> volumeFile);
            QSharedPointer<QFileInfo> getFreeFuseMountPoint();
		};
		QSharedPointer<CoreBase> getCore();
	}
}

#endif // COREBASE_H
