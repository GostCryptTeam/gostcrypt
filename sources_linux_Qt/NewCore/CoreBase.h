#ifndef COREBASE_H
#define COREBASE_H

#include <QObject>
#include <QCoreApplication>
#include "CoreParams.h"
#include "CoreResponse.h"
#include "CoreException.h"
#include "RandomNumberGenerator.h"
#include "Volume/EncryptionModeXTS.h"
#include "Volume/EncryptionAlgorithm.h"
#include "Volume/Pkcs5Kdf.h"

#define GOSTCRYPT_FUSE_MOUNT_DIR_PREFIX ".gostcrypt_aux_mnt"

namespace GostCrypt {
	namespace NewCore {
		class CoreBase : public QObject
		{
			Q_OBJECT
		public:
			CoreBase();

			virtual QSharedPointer<MountVolumeResponse> mountVolume(QSharedPointer<MountVolumeParams> params) = 0;
			virtual QSharedPointer<DismountVolumeResponse> dismountVolume(QSharedPointer<DismountVolumeParams> params) = 0;
            virtual QSharedPointer<CreateVolumeResponse> createVolume(QSharedPointer<CreateVolumeParams> params) = 0;
			virtual QSharedPointer<CreateKeyFileResponse> createKeyFile(QSharedPointer<CreateKeyFileParams> params) = 0;
			virtual QSharedPointer<ChangeVolumePasswordResponse> changeVolumePassword(QSharedPointer<ChangeVolumePasswordParams> params) = 0;
			QSharedPointer<GetHostDevicesResponse> getHostDevices(QSharedPointer<GetHostDevicesParams> params = QSharedPointer<GetHostDevicesParams>());
			QSharedPointer<GetMountedVolumesResponse> getMountedVolumes(QSharedPointer<GetMountedVolumesParams> params = QSharedPointer<GetMountedVolumesParams>());
            QSharedPointer<GetFileSystemsTypesSupportedResponse> getFileSystemsTypesSupported(QSharedPointer<GetFileSystemsTypesSupportedParams> params = QSharedPointer<GetFileSystemsTypesSupportedParams>());
		protected:
			QList<QSharedPointer<MountedFilesystem>> getMountedFilesystems(const QFileInfo &devicePath = QFileInfo(), const QFileInfo &mountPoint = QFileInfo());
            QSharedPointer<EncryptionAlgorithm> getEncryptionAlgorithm(QString algorithm);
            QSharedPointer<Pkcs5Kdf> getDerivationKeyFunction(QString function);
            QSharedPointer<QFileInfo> getDeviceMountPoint(const QSharedPointer<QFileInfo> &devicePath);
			bool isVolumeMounted(QSharedPointer<QFileInfo> volumeFile);
            QSharedPointer<QFileInfo> getFreeFuseMountPoint();
            void createRandomFile(QSharedPointer<QFileInfo> path, quint64 size, QString algorithm = "", bool random = true);
            void randomizeEncryptionAlgorithmKey (QSharedPointer <EncryptionAlgorithm> encryptionAlgorithm) const;
            QSharedPointer<QFileInfo> getFreeDefaultMountPoint(uid_t userId);
        };
        QSharedPointer<CoreBase> getCore(QCoreApplication *a);
	}
}

#endif // COREBASE_H
