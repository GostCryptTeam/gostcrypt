#include "CoreParams.h"


namespace GostCrypt {
namespace NewCore {

		bool initCoreParams()
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

            return true;
		}



	}
}

DEF_SERIALIZABLE(GostCrypt::NewCore::CoreParams)
QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::CoreParams & Valeur) {

return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::CoreParams & Valeur) {
    return in;
}

DEF_SERIALIZABLE(GostCrypt::NewCore::CreateVolumeParams)
QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::CreateVolumeParams & Valeur) {
//    out << Valeur.path; // TODO
    out << (quint32)Valeur.type;
    out << (quint64)Valeur.size;
    out << Valeur.outerVolume;
    out << Valeur.innerVolume;
    return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::CreateVolumeParams & Valeur) {
//    in >> Valeur.path; // TODO
    quint32 tmp;
    in >> tmp;
    Valeur.type = GostCrypt::VolumeType::Enum(tmp);
    in >> Valeur.size;
    in >> Valeur.outerVolume;
    in >> Valeur.innerVolume;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::NewCore::CreateVolumeParams::VolumeParams)
QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::CreateVolumeParams::VolumeParams & Valeur) {
//    out << Valeur.password;
//    out << Valeur.keyfiles;
//    out << Valeur.volumeHeaderKdf;
//    out << Valeur.encryptionAlgorithm;
    out << (quint32)Valeur.filesystem;
    out << Valeur.filesystemClusterSize;
    out << Valeur.sectorSize;
    return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::CreateVolumeParams::VolumeParams & Valeur) {
    quint32 tmp;
//    in >> Valeur.password;
//    in >> Valeur.keyfiles;
//    in >> Valeur.volumeHeaderKdf;
//    in >> Valeur.encryptionAlgorithm;
    in >> tmp;
    Valeur.filesystem = GostCrypt::NewCore::FilesystemType::Enum(tmp);
    in >> Valeur.filesystemClusterSize;
    in >> Valeur.sectorSize;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::NewCore::ChangeVolumePasswordParams)
QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::ChangeVolumePasswordParams & Valeur) {
//    out << Valeur.path;
//    out << Valeur.password;
//    out << Valeur.keyfiles;
//    out << Valeur.newVolumeHeaderKdf;
//    out << Valeur.newPassword;
//    out << Valeur.newKeyfiles;
    return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::ChangeVolumePasswordParams & Valeur) {
//    in >> Valeur.path;
//    in >> Valeur.password;
//    in >> Valeur.keyfiles;
//    in >> Valeur.newVolumeHeaderKdf;
//    in >> Valeur.newPassword;
//    in >> Valeur.newKeyfiles;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::NewCore::CreateKeyFileParams)
QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::CreateKeyFileParams & Valeur) {
//    out << Valeur.file;
    return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::CreateKeyFileParams & Valeur) {
//    in >> Valeur.file;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::NewCore::MountVolumeParams)
QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::MountVolumeParams & Valeur) {
    out << Valeur.fileSystemOptions;
    out << (quint32)Valeur.fileSystemType;
    out << Valeur.noFileSystem;
    out << Valeur.preserveTimestamps;
    //out << Valeur.keyfiles;
    //out << Valeur.password;
    //out << Valeur.path;
    out << (quint32)Valeur.protection;
    out << Valeur.useBackupHeaders;
    out << Valeur.sharedAccessAllowed;
    return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::MountVolumeParams & Valeur) {
    quint32 tmp;
    in >> Valeur.fileSystemOptions;
    in >> tmp;
    Valeur.protection = GostCrypt::VolumeProtection::Enum(tmp);
    in >> Valeur.noFileSystem;
    in >> Valeur.preserveTimestamps;
    //in >> Valeur.keyfiles;
    //in >> Valeur.password;
    //in >> Valeur.path;
    in >> tmp;
    Valeur.fileSystemType = GostCrypt::NewCore::FilesystemType::Enum(tmp);
    in >> Valeur.useBackupHeaders;
    in >> Valeur.sharedAccessAllowed;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::NewCore::DismountVolumeParams)
QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::DismountVolumeParams & Valeur) {
    out << Valeur.volumepath;
    return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::DismountVolumeParams & Valeur) {
    in >> Valeur.volumepath;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::NewCore::GetHostDevicesParams)
QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::GetHostDevicesParams & Valeur) {
    return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::GetHostDevicesParams & Valeur) {
    return in;
}

DEF_SERIALIZABLE(GostCrypt::NewCore::GetMountedVolumesParams)
QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::GetMountedVolumesParams & Valeur) {
    return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::GetMountedVolumesParams & Valeur) {
    return in;
}
