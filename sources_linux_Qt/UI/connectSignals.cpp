#include <QQuickItem>
#include <QUrl>
#include <QDesktopServices>
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
    connect(mGUI, SIGNAL(sendError(QString,QString)), this, SLOT(subWindowSendErrorMessage(QString,QString)));
    connect(mGUI, SIGNAL(askSudoPassword()), this, SLOT(subWindowAskSudoPassword()));
    connect(mGUI, SIGNAL(sendVolumeInfos(string,wstring,string,uint64)), this, SLOT(subWindowSendVolumeInfos(string,wstring,string,uint64)));
    connect(mGUI, SIGNAL(confirmSudoPassword()), this, SLOT(subWindowConfirmSudoPassword()));
    connect(mGUI, SIGNAL(mountVolumePasswordIncorrect()), this, SLOT(subWindowMountVolumePasswordIncorrect()));
}

void ConnectSignals::init(QObject* aQml)
{
    connect(aQml, SIGNAL(mountVolume(QString, QString)), mGUI, SLOT(receiveMount(QString, const QString &)));
}

void ConnectSignals::connectReceiveAutoMount(const QString& aPwd)
{
#ifdef QT_DEBUG
    qDebug() << "[DEBUG] : Auto mount devices...";
#endif
    mGUI->receiveAutoMount(aPwd); //TODO change
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
    qDebug() << "[DEBUG] : connecting to sudo";
    mGUI->receiveSudoPassword(aPwd);
}

void ConnectSignals::subWindowSendErrorMessage(QString aTitle, QString aContent)
{
    emit sendSubWindowErrorMessage(aTitle, aContent);
}

void ConnectSignals::connectEndSudo()
{
    qDebug() << "[DEBUG] : end of sudo requested : ";
    mGUI->receiveSudoEndPassword();
}

void ConnectSignals::openPath(const QString &aPath)
{
#ifdef Q_WS_WIN
    if (QFileInfo(path).isDir())
        QProcess::startDetached("explorer", QStringList(path));
    else
#endif
        QDesktopServices::openUrl(QUrl(aPath));
}

void ConnectSignals::getAllMountedVolumes()
{
    /*GostCrypt::VolumeInfoList volumes = mGUI->receiveGetAllVolumes();
    for(GostCrypt::SharedPtr<GostCrypt::VolumeInfo> volume : volumes){
        this->subWindowSendVolumeInfos(volume->MountPoint, volume->EncryptionAlgorithmName, volume->Path, volume->Size);
    }*/
}

void ConnectSignals::changepassword(const QString &volumePath, const QString &oldPassword, const QString &newPassword, shared_ptr<GostCrypt::KeyfileList> oldKeyfiles, shared_ptr<GostCrypt::KeyfileList> newKeyfiles)
{
    mGUI->receiveChangePassword(volumePath, oldPassword, newPassword, oldKeyfiles, newKeyfiles);
}

void ConnectSignals::subWindowAskSudoPassword()
{
    emit sendSubWindowAskSudoPassword();
}

void ConnectSignals::subWindowSendVolumeInfos(string aMountPoint, wstring aAlgo, string aPath, uint64 aSize)
{
    emit sendSubWindowVolumeInfos(QString::fromStdString(aMountPoint.c_str()),
                                  QString::fromWCharArray(aAlgo.c_str()),
                                  QString::fromStdString(aPath.c_str()),
                                  formatSize(aSize));
}

void ConnectSignals::subWindowConfirmSudoPassword()
{
    emit sendSubWindowConfirmSudoPassword();
}

void ConnectSignals::subWindowMountVolumePasswordIncorrect()
{
    emit sendSubWindowMountVolumePasswordIncorrect();
}
