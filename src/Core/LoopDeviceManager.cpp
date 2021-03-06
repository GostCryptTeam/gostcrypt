#include "LoopDeviceManager.h"
extern "C" {
#include "loopdevlib/loopdev.h"
}
#include "CoreException.h"

namespace GostCrypt
{
namespace Core
{

QFileInfo LoopDeviceManager::attachLoopDevice(QFileInfo imageFile, bool readonly)
{
    loopdev_cxt lc;
    quint32 lo_flags = 0;

    if (loopcxt_init(&lc, 0)) //not sure necessary
    {
        throw FailedAttachLoopDeviceException(imageFile);
    }
    do
    {
        if (readonly)
        {
            lo_flags |= LO_FLAGS_READ_ONLY;
        }
        if (loopcxt_find_unused(&lc))
        {
            throw FailedAttachLoopDeviceException(imageFile);
        }
        if (loopcxt_set_flags(&lc, lo_flags))
        {
            throw FailedAttachLoopDeviceException(imageFile);
        }
        if (loopcxt_set_backing_file(&lc, imageFile.absoluteFilePath().toLocal8Bit().data()))
        {
            throw FailedAttachLoopDeviceException(imageFile);
        }
        if (loopcxt_setup_device(&lc))
        {
            if (errno == EBUSY)
            {
                continue;
            }
            throw FailedAttachLoopDeviceException(imageFile);
        }
    }
    while (0);

    QFileInfo response(QString(loopcxt_get_device(&lc)));
    loopcxt_deinit(&lc);

    return response;
}

void LoopDeviceManager::detachLoopDevice(QFileInfo loopDevice)
{
    loopdev_cxt lc;

    if (loopcxt_init(&lc, 0))
    {
        throw FailedDetachLoopDeviceException(loopDevice);
    }
    if (!is_loopdev(loopDevice.absoluteFilePath().toLocal8Bit().data()))
    {
        throw FailedDetachLoopDeviceException(loopDevice);
    }
    if (loopcxt_set_device(&lc, loopDevice.absoluteFilePath().toLocal8Bit().data()))
    {
        throw FailedDetachLoopDeviceException(loopDevice);
    }
    if (loopcxt_delete_device(&lc))
    {
        throw FailedDetachLoopDeviceException(loopDevice);
    }

    loopcxt_deinit(&lc);
}

}
}
