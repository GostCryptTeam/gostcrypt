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
#ifdef QT_DEBUG
    qDebug() << "[DEBUG] : Creating debug device...";
#endif
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
    volume->Keyfiles = shared_ptr <GostCrypt::KeyfileList>(new GostCrypt::KeyfileList());

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
