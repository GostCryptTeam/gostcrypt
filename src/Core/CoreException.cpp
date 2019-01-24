#include "CoreException.h"


namespace GostCrypt
{
namespace Core
{
void initCoreException()
{
    INIT_SERIALIZE(CoreException);
    INIT_SERIALIZE(FailedOpenFile);
    INIT_SERIALIZE(DeviceNotMounted);
    INIT_SERIALIZE(VolumeAlreadyMounted);
    INIT_SERIALIZE(FailedOpenVolume);
    INIT_SERIALIZE(MountPointUsed);
    INIT_SERIALIZE(MountFilesystemManagerException);
    INIT_SERIALIZE(FailMountFilesystem);
    INIT_SERIALIZE(FailUnmountFilesystem);
    INIT_SERIALIZE(FailedAttachLoopDevice);
    INIT_SERIALIZE(FailedCreateDirectory);
    INIT_SERIALIZE(FailedDetachLoopDevice);
    INIT_SERIALIZE(VolumeNotMounted);
    INIT_SERIALIZE(FormattingSubException);
    INIT_SERIALIZE(FormatProcessFailed);
    INIT_SERIALIZE(FilesystemNotSupported);
    INIT_SERIALIZE(AlgorithmNotFound);
    INIT_SERIALIZE(IncorrectSudoPassword);
    INIT_SERIALIZE(ServiceProcessCrashed);
    INIT_SERIALIZE(FailFindFilesystemType);
    INIT_SERIALIZE(FailedUsingSystemRandomSource);
}

DEF_SERIALIZABLE(GostCrypt::Core::CoreException)
QDataStream& operator << (QDataStream& out, const GostCrypt::Core::CoreException& Valeur)
{
    out << static_cast<const GostCryptException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Core::CoreException& Valeur)
{
    in >> static_cast<GostCryptException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Core::DeviceNotMounted)
QDataStream& operator << (QDataStream& out, const GostCrypt::Core::DeviceNotMounted& Valeur)
{
    out << static_cast<const CoreException&>(Valeur);
    out << Valeur.device;
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Core::DeviceNotMounted& Valeur)
{
    in >> static_cast<CoreException&>(Valeur);
    in >> Valeur.device;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Core::VolumeAlreadyMounted)
QDataStream& operator << (QDataStream& out, const GostCrypt::Core::VolumeAlreadyMounted& Valeur)
{
    out << static_cast<const CoreException&>(Valeur);
    out << Valeur.volumePath.absoluteFilePath();
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Core::VolumeAlreadyMounted& Valeur)
{
    QString path;
    in >> static_cast<CoreException&>(Valeur);
    in >> path;
    Valeur.volumePath.setFile(path);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Core::VolumeNotMounted)
QDataStream& operator << (QDataStream& out, const GostCrypt::Core::VolumeNotMounted& Valeur)
{
    out << static_cast<const CoreException&>(Valeur);
    out << Valeur.volumePath.absoluteFilePath();
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Core::VolumeNotMounted& Valeur)
{
    QString path;
    in >> static_cast<CoreException&>(Valeur);
    in >> path;
    Valeur.volumePath.setFile(path);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Core::FailedOpenVolume)
QDataStream& operator << (QDataStream& out, const GostCrypt::Core::FailedOpenVolume& Valeur)
{
    out << static_cast<const CoreException&>(Valeur);
    out << Valeur.volumePath.absoluteFilePath();
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Core::FailedOpenVolume& Valeur)
{
    QString path;
    in >> static_cast<CoreException&>(Valeur);
    in >> path;
    Valeur.volumePath.setFile(path);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Core::MountPointUsed)
QDataStream& operator << (QDataStream& out, const GostCrypt::Core::MountPointUsed& Valeur)
{
    out << static_cast<const CoreException&>(Valeur);
    out << Valeur.mountpoint;
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Core::MountPointUsed& Valeur)
{
    in >> static_cast<CoreException&>(Valeur);
    in >> Valeur.mountpoint;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Core::MountFilesystemManagerException)
QDataStream& operator << (QDataStream& out,
                          const GostCrypt::Core::MountFilesystemManagerException& Valeur)
{
    out << static_cast<const CoreException&>(Valeur);
    out << Valeur.mountpoint;
    out << Valeur.errorNumber;
    return out;
}
QDataStream& operator >> (QDataStream& in,
                          GostCrypt::Core::MountFilesystemManagerException& Valeur)
{
    in >> static_cast<CoreException&>(Valeur);
    in >> Valeur.mountpoint;
    in >> Valeur.errorNumber;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Core::FailMountFilesystem)
QDataStream& operator << (QDataStream& out, const GostCrypt::Core::FailMountFilesystem& Valeur)
{
    out << static_cast<const MountFilesystemManagerException&>(Valeur);
    out << Valeur.devicePath;
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Core::FailMountFilesystem& Valeur)
{
    in >> static_cast<MountFilesystemManagerException&>(Valeur);
    in >> Valeur.devicePath;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Core::FailUnmountFilesystem)
QDataStream& operator << (QDataStream& out, const GostCrypt::Core::FailUnmountFilesystem& Valeur)
{
    out << static_cast<const MountFilesystemManagerException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Core::FailUnmountFilesystem& Valeur)
{
    in >> static_cast<MountFilesystemManagerException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Core::FailedAttachLoopDevice)
QDataStream& operator << (QDataStream& out, const GostCrypt::Core::FailedAttachLoopDevice& Valeur)
{
    out << static_cast<const CoreException&>(Valeur);
    out << Valeur.imageFile;
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Core::FailedAttachLoopDevice& Valeur)
{
    in >> static_cast<CoreException&>(Valeur);
    in >> Valeur.imageFile;
    return in;
}
DEF_SERIALIZABLE(GostCrypt::Core::FailedDetachLoopDevice)
QDataStream& operator << (QDataStream& out, const GostCrypt::Core::FailedDetachLoopDevice& Valeur)
{
    out << static_cast<const CoreException&>(Valeur);
    out << Valeur.loopDevice;
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Core::FailedDetachLoopDevice& Valeur)
{
    in >> static_cast<CoreException&>(Valeur);
    in >> Valeur.loopDevice;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Core::FormattingSubException)
QDataStream& operator << (QDataStream& out, const GostCrypt::Core::FormattingSubException& Valeur)
{
    out << static_cast<const CoreException&>(Valeur);
    out << Valeur.subExceptionString;
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Core::FormattingSubException& Valeur)
{
    in >> static_cast<CoreException&>(Valeur);
    in >> Valeur.subExceptionString;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Core::FormatProcessFailed)
QDataStream& operator << (QDataStream& out, const GostCrypt::Core::FormatProcessFailed& Valeur)
{
    out << static_cast<const CoreException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Core::FormatProcessFailed& Valeur)
{
    in >> static_cast<CoreException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Core::FilesystemNotSupported)
QDataStream& operator << (QDataStream& out, const GostCrypt::Core::FilesystemNotSupported& Valeur)
{
    out << static_cast<const CoreException&>(Valeur);
    out << Valeur.filesystem;
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Core::FilesystemNotSupported& Valeur)
{
    in >> static_cast<CoreException&>(Valeur);
    in >> Valeur.filesystem;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Core::AlgorithmNotFound)
QDataStream& operator << (QDataStream& out, const GostCrypt::Core::AlgorithmNotFound& Valeur)
{
    out << static_cast<const CoreException&>(Valeur);
    out << Valeur.algorithm;
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Core::AlgorithmNotFound& Valeur)
{
    in >> static_cast<CoreException&>(Valeur);
    in >> Valeur.algorithm;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Core::IncorrectSudoPassword)
QDataStream& operator << (QDataStream& out, const GostCrypt::Core::IncorrectSudoPassword& Valeur)
{
    out << static_cast<const CoreException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Core::IncorrectSudoPassword& Valeur)
{
    in >> static_cast<CoreException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Core::ServiceProcessCrashed)
QDataStream& operator << (QDataStream& out, const GostCrypt::Core::ServiceProcessCrashed& Valeur)
{
    out << static_cast<const CoreException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Core::ServiceProcessCrashed& Valeur)
{
    in >> static_cast<CoreException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Core::FailFindFilesystemType)
QDataStream& operator << (QDataStream& out, const GostCrypt::Core::FailFindFilesystemType& Valeur)
{
    out << static_cast<const CoreException&>(Valeur);
    out << Valeur.devicePath;
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Core::FailFindFilesystemType& Valeur)
{
    in >> static_cast<CoreException&>(Valeur);
    in >> Valeur.devicePath;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Core::RandomNumberGeneratorNotRunning)
QDataStream& operator << (QDataStream& out,
                          const GostCrypt::Core::RandomNumberGeneratorNotRunning& Valeur)
{
    out << static_cast<const CoreException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in,
                          GostCrypt::Core::RandomNumberGeneratorNotRunning& Valeur)
{
    in >> static_cast<CoreException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Core::FailedUsingSystemRandomSource)
QDataStream& operator << (QDataStream& out,
                          const GostCrypt::Core::FailedUsingSystemRandomSource& Valeur)
{
    out << static_cast<const CoreException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Core::FailedUsingSystemRandomSource& Valeur)
{
    in >> static_cast<CoreException&>(Valeur);
    return in;
}
}
}
