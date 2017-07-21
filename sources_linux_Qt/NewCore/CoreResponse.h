#ifndef CORERESPONSE_H
#define CORERESPONSE_H
#include <QVariant>

#include "SerializationUtil.h"

namespace GostCrypt {
	namespace NewCore {
		static bool initCoreResponse();

		struct CoreResponse {};
		SERIALIZABLE(CoreResponse);

		struct CoreCreateVolumeResponse : CoreResponse {};
		SERIALIZABLE(CoreCreateVolumeResponse);

		struct CoreChangeVolumePasswordResponse : CoreResponse {};
		SERIALIZABLE(CoreChangeVolumePasswordResponse);

		struct CoreCreateKeyFileResponse : CoreResponse {};
		SERIALIZABLE(CoreCreateKeyFileResponse);

		struct CoreMountVolumeResponse : CoreResponse {};
		SERIALIZABLE(CoreMountVolumeResponse);

		struct CoreDismountVolumeResponse : CoreResponse {};
		SERIALIZABLE(CoreDismountVolumeResponse);
	}
}
#endif // CORERESPONSE_H
