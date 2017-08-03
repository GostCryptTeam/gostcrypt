#ifndef LOOPDEVICEMANAGER_H
#define LOOPDEVICEMANAGER_H

#include <QSharedPointer>
#include <QFileInfo>

namespace GostCrypt {
    namespace NewCore {
        class LoopDeviceManager
        {
        public:
            static QSharedPointer<QFileInfo> attachLoopDevice(QSharedPointer<QFileInfo> imageFile, bool readonly);
            static bool detachLoopDevice(QSharedPointer<QFileInfo> imageFile);
        };
    }
}

#endif // LOOPDEVICEMANAGER_H
