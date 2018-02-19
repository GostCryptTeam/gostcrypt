#ifndef MOUNTFILESYSTEMMANAGER_H
#define MOUNTFILESYSTEMMANAGER_H

#include <QSharedPointer>
#include <QFileInfo>
namespace GostCrypt {
    namespace Core {
        class MountFilesystemManager
        {
        public:
            static void mountFilesystem(const QFileInfo devicePath, const QFileInfo mountPoint, QString filesystemType, bool readOnly, const uid_t ownerUID, const gid_t ownerGID, const QString &additionalMountOptions);
            static void dismountFilesystem (const QFileInfo mountPoint, bool force);
        private:
            static QString getFileSystemType(const QFileInfo devicePath);
        };
	}
}

#endif // MOUNTFILESYSTEMMANAGER_H
