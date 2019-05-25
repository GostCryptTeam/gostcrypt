#include "CoreUser.h"
#include <QThread>
#include "Service.h"


namespace GostCrypt
{
namespace Core
{
CoreUser::CoreUser(QObject* parent) : CoreBase(parent)
{
    connect(&csh, SIGNAL(sendResponse(QVariant&)), this, SLOT(receiveResponse(QVariant&)));
    connect(&csh, SIGNAL(sendProgressUpdate(quint32,qreal)), this, SIGNAL(sendProgressUpdate(quint32,qreal)));
    connect(&csh, SIGNAL(askSudoPassword()), this, SIGNAL(askSudoPassword()));
    connect(this, SIGNAL(sendSudoPassword(QSharedPointer<QByteArray>)), &csh,
            SLOT(receiveSudoPassword(QSharedPointer<QByteArray>)));
    connect(&csh, SIGNAL(exited()), this, SLOT(exit()));

}

void CoreUser::exit()
{
    if (csh.isRunning())
    {
        csh.exit();
    }
    else
    {
        // The main loop was not started, so an imediate call to app.quit() would not be working.
        this->CoreBase::exit();
        //QMetaObject::invokeMethod(this, "exited", Qt::QueuedConnection);
    }
}

void CoreUser::request(QVariant r)
{
    //TODO add other non-root requests
    if (!processNonRootRequest(r))
    {
        csh.sendToService(r);
    }
}

//TODO : remove the QSTRING with a c++ password class
void CoreUser::receiveSudoPassword(QString password)//QSharedPointer<QByteArray> password)
{
    emit sendSudoPassword(QSharedPointer<QByteArray>(new QByteArray(password.toLocal8Bit())));
}

void CoreUser::mountVolume(QSharedPointer<MountVolumeRequest> params)
{
    try
    {

        if (!params)
        {
            throw InvalidParameterException("params", "params is null.");
        }

        if (!params->path.isReadable() ||
            (!params->path.isWritable() && params->protection != Volume::VolumeProtection::ReadOnly)) {
            // this is maybe a block device or a usb key
            QVariant transfer(params);
            csh.sendToService(transfer);
        } else {
            // We should be able to mount it ourselves
            this->mountVolumeCommon(params);
        }
    }
    catch (GostCryptException& e)
    {
        e.clone(params->id.requestId)->raise();
    }
}

void CoreUser::continueMountVolume(QSharedPointer<MountVolumeRequest> params,
                                   QSharedPointer<MountVolumeResponse> response)
{
    //TODO implementation of this part is next. should be a Global feature in CoreBase like mountVolumeCommon
    this->sendMountVolume(response);
}

void CoreUser::receiveResponse(QVariant& r)
{
    HANDLE_RESPONSE(MountVolume)
    else HANDLE_RESPONSE(DismountVolume)
        else HANDLE_RESPONSE(CreateVolume)
            else HANDLE_RESPONSE(ChangeVolumePassword)
                    else { throw UnknowResponseException(r.typeName()); }
}
}
}
