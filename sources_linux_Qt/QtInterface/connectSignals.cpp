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
    connect(mGUI, SIGNAL(sendVolumeInfos(string,wstring,string,uint64)), this, SLOT(subWindowSendVolumeInfos(string,wstring,string,uint64)));
}

void ConnectSignals::connectReceiveMount(const QString &aPath, const QString &aPwd)
{
#ifdef QT_DEBUG
    qDebug() << "[DEBUG] : Mounting an opened volume...";
#endif
    //test de renvoi de signal c++ vers qml
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


void ConnectSignals::debug_connectReceiveCreate()
{

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
    volume->FilesystemClusterSize = 16184;

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
}

void ConnectSignals::connectSudo(const QString& aPwd)
{
    qDebug() << "[DEBUG] : connecting to sudo : " << aPwd;
    mGUI->receiveSudoPassword(aPwd);
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
