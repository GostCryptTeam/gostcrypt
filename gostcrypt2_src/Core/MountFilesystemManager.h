#ifndef MOUNTFILESYSTEMMANAGER_H
#define MOUNTFILESYSTEMMANAGER_H

#include <QSharedPointer>
#include <QFileInfo>
namespace GostCrypt {
    namespace Core {
        /**
         * @brief Class containing the methods managing the mounting of file system.
         *
         */
        class MountFilesystemManager
        {
        public:
            /**
             * @brief Mount the given block file in the given target mount point using the mount system call
             *
             * @param devicePath Path of the block file
             * @param mountPoint Path of the target directory where to mount the device
             * @param filesystemType Type of the file system contained in the device. If empty it will try to guess the type of the file system.
             * @param readOnly Boolean true if the device should be mounted in read-only mode.
             * @param ownerUID User ID of the owner of the mounted files
             * @param ownerGID Group ID of the owner of the mounted files
             * @param additionalMountOptions Additional options to pass to the file system
             */
            static void mountFilesystem(const QFileInfo devicePath, const QFileInfo mountPoint, QString filesystemType, bool readOnly, const uid_t ownerUID, const gid_t ownerGID, const QString &additionalMountOptions);
            /**
             * @brief Dismount a filesystem using the umount2 system call
             *
             * @param mountPoint Path of the directory where the device is mounted
             * @param force Boolean true to force the dismounting of the volume even if busy. This can cause data loss.
             */
            static void dismountFilesystem (const QFileInfo mountPoint, bool force);
        private:
            /**
             * @brief Return the name of the file system type contained in the given block file
             *
             * @param devicePath Path of the block file for which to guess the file system type
             * @return QString Name of the file system type contained in the given block file
             */
            static QString getFileSystemType(const QFileInfo devicePath);
        };
	}
}

#endif // MOUNTFILESYSTEMMANAGER_H
