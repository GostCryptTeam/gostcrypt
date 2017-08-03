#include "CoreUser.h"

namespace GostCrypt {
namespace NewCore {
		CoreUser::CoreUser()
		{

        }

        QSharedPointer<MountVolumeResponse> CoreUser::mountVolume(QSharedPointer<MountVolumeParams> params)
        {
            QSharedPointer<MountVolumeResponse> response;

            forwardRoot(QVariant::fromValue(params));
            return response;
        }

        QSharedPointer<DismountVolumeResponse> CoreUser::dismountVolume(QSharedPointer<DismountVolumeParams> params)
        {

        }

        QSharedPointer<CreateVolumeResponse> CoreUser::createVolume(QSharedPointer<DismountVolumeParams> params)
        {

        }

        QSharedPointer<CreateKeyFileResponse> CoreUser::createKeyFile(QSharedPointer<CreateKeyFileParams> params)
        {

        }

        QSharedPointer<ChangeVolumePasswordResponse> CoreUser::changeVolumePassword(QSharedPointer<ChangeVolumePasswordParams> params)
        {

        }

        QVariant CoreUser::forwardRoot(QVariant params)
        {

        }

        void CoreUser::sendAdminPassword()
        {

        }
	}
}
