#include <QQuickItem>
#include <QUrl>
#include "connectSignals.h"

/*! converts byte to MB, GB, KB */
QString formatSize(uint64 sizeInByte) {
    if (sizeInByte < 1024) return QString(QString("<font color=#6e9f45>")
        + QString::number(sizeInByte)
        + QString("</font>")
        + QString(" B"));

    else if (sizeInByte < 1048576) return QString("<font color=#6e9f45>")
        + QString::number((float)sizeInByte / (float)1024, 'f', 1)
        + QString("</font>")
        + QString(" KB");

    else if (sizeInByte < 1073741824) return QString("<font color=#6e9f45>")
        + QString::number((float)sizeInByte / (float)1048576, 'f', 1)
        + QString("</font>")
        + QString(" MB");

    else return QString("<font color=#6e9f45>")
        + QString::number((float)sizeInByte / (float)1073741824, 'f', 1)
        + QString("</font>")
        + QString(" GB");
}

ConnectSignals::ConnectSignals(GraphicUserInterface *aGUI)
    : mGUI(aGUI)
{
    connect(mGUI, SIGNAL(askSudoPassword()), this, SLOT(subWindowAskSudoPassword()));
}

void ConnectSignals::connectReceiveMount(const QString &aPath, const QString &aPwd)
{
#ifdef QT_DEBUG
    qDebug() << "[DEBUG] : Mounting an opened volume...";
#endif
    //test de renvoi de signal c++ vers qml
    //emit mGUI->sendVolume("test");
    emit sendReceiveMount(aPath);
    mGUI->receiveMount(QUrl(aPath).toLocalFile(),aPwd);
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

void ConnectSignals::connectSudo(const QString& aPwd)
{
    mGUI->receiveSudoPassword(aPwd);
}

void ConnectSignals::subWindowAskSudoPassword()
{
    emit sendSubWindowAskSudoPassword();
}
