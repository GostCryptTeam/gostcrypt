#include "CoreResponse.h"


namespace GostCrypt
{
namespace Core
{

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
    INIT_SERIALIZE(GetEncryptionAlgorithmsResponse);
    INIT_SERIALIZE(GetDerivationFunctionsResponse);
    INIT_SERIALIZE(HostDevice);
    INIT_SERIALIZE(MountedFilesystem);
    INIT_SERIALIZE(BackupHeaderResponse);
    INIT_SERIALIZE(RestoreHeaderResponse);
}

DEF_SERIALIZABLE(CoreResponse)
QDataStream& operator << (QDataStream& out, const CoreResponse& Valeur)
{
    Q_UNUSED(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, CoreResponse& Valeur)
{
    Q_UNUSED(Valeur);
    return in;
}

DEF_SERIALIZABLE(CreateVolumeResponse)
QDataStream& operator << (QDataStream& out, const CreateVolumeResponse& Valeur)
{
    out << static_cast<const CoreResponse&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, CreateVolumeResponse& Valeur)
{
    in >> static_cast<CoreResponse&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(BackupHeaderResponse)
QDataStream& operator << (QDataStream& out, const BackupHeaderResponse& Valeur)
{
    out << static_cast<const CoreResponse&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, BackupHeaderResponse& Valeur)
{
    in >> static_cast<CoreResponse&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(RestoreHeaderResponse)
QDataStream& operator << (QDataStream& out, const RestoreHeaderResponse& Valeur)
{
    out << static_cast<const CoreResponse&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, RestoreHeaderResponse& Valeur)
{
    in >> static_cast<CoreResponse&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(ChangeVolumePasswordResponse)
QDataStream& operator << (QDataStream& out, const ChangeVolumePasswordResponse& Valeur)
{
    out << static_cast<const CoreResponse&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, ChangeVolumePasswordResponse& Valeur)
{
    in >> static_cast<CoreResponse&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(CreateKeyFileResponse)
QDataStream& operator << (QDataStream& out, const CreateKeyFileResponse& Valeur)
{
    out << static_cast<const CoreResponse&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, CreateKeyFileResponse& Valeur)
{
    in >> static_cast<CoreResponse&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(MountVolumeResponse)
QDataStream& operator << (QDataStream& out, const MountVolumeResponse& Valeur)
{
    out << static_cast<const CoreResponse&>(Valeur);
    out << Valeur.readOnlyFailover;
    out << Valeur.volumeInfo;
    return out;
}
QDataStream& operator >> (QDataStream& in, MountVolumeResponse& Valeur)
{
    in >> static_cast<CoreResponse&>(Valeur);
    in >> Valeur.readOnlyFailover;
    in >> Valeur.volumeInfo;
    return in;
}

DEF_SERIALIZABLE(DismountVolumeResponse)
QDataStream& operator << (QDataStream& out, const DismountVolumeResponse& Valeur)
{
    out << static_cast<const CoreResponse&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, DismountVolumeResponse& Valeur)
{
    in >> static_cast<CoreResponse&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GetHostDevicesResponse)
QDataStream& operator << (QDataStream& out, const GetHostDevicesResponse& Valeur)
{
    out << static_cast<const CoreResponse&>(Valeur);
    out << Valeur.hostDevices;
    return out;
}
QDataStream& operator >> (QDataStream& in, GetHostDevicesResponse& Valeur)
{
    in >> static_cast<CoreResponse&>(Valeur);
    in >> Valeur.hostDevices;
    return in;
}

DEF_SERIALIZABLE(GetMountedVolumesResponse)
QDataStream& operator << (QDataStream& out, const GetMountedVolumesResponse& Valeur)
{
    out << static_cast<const CoreResponse&>(Valeur);
    out << Valeur.volumeInfoList;
    return out;
}
QDataStream& operator >> (QDataStream& in, GetMountedVolumesResponse& Valeur)
{
    in >> static_cast<CoreResponse&>(Valeur);
    in >> Valeur.volumeInfoList;
    return in;
}

DEF_SERIALIZABLE(GetEncryptionAlgorithmsResponse)
QDataStream& operator << (QDataStream& out, const GetEncryptionAlgorithmsResponse& Valeur)
{
    out << static_cast<const CoreResponse&>(Valeur);
    out << Valeur.algorithms;
    return out;
}
QDataStream& operator >> (QDataStream& in, GetEncryptionAlgorithmsResponse& Valeur)
{
    in >> static_cast<CoreResponse&>(Valeur);
    in >> Valeur.algorithms;
    return in;
}

DEF_SERIALIZABLE(GetDerivationFunctionsResponse)
QDataStream& operator << (QDataStream& out, const GetDerivationFunctionsResponse& Valeur)
{
    out << static_cast<const CoreResponse&>(Valeur);
    out << Valeur.algorithms;
    return out;
}
QDataStream& operator >> (QDataStream& in, GetDerivationFunctionsResponse& Valeur)
{
    in >> static_cast<CoreResponse&>(Valeur);
    in >> Valeur.algorithms;
    return in;
}

DEF_SERIALIZABLE(HostDevice)
QDataStream& operator << (QDataStream& out, const HostDevice& Valeur)
{
    out << Valeur.mountPoint;
    out << Valeur.devicePath;
    out << Valeur.size;
    out << Valeur.partitions;
    return out;
}
QDataStream& operator >> (QDataStream& in, HostDevice& Valeur)
{
    in >> Valeur.mountPoint;
    in >> Valeur.devicePath;
    in >> Valeur.size;
    in >> Valeur.partitions;
    return in;
}

DEF_SERIALIZABLE(MountedFilesystem)
QDataStream& operator << (QDataStream& out, const MountedFilesystem& Valeur)
{
    out << Valeur.Device;
    out << Valeur.MountPoint;
    out << Valeur.Type;
    return out;
}
QDataStream& operator >> (QDataStream& in, MountedFilesystem& Valeur)
{
    in >> Valeur.Device;
    in >> Valeur.MountPoint;
    in >> Valeur.Type;
    return in;
}

MountVolumeResponse::MountVolumeResponse()
{
    this->readOnlyFailover = false;
}



}
}
