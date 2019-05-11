#include "MountFilesystemManager.h"
#include "CoreException.h"
#include <sys/mount.h>
#include <blkid/blkid.h>

namespace GostCrypt
{
namespace Core
{


void MountFilesystemManager::mountFilesystem(const QFileInfo devicePath, const QFileInfo mountPoint,
        QString filesystemType, bool readOnly, const uid_t ownerUID, const gid_t ownerGID,
        const QString& additionalMountOptions)
{
    quint64 mntflags =
        MS_NOSUID; //protect against potential privilege escalation using GostCrypt (not sure it is necessary)
    if (readOnly)
    {
        mntflags |= MS_RDONLY;
    }

    QString mountOptions;

    if (filesystemType.isEmpty())
    {
        filesystemType = getFileSystemType(devicePath);
    }

    if(filesystemType == "vfat") {
        mountOptions += "umask=077,uid=" + QString::number(ownerUID) + ",gid=" + QString::number(
                            ownerGID);
    }
    if (!additionalMountOptions.isEmpty())
    {
        if(!mountOptions.isEmpty())
            mountOptions += ",";
        mountOptions += additionalMountOptions;
    }

    if (mount(devicePath.absoluteFilePath().toLocal8Bit().data(),
              mountPoint.absoluteFilePath().toLocal8Bit().data(), filesystemType.toLocal8Bit().data(), mntflags,
              mountOptions.toLocal8Bit().data())) // TODO : use the all mighty fuse-ext2 please
    {
        throw FailMountFilesystemException(errno, mountPoint, devicePath, filesystemType);
    }
}

void MountFilesystemManager::dismountFilesystem(const QFileInfo mountPoint, bool force)
{
    if (umount2(mountPoint.absoluteFilePath().toLocal8Bit().data(), force ? MNT_FORCE : 0))
    {
        throw FailUnmountFilesystemException(errno, mountPoint);
    }
}

QString MountFilesystemManager::getFileSystemType(const QFileInfo devicePath)
{
    blkid_probe pr = blkid_new_probe_from_filename(devicePath.canonicalFilePath().toLocal8Bit().data());
    const char* type;
    QString typeStr;

    if (!pr)
    {
        throw FailFindFilesystemTypeException(devicePath);
    }
    blkid_do_probe(pr);
    blkid_probe_lookup_value(pr, "TYPE", &type, NULL);
    typeStr = QString::fromLocal8Bit(type);
    blkid_free_probe(pr);

    return typeStr;
}
}
}
