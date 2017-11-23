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

DEF_SERIALIZABLE(ChangeVolumePasswordResponse)
QDataStream& operator << (QDataStream& out, const ChangeVolumePasswordResponse& Valeur)
{
    out << static_cast<const CoreResponse&>(Valeur);
    out << Valeur.changeMasterKey;
    return out;
}
QDataStream& operator >> (QDataStream& in, ChangeVolumePasswordResponse& Valeur)
{
    in >> static_cast<CoreResponse&>(Valeur);
    in >> Valeur.changeMasterKey;
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

/*
VolumeInformation::VolumeInformation(VolumeInfo v)
{
    encryptionAlgorithmName = QString::fromStdWString(v.EncryptionAlgorithmName);
    if (!v.AuxMountPoint.IsEmpty())
    {
        fuseMountPoint.reset(new QFileInfo(QString::fromStdWString(v.AuxMountPoint)));
    }
    if (!v.MountPoint.IsEmpty())
    {
        mountPoint.reset(new QFileInfo(QString::fromStdWString(v.MountPoint)));
    }
    protection = v.Protection;
    size = v.Size;
    type = v.Type;
    if (!v.VirtualDevice.IsEmpty())
    {
        virtualDevice.reset(new QFileInfo(QString::fromStdWString(v.VirtualDevice)));
    }
    if (!v.Path.IsEmpty())
    {
        volumePath.reset(new QFileInfo(QString::fromStdWString(v.Path)));
    }
}//*/

MountVolumeResponse::MountVolumeResponse()
{
    this->readOnlyFailover = false;
}



}
}
