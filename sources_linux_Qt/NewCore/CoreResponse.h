#ifndef CORERESPONSE_H
#define CORERESPONSE_H
#include <QVariant>
#include <QList>
#include <QSharedPointer>
#include <QFileInfo>
#include "SerializationUtil.h"
#include "Volume/VolumeInfo.h"
#include "CoreParams.h"

namespace GostCrypt {
	namespace NewCore {
        bool initCoreResponse();

		struct HostDevice;
		struct CoreResponse {
			DEC_SERIALIZABLE(CoreResponse);
		};

		struct InitResponse : CoreResponse {
			DEC_SERIALIZABLE(InitResponse);
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

		struct MountVolumeResponse : CoreResponse {
			bool writeDisabled;
			QSharedPointer<VolumeInfo> volumeInfo;
			DEC_SERIALIZABLE(MountVolumeResponse);
		};

		struct DismountVolumeResponse : CoreResponse {
			DEC_SERIALIZABLE(DismountVolumeResponse);
		};

		struct GetHostDevicesResponse : CoreResponse {
			QList<QSharedPointer<HostDevice>> hostDevices;
			DEC_SERIALIZABLE(GetHostDevicesResponse);
		};

		struct GetMountedVolumesResponse : CoreResponse {
			QList<QSharedPointer<VolumeInfo>> volumeInfoList;
			DEC_SERIALIZABLE(GetMountedVolumesResponse);
		};

        struct GetFileSystemsTypesSupportedResponse : CoreResponse {
            QStringList filesystems;
            DEC_SERIALIZABLE(GetFileSystemsTypesSupportedResponse);
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
            QSharedPointer<QFileInfo> Device;
            QSharedPointer<QFileInfo> MountPoint;
            QSharedPointer<QString> Type;
		};

		struct HostDevice {
            QSharedPointer<QFileInfo> mountPoint;
            QSharedPointer<QFileInfo> devicePath;
			quint64 size;
			QList<QSharedPointer<HostDevice>> partitions;
		};
	}
}

SERIALIZABLE(GostCrypt::NewCore::CoreResponse)
SERIALIZABLE(GostCrypt::NewCore::CreateVolumeResponse)
SERIALIZABLE(GostCrypt::NewCore::ChangeVolumePasswordResponse)
SERIALIZABLE(GostCrypt::NewCore::CreateKeyFileResponse)
SERIALIZABLE(GostCrypt::NewCore::MountVolumeResponse)
SERIALIZABLE(GostCrypt::NewCore::DismountVolumeResponse)
SERIALIZABLE(GostCrypt::NewCore::GetHostDevicesResponse)
SERIALIZABLE(GostCrypt::NewCore::GetMountedVolumesResponse)
SERIALIZABLE(GostCrypt::NewCore::GetFileSystemsTypesSupportedResponse)
SERIALIZABLE(GostCrypt::NewCore::GetEncryptionAlgorithmsResponse)
SERIALIZABLE(GostCrypt::NewCore::GetDerivationFunctionsResponse)
SERIALIZABLE(GostCrypt::NewCore::InitResponse)








#endif // CORERESPONSE_H
