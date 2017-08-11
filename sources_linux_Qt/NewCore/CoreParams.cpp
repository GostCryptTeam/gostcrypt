#include "CoreParams.h"

namespace GostCrypt {
    namespace NewCore {

		void initCoreParams()
		{
			INIT_SERIALIZE(CoreParams);
			INIT_SERIALIZE(CreateVolumeParams);
			INIT_SERIALIZE(CreateVolumeParams::VolumeParams);
			INIT_SERIALIZE(ChangeVolumePasswordParams);
			INIT_SERIALIZE(CreateKeyFileParams);
			INIT_SERIALIZE(MountVolumeParams);
			INIT_SERIALIZE(DismountVolumeParams);
			INIT_SERIALIZE(GetHostDevicesParams);
			INIT_SERIALIZE(GetMountedVolumesParams);
            INIT_SERIALIZE(GetFileSystemsTypesSupportedParams);
            INIT_SERIALIZE(QFileInfo);
            INIT_SERIALIZE(GetEncryptionAlgorithmsParams);
            INIT_SERIALIZE(GetDerivationFunctionsParams);
            INIT_SERIALIZE(ExitParams);
		}

        QDataStream & operator<< (QDataStream & out, const CoreParams & Valeur) {
           (void)Valeur;
           return out;
        }
        QDataStream & operator>> (QDataStream & in, CoreParams & Valeur) {
            (void)Valeur;
            return in;
        }
        DEF_SERIALIZABLE(CoreParams)

        QDataStream & operator << (QDataStream & out, const CreateVolumeParams & Valeur) {
            out << static_cast<const CoreParams&>(Valeur);
            out << (quint32)Valeur.type;
            out << Valeur.size;
            out << Valeur.path;
            out << Valeur.outerVolume;
            out << Valeur.innerVolume;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, CreateVolumeParams & Valeur) {
            quint32 tmp;
            in >> static_cast<CoreParams&>(Valeur);
            in >> tmp;
            Valeur.type = VolumeType::Enum(tmp);
            in >> Valeur.size;
            in >> Valeur.path;
            in >> Valeur.outerVolume;
            in >> Valeur.innerVolume;
            return in;
        }
        DEF_SERIALIZABLE(CreateVolumeParams)

        QDataStream & operator << (QDataStream & out, const CreateVolumeParams::VolumeParams & Valeur) {
        //    out << Valeur.password;
        //    out << Valeur.keyfiles;
            out << Valeur.volumeHeaderKdf;
            out << Valeur.encryptionAlgorithm;
            out << Valeur.filesystem;
            //out << Valeur.filesystemClusterSize;
            out << Valeur.size;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, CreateVolumeParams::VolumeParams & Valeur) {
        //    in >> Valeur.password;
        //    in >> Valeur.keyfiles;
            in >> Valeur.volumeHeaderKdf;
            in >> Valeur.encryptionAlgorithm;
            in >> Valeur.filesystem;
            //in >> Valeur.filesystemClusterSize;
            in >> Valeur.size;
            return in;
        }
        DEF_SERIALIZABLE(CreateVolumeParams::VolumeParams)

        QDataStream & operator << (QDataStream & out, const ChangeVolumePasswordParams & Valeur) {
            out << static_cast<const CoreParams&>(Valeur);
			out << Valeur.path;
        //    out << Valeur.password;
        //    out << Valeur.keyfiles;
        //    out << Valeur.newVolumeHeaderKdf;
        //    out << Valeur.newPassword;
        //    out << Valeur.newKeyfiles;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, ChangeVolumePasswordParams & Valeur) {
			in >> static_cast<CoreParams&>(Valeur);
            in >> Valeur.path;
        //    in >> Valeur.password;
        //    in >> Valeur.keyfiles;
        //    in >> Valeur.newVolumeHeaderKdf;
        //    in >> Valeur.newPassword;
        //    in >> Valeur.newKeyfiles;
            return in;
        }
        DEF_SERIALIZABLE(ChangeVolumePasswordParams)

        QDataStream & operator << (QDataStream & out, const CreateKeyFileParams & Valeur) {
            out << static_cast<const CoreParams&>(Valeur);
            out << Valeur.file;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, CreateKeyFileParams & Valeur) {
			in >> static_cast<CoreParams&>(Valeur);
            in >> Valeur.file;
            return in;
        }
        DEF_SERIALIZABLE(CreateKeyFileParams)

