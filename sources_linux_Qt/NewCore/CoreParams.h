#ifndef COREPARAMS_H
#define COREPARAMS_H
#include <QVariant>
#include "SerializationUtil.h"

namespace GostCrypt {
	namespace NewCore {
		static bool initCoreParams();

		struct CoreParams {};
		SERIALIZABLE(CoreParams);

		struct CoreCreateVolumeParams : CoreParams {};
		SERIALIZABLE(CoreCreateVolumeParams);

		struct CoreChangeVolumePasswordParams : CoreParams {};
		SERIALIZABLE(CoreChangeVolumePasswordParams);

		struct CoreCreateKeyFileParams : CoreParams {};
		SERIALIZABLE(CoreCreateKeyFileParams);

		struct CoreMountVolumeParams : CoreParams {};
		SERIALIZABLE(CoreMountVolumeParams);

		struct CoreDismountVolumeParams : CoreParams {};
		SERIALIZABLE(CoreDismountVolumeParams);
	}
}
#endif // COREPARAMS_H
