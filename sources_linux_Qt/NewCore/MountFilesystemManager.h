#ifndef MOUNTFILESYSTEMMANAGER_H
#define MOUNTFILESYSTEMMANAGER_H

#include <QSharedPointer>
#include <QFileInfo>
namespace GostCrypt {
    namespace NewCore {
        class MountFilesystemManager
        {
        public:
			void MountFilesystem(const QSharedPointer<QFileInfo> devicePath, const QSharedPointer<QFileInfo> mountPoint, const QString &filesystemType, bool readOnly, const uid_t ownerUID, const gid_t ownerGID, const QString additionalMountOptions);
            void DismountFilesystem (const QSharedPointer<QFileInfo> mountPoint, bool force);
        };
	}
}

#endif // MOUNTFILESYSTEMMANAGER_H
