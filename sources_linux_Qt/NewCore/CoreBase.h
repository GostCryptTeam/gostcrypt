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
			explicit CoreBase(QObject *parent = nullptr);
		public slots:
			virtual void request(QVariant request) = 0;
			virtual void exit() = 0;
			virtual void receiveSudoPassword(QSharedPointer<QByteArray> password) = 0;
		protected:
            QSharedPointer<GetEncryptionAlgorithmsResponse> getEncryptionAlgorithms(QSharedPointer<GetEncryptionAlgorithmsParams> params = QSharedPointer<GetEncryptionAlgorithmsParams>());
            QSharedPointer<GetDerivationFunctionsResponse> getDerivationFunctions(QSharedPointer<GetDerivationFunctionsParams> params = QSharedPointer<GetDerivationFunctionsParams>());
			QSharedPointer<GetHostDevicesResponse> getHostDevices(QSharedPointer<GetHostDevicesParams> params = QSharedPointer<GetHostDevicesParams>());
			QSharedPointer<GetMountedVolumesResponse> getMountedVolumes(QSharedPointer<GetMountedVolumesParams> params = QSharedPointer<GetMountedVolumesParams>());
            QSharedPointer<GetFileSystemsTypesSupportedResponse> getFileSystemsTypesSupported(QSharedPointer<GetFileSystemsTypesSupportedParams> params = QSharedPointer<GetFileSystemsTypesSupportedParams>());
            QSharedPointer<CreateKeyFileResponse> createKeyFile(QSharedPointer<CreateKeyFileParams> params);

			QList<QSharedPointer<MountedFilesystem>> getMountedFilesystems(const QFileInfo &devicePath = QFileInfo(), const QFileInfo &mountPoint = QFileInfo());
            QSharedPointer<EncryptionAlgorithm> getEncryptionAlgorithm(QString algorithm);
            QSharedPointer<Pkcs5Kdf> getDerivationKeyFunction(QString function);
            QSharedPointer<QFileInfo> getDeviceMountPoint(const QSharedPointer<QFileInfo> &devicePath);
            bool isDevice(QString path);
			bool isVolumeMounted(QSharedPointer<QFileInfo> volumeFile);
            QSharedPointer<QFileInfo> getFreeFuseMountPoint();
            void createRandomFile(QSharedPointer<QFileInfo> path, quint64 size, QString algorithm = "", bool random = true);
            void randomizeEncryptionAlgorithmKey (QSharedPointer <EncryptionAlgorithm> encryptionAlgorithm) const;
            QSharedPointer<QFileInfo> getFreeDefaultMountPoint(uid_t userId);
		signals:
			void sendMountVolume(QSharedPointer<MountVolumeResponse> r);
			void sendDismountVolume(QSharedPointer<DismountVolumeResponse> r);
			void sendCreateVolume(QSharedPointer<CreateVolumeResponse> r);
			void sendGetMountedVolumes(QSharedPointer<GetMountedVolumesResponse> r);
			void exited();
			void askSudoPassword();
        };
        QSharedPointer<CoreBase> getCore(QCoreApplication *a);
	}
}

#endif // COREBASE_H
