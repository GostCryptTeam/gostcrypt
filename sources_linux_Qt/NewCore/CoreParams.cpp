#include "CoreParams.h"


namespace GostCrypt {
namespace NewCore {

		bool initCoreParams()
		{
			INIT_SERIALIZE(CoreParams);
			INIT_SERIALIZE(CoreCreateVolumeParams);
			INIT_SERIALIZE(CoreChangeVolumePasswordParams);
			INIT_SERIALIZE(CoreCreateKeyFileParams);
			INIT_SERIALIZE(CoreMountVolumeParams);
			INIT_SERIALIZE(CoreDismountVolumeParams);
		}

	}
}
