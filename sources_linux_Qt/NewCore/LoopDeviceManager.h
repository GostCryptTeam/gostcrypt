#ifndef LOOPDEVICEMANAGER_H
#define LOOPDEVICEMANAGER_H

#include <QSharedPointer>
#include <QFileInfo>

namespace GostCrypt {
    namespace NewCore {
        class LoopDeviceManager
        {
        public:
            QSharedPointer<QFileInfo> attachLoopDevice(QSharedPointer<QFileInfo> imageFile, bool readonly);
            bool attachLoopDevice(QSharedPointer<QFileInfo> imageFile);
        };
    }
}

#endif // LOOPDEVICEMANAGER_H
