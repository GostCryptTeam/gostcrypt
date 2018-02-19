#ifndef CORERESPONSE_H
#define CORERESPONSE_H
#include <QVariant>
#include <QList>
#include <QSharedPointer>
#include <QFileInfo>
#include <QtGlobal>
#include "SerializationUtil.h"
#include "CoreRequest.h"
#include "CoreException.h"

namespace GostCrypt {
	namespace Core {
        void initCoreResponse();

		struct HostDevice;
		struct CoreResponse {
			QVariantMap passThrough;
			DEC_SERIALIZABLE(CoreResponse);
		};

		struct CreateVolumeResponse : CoreResponse {
			DEC_SERIALIZABLE(CreateVolumeResponse);
		};

		struct ChangeVolumePasswordResponse : CoreResponse {
			DEC_SERIALIZABLE(ChangeVolumePasswordResponse);
		};

		struct CreateKeyFileResponse : CoreResponse {
			DEC_SERIALIZABLE(CreateKeyFileResponse);
		};

        struct BackupHeaderResponse : CoreResponse {
            DEC_SERIALIZABLE(BackupHeaderResponse);
        };

        struct RestoreHeaderResponse : CoreResponse {
            DEC_SERIALIZABLE(RestoreHeaderResponse);
        };

		struct MountVolumeResponse : CoreResponse {
			MountVolumeResponse();
			bool readOnlyFailover;
			QSharedPointer<Volume::VolumeInformation> volumeInfo;
			DEC_SERIALIZABLE(MountVolumeResponse);
		};

		struct DismountVolumeResponse : CoreResponse {
                        QList<QFileInfo> volumePath; // path of the file dismounted, not the mount point
			DEC_SERIALIZABLE(DismountVolumeResponse);
		};

		struct GetHostDevicesResponse : CoreResponse {
			QList<QSharedPointer<HostDevice>> hostDevices;
			DEC_SERIALIZABLE(GetHostDevicesResponse);
		};

		struct GetMountedVolumesResponse : CoreResponse {
			QList<QSharedPointer<Volume::VolumeInformation>> volumeInfoList;
			DEC_SERIALIZABLE(GetMountedVolumesResponse);
		};

        struct GetEncryptionAlgorithmsResponse : CoreResponse {
            QStringList algorithms;
            DEC_SERIALIZABLE(GetEncryptionAlgorithmsResponse);
        };

        struct GetDerivationFunctionsResponse : CoreResponse {
            QStringList algorithms;
            DEC_SERIALIZABLE(GetDerivationFunctionsResponse);
        };

		struct MountedFilesystem
		{
            QFileInfo Device;
            QFileInfo MountPoint;
            QString Type;
            DEC_SERIALIZABLE(MountedFilesystem);
		};

		struct HostDevice {
            QFileInfo mountPoint;
            QFileInfo devicePath;
			quint64 size;
			QList<QSharedPointer<HostDevice>> partitions;
			DEC_SERIALIZABLE(HostDevice);
		};

        struct BenchmarkAlgorithmsResponse: CoreResponse {
            QStringList algorithmsNames;
            QList<quint32> encryptionSpeed;// Encryption time
            QList<quint32> decryptionSpeed;// Decryption time
            QList<quint32> meanSpeed;// Mean
            DEC_SERIALIZABLE(BenchmarkAlgorithmsResponse);
        };
}
}

SERIALIZABLE(GostCrypt::Core::CoreResponse)
SERIALIZABLE(GostCrypt::Core::CreateVolumeResponse)
SERIALIZABLE(GostCrypt::Core::ChangeVolumePasswordResponse)
SERIALIZABLE(GostCrypt::Core::CreateKeyFileResponse)
SERIALIZABLE(GostCrypt::Core::MountVolumeResponse)
SERIALIZABLE(GostCrypt::Core::DismountVolumeResponse)
SERIALIZABLE(GostCrypt::Core::GetHostDevicesResponse)
SERIALIZABLE(GostCrypt::Core::GetMountedVolumesResponse)
SERIALIZABLE(GostCrypt::Core::GetEncryptionAlgorithmsResponse)
SERIALIZABLE(GostCrypt::Core::GetDerivationFunctionsResponse)
SERIALIZABLE(GostCrypt::Core::HostDevice)
SERIALIZABLE(GostCrypt::Core::MountedFilesystem)
SERIALIZABLE(GostCrypt::Core::BackupHeaderResponse)
SERIALIZABLE(GostCrypt::Core::RestoreHeaderResponse)
SERIALIZABLE(GostCrypt::Core::BenchmarkAlgorithmsResponse)

#endif // CORERESPONSE_H
