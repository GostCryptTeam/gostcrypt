#ifndef LOOPDEVICEMANAGER_H
#define LOOPDEVICEMANAGER_H

#include <QSharedPointer>
#include <QFileInfo>

namespace GostCrypt
{
namespace Core
{
/**
 * @brief Class containing the methods managing the loop device. The loop device is a block file pointing to a regular file
 *
 */
class LoopDeviceManager
{
 public:
    /**
     * @brief Attach a loop device to the given imageFile
     *
     * @param imageFile Path of the image file to which the loop device should be attached
     * @param readonly Boolean true if the loop device should be in read-only mode
     * @return QFileInfo Path to the loop device now attached to the image file given in parameters
     */
    static QFileInfo attachLoopDevice(QFileInfo imageFile, bool readonly);
    /**
     * @brief Detach a loop device from its regular file
     *
     * @param loopDevice Path to the loop device to detach
     */
    static void detachLoopDevice(QFileInfo loopDevice);
};
}
}

#endif // LOOPDEVICEMANAGER_H
