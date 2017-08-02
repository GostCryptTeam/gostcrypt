#include "LoopDeviceManager.h"
#include "loopdev.h"
#include "CoreException.h"

#define FailedAttachLoopDeviceException SystemException
namespace GostCrypt {
namespace NewCore {

        QSharedPointer<QFileInfo> LoopDeviceManager::attachLoopDevice(QSharedPointer<QFileInfo> imageFile, bool readonly)
        {
            loopdev_cxt lc;
            quint32 lo_flags = 0;

            if(!loopcxt_init(&lc, 0)) //not sure necessary
                throw FailedAttachLoopDeviceException(imageFile);
            do {
                if(readonly)
                    lo_flags |= LO_FLAGS_READ_ONLY;
                if(loopcxt_find_unused(lc))
                    throw FailedAttachLoopDeviceException(imageFile);
                if(loopcxt_set_flags(lc, lo_flags))
                    throw FailedAttachLoopDeviceException(imageFile);
                if(loopcxt_set_backing_file(lc, imageFile->canonicalFilePath().toLocal8Bit().data()))
                    throw FailedAttachLoopDeviceException(imageFile);
                if(loopcxt_setup_device(lc)) {
                    if(errno == EBUSY)
                        continue;
                    throw FailedAttachLoopDeviceException(imageFile);
                }
            } while(0);

            QSharedPointer<QFileInfo> response(new QFileInfo(QString(loopcxt_get_device(&lc))));
            loopcxt_deinit(&lc);

            return response;
        }

        bool LoopDeviceManager::attachLoopDevice(QSharedPointer<QFileInfo> imageFile)
        {
            return false;
        }

    }
}
