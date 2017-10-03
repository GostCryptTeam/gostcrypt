#ifndef MOUNTFILESYSTEMMANAGER_H
#define MOUNTFILESYSTEMMANAGER_H

#include <QSharedPointer>
#include <QFileInfo>
namespace GostCrypt {
    namespace NewCore {
        class MountFilesystemManager
        {
        public:
            static void mountFilesystem(const QSharedPointer<QFileInfo> devicePath, const QSharedPointer<QFileInfo> mountPoint, QString filesystemType, bool readOnly, const uid_t ownerUID, const gid_t ownerGID, const QString &additionalMountOptions);
            static void dismountFilesystem (const QSharedPointer<QFileInfo> mountPoint, bool force);
        private:
			static QString getFileSystemType(const QSharedPointer<QFileInfo> devicePath);
        };
	}
}

#endif // MOUNTFILESYSTEMMANAGER_H
