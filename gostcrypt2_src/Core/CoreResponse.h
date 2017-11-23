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
			bool changeMasterKey;
			DEC_SERIALIZABLE(ChangeVolumePasswordResponse);
		};

		struct CreateKeyFileResponse : CoreResponse {
			DEC_SERIALIZABLE(CreateKeyFileResponse);
		};

		struct MountVolumeResponse : CoreResponse {
			MountVolumeResponse();
			bool readOnlyFailover;
			QSharedPointer<Volume::VolumeInformation> volumeInfo;
			DEC_SERIALIZABLE(MountVolumeResponse);
		};

		struct DismountVolumeResponse : CoreResponse {
			QList<QSharedPointer<QFileInfo>> volumePath; // path of the file dismounted, not the mount point
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
            QSharedPointer<QFileInfo> Device;
            QSharedPointer<QFileInfo> MountPoint;
            QString Type;
            DEC_SERIALIZABLE(MountedFilesystem);
		};

		struct HostDevice {
            QSharedPointer<QFileInfo> mountPoint;
            QSharedPointer<QFileInfo> devicePath;
			quint64 size;
			QList<QSharedPointer<HostDevice>> partitions;
			DEC_SERIALIZABLE(HostDevice);
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

#endif // CORERESPONSE_H
