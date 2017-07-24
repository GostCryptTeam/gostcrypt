#include "CoreParams.h"


namespace GostCrypt {
namespace NewCore {

		bool initCoreParams()
		{
			INIT_SERIALIZE(CoreParams);
			INIT_SERIALIZE(CreateVolumeParams);
			INIT_SERIALIZE(ChangeVolumePasswordParams);
			INIT_SERIALIZE(CreateKeyFileParams);
			INIT_SERIALIZE(MountVolumeParams);
			INIT_SERIALIZE(DismountVolumeParams);
			INIT_SERIALIZE(GetHostDevicesParams);
			INIT_SERIALIZE(GetMountedVolumesParams);
		}
	}
}
