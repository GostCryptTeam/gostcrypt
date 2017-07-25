#include "CoreUser.h"

namespace GostCrypt {
namespace NewCore {
		CoreUser::CoreUser()
		{

		}

		QSharedPointer<CreateVolumeResponse> CoreUser::createVolume(QSharedPointer<DismountVolumeParams> params)
		{

		}

		QSharedPointer<MountVolumeResponse> CoreBase::mountVolume(QSharedPointer<MountVolumeParams> params)
		{

		}

		QSharedPointer<DismountVolumeResponse> CoreUser::dismountVolume(QSharedPointer<DismountVolumeParams> params)
		{

		}

		QSharedPointer<CreateKeyFileResponse> CoreUser::createKeyFile(QSharedPointer<CreateKeyFileParams> params)
		{

		}

		QSharedPointer<ChangeVolumePasswordResponse> CoreUser::changeVolumePassword(QSharedPointer<ChangeVolumePasswordParams> params)
		{
			QSharedPointer<ChangeVolumePasswordResponse> response;
			rootProcessStream << QVariant::fromValue(params);
			rootProcessStream >> response;
			return response;
		}
	}
}
