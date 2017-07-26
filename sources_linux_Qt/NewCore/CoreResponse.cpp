#include "CoreResponse.h"


namespace GostCrypt {
    namespace NewCore {

		bool initCoreResponse()
		{
			INIT_SERIALIZE(CoreResponse);
			INIT_SERIALIZE(CreateVolumeResponse);
			INIT_SERIALIZE(ChangeVolumePasswordResponse);
			INIT_SERIALIZE(CreateKeyFileResponse);
			INIT_SERIALIZE(MountVolumeResponse);
			INIT_SERIALIZE(DismountVolumeResponse);
			INIT_SERIALIZE(GetHostDevicesResponse);
			INIT_SERIALIZE(GetMountedVolumesResponse);

            return true;
		}

        DEF_SERIALIZABLE(CoreResponse)
        QDataStream & operator << (QDataStream & out, const CoreResponse& Valeur) {
            return out;
        }
        QDataStream & operator >> (QDataStream & in, CoreResponse & Valeur) {
            return in;
        }

        DEF_SERIALIZABLE(CreateVolumeResponse)
        QDataStream & operator << (QDataStream & out, const CreateVolumeResponse& Valeur) {
            return out;
        }
        QDataStream & operator >> (QDataStream & in, CreateVolumeResponse & Valeur) {
            return in;
        }

        DEF_SERIALIZABLE(ChangeVolumePasswordResponse)
        QDataStream & operator << (QDataStream & out, const ChangeVolumePasswordResponse& Valeur) {
            return out;
        }
        QDataStream & operator >> (QDataStream & in, ChangeVolumePasswordResponse & Valeur) {
            return in;
        }

        DEF_SERIALIZABLE(CreateKeyFileResponse)
        QDataStream & operator << (QDataStream & out, const CreateKeyFileResponse& Valeur) {
            return out;
        }
        QDataStream & operator >> (QDataStream & in, CreateKeyFileResponse & Valeur) {
            return in;
        }

        DEF_SERIALIZABLE(MountVolumeResponse)
        QDataStream & operator << (QDataStream & out, const MountVolumeResponse& Valeur) {
            return out;
        }
        QDataStream & operator >> (QDataStream & in, MountVolumeResponse & Valeur) {
            return in;
        }

        DEF_SERIALIZABLE(DismountVolumeResponse)
        QDataStream & operator << (QDataStream & out, const DismountVolumeResponse& Valeur) {
            return out;
        }
        QDataStream & operator >> (QDataStream & in, DismountVolumeResponse & Valeur) {
            return in;
        }

        DEF_SERIALIZABLE(GetHostDevicesResponse)
        QDataStream & operator << (QDataStream & out, const GetHostDevicesResponse& Valeur) {
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GetHostDevicesResponse & Valeur) {
            return in;
        }

        DEF_SERIALIZABLE(GetMountedVolumesResponse)
        QDataStream & operator << (QDataStream & out, const GetMountedVolumesResponse& Valeur) {
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GetMountedVolumesResponse & Valeur) {
            return in;
        }

    }
}
