#ifndef CORERESPONSE_H
#define CORERESPONSE_H
#include <QVariant>
#include <QList>

#include "SerializationUtil.h"

namespace GostCrypt {
	namespace NewCore {
		static bool initCoreResponse();

		struct CoreResponse {};
		SERIALIZABLE(CoreResponse)

		struct CreateVolumeResponse : CoreResponse {};
		SERIALIZABLE(CreateVolumeResponse)

		struct ChangeVolumePasswordResponse : CoreResponse {};
		SERIALIZABLE(ChangeVolumePasswordResponse)

		struct CreateKeyFileResponse : CoreResponse {};
		SERIALIZABLE(CreateKeyFileResponse)

		struct MountVolumeResponse : CoreResponse {};
		SERIALIZABLE(MountVolumeResponse)

		struct DismountVolumeResponse : CoreResponse {};
		SERIALIZABLE(DismountVolumeResponse)

		struct GetHostDevicesResponse : CoreResponse {};
		SERIALIZABLE(GetHostDevicesResponse)

		struct GetMountedVolumesResponse : CoreResponse {
			QList<QSharedPointer<VolumeInfo>> volumeInfoList;
		};
		SERIALIZABLE(GetMountedVolumesResponse)
	}
}
#endif // CORERESPONSE_H