        QDataStream & operator << (QDataStream & out, const MountVolumeParams & Valeur) {
            out << static_cast<const CoreParams&>(Valeur);
            out << Valeur.fileSystemOptions;
            out << Valeur.fileSystemType;
            out << Valeur.doMount;
            out << Valeur.preserveTimestamps;
            //+out << Valeur.keyfiles;
            out << Valeur.password;
            out << Valeur.path;
            out << (quint32)Valeur.protection;
            out << Valeur.protectionPassword;
            //out << Valeur.protectionKeyfiles;
            out << Valeur.useBackupHeaders;
            out << Valeur.sharedAccessAllowed;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, MountVolumeParams & Valeur) {
			in >> static_cast<CoreParams&>(Valeur);
            quint32 tmp;
            in >> Valeur.fileSystemOptions;
            in >> Valeur.fileSystemType;
            in >> Valeur.doMount;
            in >> Valeur.preserveTimestamps;
            //in >> Valeur.keyfiles;
            in >> Valeur.password;
            in >> Valeur.path;
            in >> tmp;
            Valeur.protection = GostCrypt::VolumeProtection::Enum(tmp);
            in >> Valeur.protectionPassword;
            //in >> Valeur.protectionKeyfiles;
            in >> Valeur.useBackupHeaders;
            in >> Valeur.sharedAccessAllowed;
            return in;
        }
        DEF_SERIALIZABLE(MountVolumeParams)

        QDataStream & operator << (QDataStream & out, const DismountVolumeParams & Valeur) {
            out << static_cast<const CoreParams&>(Valeur);
            out << Valeur.volumepath;
            out << Valeur.force;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, DismountVolumeParams & Valeur) {
			in >> static_cast<CoreParams&>(Valeur);
            in >> Valeur.volumepath;
            in >> Valeur.force;
            return in;
        }
        DEF_SERIALIZABLE(DismountVolumeParams)

        QDataStream & operator << (QDataStream & out, const GetHostDevicesParams & Valeur) {
            out << static_cast<const CoreParams&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GetHostDevicesParams & Valeur) {
            in >> static_cast<CoreParams&>(Valeur);
            return in;
        }
        DEF_SERIALIZABLE(GetHostDevicesParams)

        QDataStream & operator << (QDataStream & out, const GetMountedVolumesParams & Valeur) {
            out << static_cast<const CoreParams&>(Valeur);
            out << Valeur.volumePath;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GetMountedVolumesParams & Valeur) {
            in >> static_cast<CoreParams&>(Valeur);
            in >> Valeur.volumePath;
            return in;
        }
        DEF_SERIALIZABLE(GetMountedVolumesParams)

        QDataStream & operator << (QDataStream & out, const GetFileSystemsTypesSupportedParams & Valeur) {
            out << static_cast<const CoreParams&>(Valeur);
            out << Valeur.volumetypefilter;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GetFileSystemsTypesSupportedParams & Valeur) {
			in >> static_cast<CoreParams&>(Valeur);
            in >> Valeur.volumetypefilter;
            return in;
        }
        DEF_SERIALIZABLE(GetFileSystemsTypesSupportedParams)

        QDataStream & operator << (QDataStream & out, const GetEncryptionAlgorithmsParams & Valeur) {
            out << static_cast<const CoreParams&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GetEncryptionAlgorithmsParams & Valeur) {
			in >> static_cast<CoreParams&>(Valeur);
            return in;
        }
        DEF_SERIALIZABLE(GetEncryptionAlgorithmsParams)

        QDataStream & operator << (QDataStream & out, const GetDerivationFunctionsParams & Valeur) {
            out << static_cast<const CoreParams&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GetDerivationFunctionsParams & Valeur) {
			in >> static_cast<CoreParams&>(Valeur);
            return in;
        }
        DEF_SERIALIZABLE(GetDerivationFunctionsParams)

        QDataStream & operator << (QDataStream & out, const ExitParams & Valeur) {
            out << static_cast<const CoreParams&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, ExitParams & Valeur) {
			in >> static_cast<CoreParams&>(Valeur);
            return in;
        }
        DEF_SERIALIZABLE(ExitParams)
    }
}


QDataStream & operator<< (QDataStream & out, const QFileInfo & Valeur){
    out << Valeur.canonicalFilePath();
    return out;
}

QDataStream & operator>> (QDataStream & in, QFileInfo & Valeur){
    QString path;
    in >> path;
    Valeur = QFileInfo(path);
    return in;
}
DEF_SERIALIZABLE(QFileInfo)
DEF_SERIALIZABLE(QByteArray)
