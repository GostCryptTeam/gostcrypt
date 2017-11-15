#include "CoreRequest.h"
#include <QDebug>

namespace GostCrypt
{
namespace Core
{

void initCoreRequest()
{
    INIT_SERIALIZE(CoreRequest);
    INIT_SERIALIZE(CreateVolumeRequest);
    INIT_SERIALIZE(CreateVolumeRequest::VolumeParams);
    INIT_SERIALIZE(ChangeVolumePasswordRequest);
    INIT_SERIALIZE(CreateKeyFileRequest);
    INIT_SERIALIZE(MountVolumeRequest);
    INIT_SERIALIZE(DismountVolumeRequest);
    INIT_SERIALIZE(GetHostDevicesRequest);
    INIT_SERIALIZE(GetMountedVolumesRequest);
    INIT_SERIALIZE(QFileInfo);
    INIT_SERIALIZE(GetEncryptionAlgorithmsRequest);
    INIT_SERIALIZE(GetDerivationFunctionsRequest);
    INIT_SERIALIZE(ProgressTrackingParameters);
}

QDataStream& operator<< (QDataStream& out, const CoreRequest& Valeur)
{
    out << Valeur.emitResponse;
    out << Valeur.id;
    return out;
}
QDataStream& operator>> (QDataStream& in, CoreRequest& Valeur)
{
    in >> Valeur.emitResponse;
    in >> Valeur.id;
    return in;
}
DEF_SERIALIZABLE(CoreRequest)

QDataStream& operator << (QDataStream& out, const CreateVolumeRequest& Valeur)
{
    out << static_cast<const CoreRequest&>(Valeur);
    out << Valeur.path;
    out << (quint32)Valeur.type;
    out << Valeur.size;
    out << Valeur.outerVolume;
    out << Valeur.innerVolume;
    return out;
}
QDataStream& operator >> (QDataStream& in, CreateVolumeRequest& Valeur)
{
    quint32 tmp;
    in >> static_cast<CoreRequest&>(Valeur);
    in >> Valeur.path;
    in >> tmp;
    Valeur.type = VolumeType::Enum(tmp);
    in >> Valeur.size;
    in >> Valeur.outerVolume;
    in >> Valeur.innerVolume;
    return in;
}
DEF_SERIALIZABLE(CreateVolumeRequest)

QDataStream& operator << (QDataStream& out, const CreateVolumeRequest::VolumeParams& Valeur)
{
    out << Valeur.password;
    out << Valeur.keyfiles;
    out << Valeur.size;
    out << Valeur.volumeHeaderKdf;
    out << Valeur.encryptionAlgorithm;
    out << Valeur.filesystem;
    return out;
}
QDataStream& operator >> (QDataStream& in, CreateVolumeRequest::VolumeParams& Valeur)
{
    in >> Valeur.password;
    in >> Valeur.keyfiles;
    in >> Valeur.size;
    in >> Valeur.volumeHeaderKdf;
    in >> Valeur.encryptionAlgorithm;
    in >> Valeur.filesystem;
    return in;
}
DEF_SERIALIZABLE(CreateVolumeRequest::VolumeParams)

QDataStream& operator << (QDataStream& out, const ChangeVolumePasswordRequest& Valeur)
{
    out << static_cast<const CoreRequest&>(Valeur);
    out << Valeur.path;
    out << Valeur.password;
    out << Valeur.keyfiles;
    out << Valeur.newVolumeHeaderKdf;
    out << Valeur.newPassword;
    out << Valeur.newKeyfiles;
    return out;
}
QDataStream& operator >> (QDataStream& in, ChangeVolumePasswordRequest& Valeur)
{
    in >> static_cast<CoreRequest&>(Valeur);
    in >> Valeur.path;
    in >> Valeur.password;
    in >> Valeur.keyfiles;
    in >> Valeur.newVolumeHeaderKdf;
    in >> Valeur.newPassword;
    in >> Valeur.newKeyfiles;
    return in;
}
DEF_SERIALIZABLE(ChangeVolumePasswordRequest)

QDataStream& operator << (QDataStream& out, const CreateKeyFileRequest& Valeur)
{
    out << static_cast<const CoreRequest&>(Valeur);
    out << Valeur.file;
    return out;
}
QDataStream& operator >> (QDataStream& in, CreateKeyFileRequest& Valeur)
{
    in >> static_cast<CoreRequest&>(Valeur);
    in >> Valeur.file;
    return in;
}
DEF_SERIALIZABLE(CreateKeyFileRequest)

QDataStream& operator << (QDataStream& out, const MountVolumeRequest& Valeur)
{
    out << static_cast<const CoreRequest&>(Valeur);
    out << Valeur.fileSystemOptions;
    out << Valeur.fileSystemType;
    out << Valeur.doMount;
    out << Valeur.isDevice;
    out << Valeur.preserveTimestamps;
    out << Valeur.keyfiles;
    out << Valeur.password;
    out << Valeur.mountPoint;
    out << Valeur.path;
    out << Valeur.fuseMountPoint;
    out << (quint32)Valeur.protection;
    out << Valeur.protectionPassword;
    out << Valeur.protectionKeyfiles;
    out << Valeur.useBackupHeaders;
    out << Valeur.sharedAccessAllowed;
    out << Valeur.mountedForUser;
    out << Valeur.mountedForGroup;
    out << Valeur.forVolumeCreation;
    return out;
}
QDataStream& operator >> (QDataStream& in, MountVolumeRequest& Valeur)
{
    in >> static_cast<CoreRequest&>(Valeur);
    quint32 tmp;
    in >> Valeur.fileSystemOptions;
    in >> Valeur.fileSystemType;
    in >> Valeur.doMount;
    in >> Valeur.isDevice;
    in >> Valeur.preserveTimestamps;
    in >> Valeur.keyfiles;
    in >> Valeur.password;
    in >> Valeur.mountPoint;
    in >> Valeur.path;
    in >> Valeur.fuseMountPoint;
    in >> tmp;
    Valeur.protection = GostCrypt::VolumeProtection::Enum(tmp);
    in >> Valeur.protectionPassword;
    in >> Valeur.protectionKeyfiles;
    in >> Valeur.useBackupHeaders;
    in >> Valeur.sharedAccessAllowed;
    in >> Valeur.mountedForUser;
    in >> Valeur.mountedForGroup;
    in >> Valeur.forVolumeCreation;
    return in;
}
DEF_SERIALIZABLE(MountVolumeRequest)

QDataStream& operator << (QDataStream& out, const DismountVolumeRequest& Valeur)
{
    out << static_cast<const CoreRequest&>(Valeur);
    out << Valeur.volumePath;
    out << Valeur.force;
    out << Valeur.forVolumeCreation;
    return out;
}
QDataStream& operator >> (QDataStream& in, DismountVolumeRequest& Valeur)
{
    in >> static_cast<CoreRequest&>(Valeur);
    in >> Valeur.volumePath;
    in >> Valeur.force;
    in >> Valeur.forVolumeCreation;
    return in;
}
DEF_SERIALIZABLE(DismountVolumeRequest)

QDataStream& operator << (QDataStream& out, const GetHostDevicesRequest& Valeur)
{
    out << static_cast<const CoreRequest&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, GetHostDevicesRequest& Valeur)
{
    in >> static_cast<CoreRequest&>(Valeur);
    return in;
}
DEF_SERIALIZABLE(GetHostDevicesRequest)

QDataStream& operator << (QDataStream& out, const GetMountedVolumesRequest& Valeur)
{
    out << static_cast<const CoreRequest&>(Valeur);
    out << Valeur.volumePath;
    return out;
}
QDataStream& operator >> (QDataStream& in, GetMountedVolumesRequest& Valeur)
{
    in >> static_cast<CoreRequest&>(Valeur);
    in >> Valeur.volumePath;
    return in;
}
DEF_SERIALIZABLE(GetMountedVolumesRequest)

QDataStream& operator << (QDataStream& out, const GetEncryptionAlgorithmsRequest& Valeur)
{
    out << static_cast<const CoreRequest&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, GetEncryptionAlgorithmsRequest& Valeur)
{
    in >> static_cast<CoreRequest&>(Valeur);
    return in;
}
DEF_SERIALIZABLE(GetEncryptionAlgorithmsRequest)

QDataStream& operator << (QDataStream& out, const GetDerivationFunctionsRequest& Valeur)
{
    out << static_cast<const CoreRequest&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, GetDerivationFunctionsRequest& Valeur)
{
    in >> static_cast<CoreRequest&>(Valeur);
    return in;
}
DEF_SERIALIZABLE(GetDerivationFunctionsRequest)

QDataStream& operator << (QDataStream& out, const ProgressTrackingParameters& Valeur)
{
    out << Valeur.requestId;
    out << Valeur.start;
    out << Valeur.end;
    return out;
}
QDataStream& operator >> (QDataStream& in, ProgressTrackingParameters& Valeur)
{
    in >> Valeur.requestId;
    in >> Valeur.start;
    in >> Valeur.end;
    return in;
}
DEF_SERIALIZABLE(ProgressTrackingParameters)

MountVolumeRequest::MountVolumeRequest()
{
    this->doMount = true;
    this->fileSystemType = "vfat";
    this->preserveTimestamps = false;
    this->protection = VolumeProtection::Enum::None;
    this->sharedAccessAllowed = false;
    this->useBackupHeaders = false;
    this->forVolumeCreation = false;
}

DismountVolumeRequest::DismountVolumeRequest()
{
    this->force = false;
    this->forVolumeCreation = false;
}

}
}
