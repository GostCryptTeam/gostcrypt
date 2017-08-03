#include "MountFilesystemManager.h"
#include "CoreException.h"
#include <sys/mount.h>

namespace GostCrypt {
namespace NewCore {


        void MountFilesystemManager::MountFilesystem(const QSharedPointer<QFileInfo> devicePath, const QSharedPointer<QFileInfo> mountPoint, QString filesystemType, bool readOnly, const uid_t ownerUID, const gid_t ownerGID, const QString &additionalMountOptions)
        {
			quint64 mntflags = MS_NOSUID; //protect against potential privilege escalation using GostCrypt (not sure it is necessary)
			if(readOnly)
				mntflags |= MS_RDONLY;

			QString mountOptions;
			mountOptions += "uid=" + QString::number(ownerUID) + ",gid=" + QString::number(ownerGID) + ",umask=077";
			if(!additionalMountOptions.isEmpty())
				mountOptions += "," + additionalMountOptions;

            if(filesystemType.isNull() || filesystemType.isEmpty()) {
                //TODO use blkid libs to detect filesystem if not specified
                filesystemType = QString("vfat");
            }
            if(mount(devicePath->absoluteFilePath().toLocal8Bit().data(), mountPoint->absoluteFilePath().toLocal8Bit().data(), filesystemType.toLocal8Bit().data(), mntflags, mountOptions.toLocal8Bit().data()))
				throw FailMountFilesystemException(errno, mountPoint, devicePath);
        }


        void MountFilesystemManager::DismountFilesystem(const QSharedPointer<QFileInfo> mountPoint, bool force)
        {
            if(umount2(mountPoint->absoluteFilePath().toLocal8Bit().data(), force ? MNT_FORCE : 0))
				throw FailUnmountFilesystemException(errno, mountPoint);
        }
    }
}
