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

		struct CoreResponse {
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

		struct MountVolumeResponse : CoreResponse {
			DEC_SERIALIZABLE(MountVolumeResponse);
		};

		struct DismountVolumeResponse : CoreResponse {
			DEC_SERIALIZABLE(DismountVolumeResponse);
		};

		struct GetHostDevicesResponse : CoreResponse {
			DEC_SERIALIZABLE(GetHostDevicesResponse);
		};

		struct GetMountedVolumesResponse : CoreResponse {
			QList<QSharedPointer<VolumeInfo>> volumeInfoList;
			DEC_SERIALIZABLE(GetMountedVolumesResponse);
		};

        struct GetFileSystemTypesResponse : CoreResponse {
            QStringList filesystems;
            DEC_SERIALIZABLE(GetFileSystemTypesResponse);
        };

		struct MountedFilesystem
		{
			QFileInfo Device;
			QFileInfo MountPoint;
            QString Type;
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
SERIALIZABLE(GostCrypt::NewCore::GetFileSystemTypesResponse)








#endif // CORERESPONSE_H
