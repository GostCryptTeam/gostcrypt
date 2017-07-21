#include "CoreResponse.h"


namespace GostCrypt {
namespace NewCore {

		bool initCoreResponse()
		{
			INIT_SERIALIZE(CoreResponse);
			INIT_SERIALIZE(CoreCreateVolumeResponse);
			INIT_SERIALIZE(CoreChangeVolumePasswordResponse);
			INIT_SERIALIZE(CoreCreateKeyFileResponse);
			INIT_SERIALIZE(CoreMountVolumeResponse);
			INIT_SERIALIZE(CoreDismountVolumeResponse);
		}

	}
}
