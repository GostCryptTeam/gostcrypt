#include <QQuickItem>
#include <QUrl>
#include "connectSignals.h"

ConnectSignals::ConnectSignals(GraphicUserInterface *aGUI)
    : mGUI(aGUI)
{
}

void ConnectSignals::connectReceiveMount(const QString &aPath, const QString &aPwd)
{
#ifdef QT_DEBUG
    qDebug() << "[DEBUG] : Mounting an opened volume...";
#endif
    //test de renvoi de signal c++ vers qml
    //emit mGUI->sendVolume("test");
    emit sendReceiveMount(aPath);
    //à décommenter : terminate called after throwing an instance of 'GostCrypt::ExecutedProcessFailed'
    //what():  GostCrypt::Process::Execute:193
    //mGUI->receiveMount(QUrl(aPath).toLocalFile(),aPwd);
}

void ConnectSignals::connectReceiveAutoMount()
{
#ifdef QT_DEBUG
    qDebug() << "[DEBUG] : Auto mount devices...";
#endif
    mGUI->receiveAutoMount();
}

void ConnectSignals::connectReceiveDismount(const QString &aString)
{
#ifdef QT_DEBUG
    qDebug() << "[DEBUG] : Dismount a device...";
#endif
    mGUI->receiveDismount(aString);
}

void ConnectSignals::connectReceiveDismountAll()
{
#ifdef QT_DEBUG
    qDebug() << "[DEBUG] : Dismount all devices...";
#endif
    mGUI->receiveDismountAll();
}
