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


/*void ConnectSignals::debug_connectReceiveCreate()
{
    return;
    qDebug() << "[DEBUG] : Creating debug device...";

    shared_ptr <GostCrypt::VolumeCreationOptions> volume
            = shared_ptr <GostCrypt::VolumeCreationOptions>(new GostCrypt::VolumeCreationOptions());

    GostCrypt::RandomNumberGenerator::Start();

    GostCrypt::HashList Hashes = GostCrypt::Hash::GetAvailableAlgorithms();
    for (GostCrypt::HashList::iterator h = Hashes.begin(); h != Hashes.end(); h++)
    {
        if (!(*h)->IsDeprecated()){
            volume->VolumeHeaderKdf = GostCrypt::Pkcs5Kdf::GetAlgorithm (*(*h).get());
            GostCrypt::RandomNumberGenerator::SetHash (*h);
            break;
        }
    }

    volume->Path = wstring(L"/home/user/newVolume");
    volume->Type = GostCrypt::VolumeType::Normal;
    volume->Size = 50*1024*1024;
    volume->Password = shared_ptr <GostCrypt::VolumePassword>(new GostCrypt::VolumePassword("banana",6));
    volume->Keyfiles = shared_ptr <GostCrypt::KeyfileList>(nullptr);

    volume->Filesystem = GostCrypt::VolumeCreationOptions::FilesystemType::Ext4;
    volume->FilesystemClusterSize = 4096;

    GostCrypt::EncryptionAlgorithmList eas = GostCrypt::EncryptionAlgorithm::GetAvailableAlgorithms();
    for (GostCrypt::EncryptionAlgorithmList::iterator ea = eas.begin(); ea != eas.end(); ea++)
    {
        if (!(*ea)->IsDeprecated()){
            volume->EA = ((GostCrypt::EncryptionAlgorithm *)ea->get())->GetNew();
            break;
        }
    }
    volume->SectorSize = 512;
    volume->Quick = false;
    //volume->SectorSize = GostCrypt::Core->GetDeviceSectorSize (volume->Path);

    mGUI->receiveCreateVolume(volume);
}*/

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

void iterateHostDeviceList(GostCrypt::HostDeviceList volumes, QVariantList *listOfDevices)
{
    if(volumes.size() <= 0)
        return;
    for (std::list<shared_ptr <GostCrypt::HostDevice>>::iterator it = volumes.begin();
         it != volumes.end();
         it++)
    {
#ifdef QT_DEBUG
        qDebug() << "======\nMountPoint : " << QString::fromStdString(GostCrypt::StringConverter::ToSingle(it->get()->MountPoint));
        qDebug() << "Name : " << QString::fromWCharArray(it->get()->Name.c_str());
        qDebug() << "Path : " << QString::fromStdString(GostCrypt::StringConverter::ToSingle(it->get()->Path));
        qDebug() << "Removable : " << it->get()->Removable;
        qDebug() << "Size : " << it->get()->Size;
        qDebug() << "SystemNumber : " << it->get()->SystemNumber;
#endif
        *listOfDevices << QVariant::fromValue(
                             QVariantList{
                                 QString::fromStdString(GostCrypt::StringConverter::ToSingle(it->get()->MountPoint)),
                                 QString::fromWCharArray(it->get()->Name.c_str()),
                                 QString::fromStdString(GostCrypt::StringConverter::ToSingle(it->get()->Path)),
                                 (bool)it->get()->Removable,
                                 (int)it->get()->Size,
                                 (int)it->get()->SystemNumber
                             });
        iterateHostDeviceList(it->get()->Partitions, listOfDevices);
    }
}

QVariantList ConnectSignals::getListOfDevices()
{
    GostCrypt::HostDeviceList volumes = GostCrypt::Core->GetHostDevices(false);
    QVariantList listOfDevices{};
    iterateHostDeviceList(volumes, &listOfDevices);
    return listOfDevices;
}

bool ConnectSignals::isMounted(const QString &path)
{
    GostCrypt::VolumeInfoList volumes = mGUI->receiveGetAllVolumes();
    for(GostCrypt::SharedPtr<GostCrypt::VolumeInfo> volume : volumes)
        if(path == QString::fromStdString((string)volume->Path)) return true;
    return false;
}

QString ConnectSignals::getMountPoint(const QString &path) const
{
    GostCrypt::VolumeInfoList volumes = mGUI->receiveGetAllVolumes();
    for(GostCrypt::SharedPtr<GostCrypt::VolumeInfo> volume : volumes)
        if(path == QString::fromStdString((string)volume->Path)) return QString::fromStdString((string)volume->MountPoint);
    return "";
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
    GostCrypt::VolumeInfoList volumes = mGUI->receiveGetAllVolumes();
    for(GostCrypt::SharedPtr<GostCrypt::VolumeInfo> volume : volumes){
        this->subWindowSendVolumeInfos(volume->MountPoint, volume->EncryptionAlgorithmName, volume->Path, volume->Size);
    }
}

void ConnectSignals::changepassword(const QString &volumePath, const QString &oldPassword, const QString &newPassword, GostCrypt::SharedPtr<GostCrypt::KeyfileList> oldKeyfiles, GostCrypt::SharedPtr<GostCrypt::KeyfileList> newKeyfiles)
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
