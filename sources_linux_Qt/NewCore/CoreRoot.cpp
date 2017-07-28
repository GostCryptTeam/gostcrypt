#include "CoreRoot.h"
namespace GostCrypt {
	namespace NewCore {
		CoreRoot::CoreRoot()
		{
		}

		QSharedPointer<MountVolumeResponse> CoreRoot::mountVolume(QSharedPointer<MountVolumeParams> params)
		{
			if(!params)
				throw MissingParamException("params");

			if(isVolumeMounted(params->path))
				throw VolumeAlreadyMountedException(*params->path);


		}

		QSharedPointer<DismountVolumeResponse> CoreRoot::dismountVolume(QSharedPointer<DismountVolumeParams> params)
		{

		}

		QSharedPointer<CreateVolumeResponse> CoreRoot::createVolume(QSharedPointer<DismountVolumeParams> params)
		{

		}

		QSharedPointer<CreateKeyFileResponse> CoreRoot::createKeyFile(QSharedPointer<CreateKeyFileParams> params)
		{

		}

		QSharedPointer<ChangeVolumePasswordResponse> CoreRoot::changeVolumePassword(QSharedPointer<ChangeVolumePasswordParams> params)
		{

		}
	}
}
