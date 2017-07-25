#ifndef CORERESPONSE_H
#define CORERESPONSE_H
#include <QVariant>
<<<<<<< HEAD
#include <QList>

=======
#include <QSharedPointer>
>>>>>>> 13fe05bc5bb7f62448a6df80656dc0ff28e6b1f4
#include "SerializationUtil.h"

namespace GostCrypt {
	namespace NewCore {
		static bool initCoreResponse();

		struct CoreResponse {};

		struct CreateVolumeResponse : CoreResponse {};

		struct ChangeVolumePasswordResponse : CoreResponse {};

		struct CreateKeyFileResponse : CoreResponse {};

		struct MountVolumeResponse : CoreResponse {};

		struct DismountVolumeResponse : CoreResponse {};

		struct GetHostDevicesResponse : CoreResponse {};

		struct GetMountedVolumesResponse : CoreResponse {
			QList<QSharedPointer<VolumeInfo>> volumeInfoList;
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








#endif // CORERESPONSE_H
