#include "MountFilesystemManager.h"
#include "CoreException.h"
#include <sys/mount.h>

namespace GostCrypt {
namespace NewCore {


        void MountFilesystemManager::mountFilesystem(const QSharedPointer<QFileInfo> devicePath, const QSharedPointer<QFileInfo> mountPoint, QStringList possiblefilesystemTypes, bool readOnly, const uid_t ownerUID, const gid_t ownerGID, const QString &additionalMountOptions)
        {
			bool mounted = false;
			quint64 mntflags = MS_NOSUID; //protect against potential privilege escalation using GostCrypt (not sure it is necessary)
			if(readOnly)
				mntflags |= MS_RDONLY;

			QString mountOptions;
			mountOptions += "uid=" + QString::number(ownerUID) + ",gid=" + QString::number(ownerGID) + ",umask=077";
			if(!additionalMountOptions.isEmpty())
				mountOptions += "," + additionalMountOptions;

            for (QString filesystemType : possiblefilesystemTypes) {
				if(!mount(devicePath->absoluteFilePath().toLocal8Bit().data(), mountPoint->absoluteFilePath().toLocal8Bit().data(), filesystemType.toLocal8Bit().data(), mntflags, mountOptions.toLocal8Bit().data())) {
					mounted = true;
					break;
				}
			}
			if(!mounted)
				throw FailMountFilesystemException(errno, mountPoint, devicePath);
        }


        void MountFilesystemManager::dismountFilesystem(const QSharedPointer<QFileInfo> mountPoint, bool force)
        {
            if(umount2(mountPoint->absoluteFilePath().toLocal8Bit().data(), force ? MNT_FORCE : 0))
				throw FailUnmountFilesystemException(errno, mountPoint);
        }
    }
}
