#ifndef LOOPDEVICEMANAGER_H
#define LOOPDEVICEMANAGER_H

#include <QSharedPointer>
#include <QFileInfo>

namespace GostCrypt {
    namespace Core {
        class LoopDeviceManager
        {
        public:
            static QFileInfo attachLoopDevice(QFileInfo imageFile, bool readonly);
            static void detachLoopDevice(QFileInfo loopDevice);
        };
    }
}

#endif // LOOPDEVICEMANAGER_H
