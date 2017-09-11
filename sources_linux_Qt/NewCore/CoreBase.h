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
#define HANDLE_REQUEST(requestName, fct) \
	if(r.canConvert<QSharedPointer<requestName ## Params>>()) { \
		QSharedPointer<requestName ## Params> request; \
		QSharedPointer<requestName ## Response> response; \
		request = r.value<QSharedPointer<requestName ## Params>>(); \
		response = fct (request); \
		emit send ## requestName (response); \
	}
#define DEC_REQUEST_SIGNAL(requestName) void send ## requestName (QSharedPointer<requestName ## Response> r)


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
            bool processNonRootRequest(QVariant r);
		signals:
			DEC_REQUEST_SIGNAL(CreateVolume);
			DEC_REQUEST_SIGNAL(MountVolume);
			DEC_REQUEST_SIGNAL(DismountVolume);
            DEC_REQUEST_SIGNAL(GetMountedVolumes);
			DEC_REQUEST_SIGNAL(GetEncryptionAlgorithms);
			DEC_REQUEST_SIGNAL(GetDerivationFunctions);
			DEC_REQUEST_SIGNAL(GetHostDevices);
            DEC_REQUEST_SIGNAL(CreateKeyFile);
            DEC_REQUEST_SIGNAL(ChangeVolumePassword);
			void exited();
			void askSudoPassword();
        };
        QSharedPointer<CoreBase> getCore();
	}
}

#endif // COREBASE_H
