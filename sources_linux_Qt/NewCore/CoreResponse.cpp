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
            INIT_SERIALIZE(GetFileSystemsTypesSupportedResponse);
            INIT_SERIALIZE(GetEncryptionAlgorithmsResponse);
            INIT_SERIALIZE(GetDerivationFunctionsResponse);
            INIT_SERIALIZE(InitResponse);
            return true;
		}

        DEF_SERIALIZABLE(CoreResponse)
        QDataStream & operator << (QDataStream & out, const CoreResponse& Valeur) {
            return out;
        }
        QDataStream & operator >> (QDataStream & in, CoreResponse & Valeur) {
            return in;
        }

        DEF_SERIALIZABLE(InitResponse)
        QDataStream & operator << (QDataStream & out, const InitResponse& Valeur) {
            return out;
        }
        QDataStream & operator >> (QDataStream & in, InitResponse & Valeur) {
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

        DEF_SERIALIZABLE(GetFileSystemsTypesSupportedResponse)
        QDataStream & operator << (QDataStream & out, const GetFileSystemsTypesSupportedResponse& Valeur) {
            out << Valeur.filesystems;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GetFileSystemsTypesSupportedResponse & Valeur) {
            in >> Valeur.filesystems;
            return in;
        }

        DEF_SERIALIZABLE(GetEncryptionAlgorithmsResponse)
        QDataStream & operator << (QDataStream & out, const GetEncryptionAlgorithmsResponse& Valeur) {
            out << Valeur.algorithms;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GetEncryptionAlgorithmsResponse & Valeur) {
            in >> Valeur.algorithms;
            return in;
        }

        DEF_SERIALIZABLE(GetDerivationFunctionsResponse)
        QDataStream & operator << (QDataStream & out, const GetDerivationFunctionsResponse& Valeur) {
            out << Valeur.algorithms;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GetDerivationFunctionsResponse & Valeur) {
            in >> Valeur.algorithms;
            return in;
        }

    }
}
