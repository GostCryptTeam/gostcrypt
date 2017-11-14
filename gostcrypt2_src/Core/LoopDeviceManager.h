#ifndef LOOPDEVICEMANAGER_H
#define LOOPDEVICEMANAGER_H

#include <QSharedPointer>
#include <QFileInfo>

namespace GostCrypt {
    namespace Core {
        class LoopDeviceManager
        {
        public:
            static QSharedPointer<QFileInfo> attachLoopDevice(QSharedPointer<QFileInfo> imageFile, bool readonly);
            static void detachLoopDevice(QSharedPointer<QFileInfo> loopDevice);
        };
    }
}

#endif // LOOPDEVICEMANAGER_H
