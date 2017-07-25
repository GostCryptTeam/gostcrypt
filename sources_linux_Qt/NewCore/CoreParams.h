#ifndef COREPARAMS_H
#define COREPARAMS_H
#include <QVariant>
#include "SerializationUtil.h"
#include "Volume/Volume.h"

namespace GostCrypt {
	namespace NewCore {
		static bool initCoreParams();

		struct CoreParams {};
		SERIALIZABLE(CoreParams)

		struct CreateVolumeParams : CoreParams {};
		SERIALIZABLE(CoreCreateVolumeParams)

		struct ChangeVolumePasswordParams : CoreParams {};
		SERIALIZABLE(CoreChangeVolumePasswordParams)

		struct CreateKeyFileParams : CoreParams {};
		SERIALIZABLE(CoreCreateKeyFileParams)

		struct MountVolumeParams : CoreParams {};
		SERIALIZABLE(CoreMountVolumeParams)

		struct DismountVolumeParams : CoreParams {};
		SERIALIZABLE(CoreDismountVolumeParams)

		struct GetHostDevicesParams : CoreParams {};
		SERIALIZABLE(GetHostDevicesParams)

		struct GetMountedVolumesParams : CoreParams {
			VolumePath volumePath;
		};
		SERIALIZABLE(GetMountedVolumesParams)
	}
}
#endif // COREPARAMS_H
