#include "MountFilesystemManager.h"
#include "CoreException.h"
#include <sys/mount.h>
#include <blkid/blkid.h>

namespace GostCrypt {
namespace NewCore {


        void MountFilesystemManager::mountFilesystem(const QSharedPointer<QFileInfo> devicePath, const QSharedPointer<QFileInfo> mountPoint, QString filesystemType, bool readOnly, const uid_t ownerUID, const gid_t ownerGID, const QString &additionalMountOptions)
        {
			quint64 mntflags = MS_NOSUID; //protect against potential privilege escalation using GostCrypt (not sure it is necessary)
			if(readOnly)
				mntflags |= MS_RDONLY;

			QString mountOptions;
			mountOptions += "uid=" + QString::number(ownerUID) + ",gid=" + QString::number(ownerGID) + ",umask=077";
			if(!additionalMountOptions.isEmpty())
				mountOptions += "," + additionalMountOptions;

			if(filesystemType.isEmpty())
				filesystemType =getFileSystemType(devicePath);
			if(mount(devicePath->absoluteFilePath().toLocal8Bit().data(), mountPoint->absoluteFilePath().toLocal8Bit().data(), filesystemType.toLocal8Bit().data(), mntflags, mountOptions.toLocal8Bit().data()))
				throw FailMountFilesystemException(errno, mountPoint, devicePath);
        }


        void MountFilesystemManager::dismountFilesystem(const QSharedPointer<QFileInfo> mountPoint, bool force)
        {
            if(umount2(mountPoint->absoluteFilePath().toLocal8Bit().data(), force ? MNT_FORCE : 0))
				throw FailUnmountFilesystemException(errno, mountPoint);
		}

		QString MountFilesystemManager::getFileSystemType(const QSharedPointer<QFileInfo> devicePath)
		{
			blkid_probe pr = blkid_new_probe_from_filename(devicePath->canonicalFilePath().toLocal8Bit().data());
			const char *type;
			QString typeStr;

			if(!pr)
				throw FailFindFilesystemTypeException(devicePath);
			blkid_do_probe(pr);
			blkid_probe_lookup_value(pr, "TYPE", &type, NULL);
			typeStr = QString::fromLocal8Bit(type);
			blkid_free_probe(pr);

			return typeStr;
		}
    }
}
