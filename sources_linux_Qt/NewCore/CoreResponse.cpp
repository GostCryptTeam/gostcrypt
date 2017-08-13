#include "CoreResponse.h"


namespace GostCrypt {
    namespace NewCore {

		void initCoreResponse()
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
            INIT_SERIALIZE(ExceptionResponse);
		}

        DEF_SERIALIZABLE(CoreResponse)
        QDataStream & operator << (QDataStream & out, const CoreResponse& Valeur) {
            (void)Valeur;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, CoreResponse & Valeur) {
            (void)Valeur;
            return in;
        }

        DEF_SERIALIZABLE(InitResponse)
        QDataStream & operator << (QDataStream & out, const InitResponse& Valeur) {
            out << static_cast<const CoreResponse&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, InitResponse & Valeur) {
            in >> static_cast<CoreResponse&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(ExceptionResponse)
        QDataStream & operator << (QDataStream & out, const ExceptionResponse& Valeur) {
            out << static_cast<const CoreResponse&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, ExceptionResponse & Valeur) {
            in >> static_cast<CoreResponse&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(CreateVolumeResponse)
        QDataStream & operator << (QDataStream & out, const CreateVolumeResponse& Valeur) {
            out << static_cast<const CoreResponse&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, CreateVolumeResponse & Valeur) {
            in >> static_cast<CoreResponse&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(ChangeVolumePasswordResponse)
        QDataStream & operator << (QDataStream & out, const ChangeVolumePasswordResponse& Valeur) {
            out << static_cast<const CoreResponse&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, ChangeVolumePasswordResponse & Valeur) {
            in >> static_cast<CoreResponse&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(CreateKeyFileResponse)
        QDataStream & operator << (QDataStream & out, const CreateKeyFileResponse& Valeur) {
            out << static_cast<const CoreResponse&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, CreateKeyFileResponse & Valeur) {
            in >> static_cast<CoreResponse&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(MountVolumeResponse)
        QDataStream & operator << (QDataStream & out, const MountVolumeResponse& Valeur) {
            out << static_cast<const CoreResponse&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, MountVolumeResponse & Valeur) {
            in >> static_cast<CoreResponse&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(DismountVolumeResponse)
        QDataStream & operator << (QDataStream & out, const DismountVolumeResponse& Valeur) {
            out << static_cast<const CoreResponse&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, DismountVolumeResponse & Valeur) {
            in >> static_cast<CoreResponse&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(GetHostDevicesResponse)
        QDataStream & operator << (QDataStream & out, const GetHostDevicesResponse& Valeur) {
            out << static_cast<const CoreResponse&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GetHostDevicesResponse & Valeur) {
            in >> static_cast<CoreResponse&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(GetMountedVolumesResponse)
        QDataStream & operator << (QDataStream & out, const GetMountedVolumesResponse& Valeur) {
            out << static_cast<const CoreResponse&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GetMountedVolumesResponse & Valeur) {
            in >> static_cast<CoreResponse&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(GetFileSystemsTypesSupportedResponse)
        QDataStream & operator << (QDataStream & out, const GetFileSystemsTypesSupportedResponse& Valeur) {
            out << static_cast<const CoreResponse&>(Valeur);
            out << Valeur.filesystems;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GetFileSystemsTypesSupportedResponse & Valeur) {
            in >> static_cast<CoreResponse&>(Valeur);
            in >> Valeur.filesystems;
            return in;
        }

        DEF_SERIALIZABLE(GetEncryptionAlgorithmsResponse)
        QDataStream & operator << (QDataStream & out, const GetEncryptionAlgorithmsResponse& Valeur) {
            out << static_cast<const CoreResponse&>(Valeur);
            out << Valeur.algorithms;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GetEncryptionAlgorithmsResponse & Valeur) {
            in >> static_cast<CoreResponse&>(Valeur);
            in >> Valeur.algorithms;
            return in;
        }

        DEF_SERIALIZABLE(GetDerivationFunctionsResponse)
        QDataStream & operator << (QDataStream & out, const GetDerivationFunctionsResponse& Valeur) {
            out << static_cast<const CoreResponse&>(Valeur);
            out << Valeur.algorithms;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GetDerivationFunctionsResponse & Valeur) {
            in >> static_cast<CoreResponse&>(Valeur);
            in >> Valeur.algorithms;
            return in;
        }

    }
}
