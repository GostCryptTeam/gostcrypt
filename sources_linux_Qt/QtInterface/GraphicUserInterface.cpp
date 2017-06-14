#include "GraphicUserInterface.h"

#include <iostream>
#include <QDebug>
#include <QCoreApplication>

GraphicUserInterface::GraphicUserInterface(QObject * parent)
    :   QObject(parent),
        mAdminPasswordRequestHandler(new AdminPasswordRequestHandler(this))
{
    this->init();
}

void GraphicUserInterface::init() {
    // Start core service
    GostCrypt::CoreService::Start();
    // Start encryption thread pool
    GostCrypt::EncryptionThreadPool::Start();

    GostCrypt::Core->Init();
    GostCrypt::Core->SetApplicationExecutablePath (QCoreApplication::applicationFilePath().toStdWString());

    GostCrypt::Core->SetAdminPasswordCallback (shared_ptr<GostCrypt::GetStringFunctor>(mAdminPasswordRequestHandler));
}

void GraphicUserInterface::receive(const QString& str)
{
#ifdef QT_DEBUG
    qDebug() << str;
#endif
}


void GraphicUserInterface::receiveMount(const QString& aPath, const QString& aPassword)
{
#ifdef QT_DEBUG
    qDebug() << "Monter : " << aPath << " " << "********";
#endif
    GostCrypt::VolumePath *volumePath = new GostCrypt::VolumePath(aPath.toStdString());
    GostCrypt::VolumePassword *volumePassword = new GostCrypt::VolumePassword(aPassword.toStdWString());
    GostCrypt::MountOptions options;
    try {
        if(GostCrypt::Core->IsVolumeMounted (*volumePath)) {
            qDebug() << "Volume already mounted";
            return;
        }
        options.Password.reset(volumePassword);
        options.Path.reset(volumePath);
        try {
            shared_ptr <GostCrypt::VolumeInfo> volumeData = GostCrypt::Core->MountVolume (options);
            emit sendVolumeInfos((string)volumeData.get()->MountPoint, volumeData.get()->EncryptionAlgorithmName, (string)volumeData.get()->Path, volumeData.get()->Size);
        } catch (GostCrypt::PasswordIncorrect &e) {
            emit mountVolumePasswordIncorrect();
        }
    } catch (GostCrypt::SystemException e) {
        qDebug() << "Exception catch";
    }
}

void GraphicUserInterface::receiveAutoMount(const QString& aPassword)
{
#ifdef QT_DEBUG
    qDebug() << "Monter auto";
#endif
    (void) aPassword;
    // Voir Main/GraphicUserInterface.cpp:617
    // Voir Main/UserInterface.cpp:509
}

void GraphicUserInterface::receiveDismountAll()
{
#ifdef QT_DEBUG
    qDebug() << "Tout démonter";
#endif
    GostCrypt::VolumeInfoList volumes = GostCrypt::Core->GetMountedVolumes();
    for(GostCrypt::SharedPtr<GostCrypt::VolumeInfo> volume : volumes){
        GostCrypt::Core->DismountVolume(volume);
        emit confirmSudoPassword();
    }

}

GostCrypt::VolumeInfoList GraphicUserInterface::receiveGetAllVolumes()
{
    return GostCrypt::Core->GetMountedVolumes();
}

void GraphicUserInterface::receiveSudoPassword(const QString &aPwd)
{
    mAdminPasswordRequestHandler->sendPassword(aPwd);
}

void GraphicUserInterface::receiveDismount(const QString& aStr)
{
#ifdef QT_DEBUG
    qDebug() << "On démonte " << aStr;
#endif
    GostCrypt::VolumePath path = GostCrypt::VolumePath(aStr.toStdString());
    shared_ptr<GostCrypt::VolumeInfo> volume = GostCrypt::Core->GetMountedVolume(path);
    if(volume) GostCrypt::Core->DismountVolume(volume);
    emit confirmSudoPassword();
}

void GraphicUserInterface::receiveChangePassword(const QString &volumePath, const QString &oldPassword, const QString &newPassword, shared_ptr <GostCrypt::KeyfileList> oldKeyFiles, shared_ptr <GostCrypt::KeyfileList> newKeyFiles){
    GostCrypt::Core->ChangePassword(shared_ptr<GostCrypt::VolumePath>(new GostCrypt::VolumePath(volumePath.toStdWString())),
                                    true,
                                    shared_ptr<GostCrypt::VolumePassword>(new GostCrypt::VolumePassword(oldPassword.toStdString().c_str(),oldPassword.size())),
                                    oldKeyFiles,
                                    shared_ptr<GostCrypt::VolumePassword>(new GostCrypt::VolumePassword(newPassword.toStdString().c_str(), newPassword.size())),
                                    newKeyFiles);
}

void GraphicUserInterface::receiveCreateVolume(shared_ptr <GostCrypt::VolumeCreationOptions> aCreate){
#ifdef QT_DEBUG
    qDebug() << "Création de volume";
#endif
    GostCrypt::VolumeCreator *creator = new GostCrypt::VolumeCreator();
    creator->CreateVolume(aCreate);
    struct timespec ts = { 0, 10 * 1000 * 1000 };
    GostCrypt::VolumeCreator::ProgressInfo pi = creator->GetProgressInfo();
    while(pi.CreationInProgress){
        nanosleep(&ts, NULL);
        pi = creator->GetProgressInfo();
    }
    try{
        creator->CheckResult();
    }catch(exception &e) {
        qDebug() << "Erreur lors de la création : " << e.what();
        return;
    }
    // Format non-FAT filesystem
    /*const char *fsFormatter = nullptr;

    switch (aCreate->Filesystem)
    {
    case GostCrypt::VolumeCreationOptions::FilesystemType::Ext2:		fsFormatter = "mkfs.ext2"; break;
    case GostCrypt::VolumeCreationOptions::FilesystemType::Ext3:		fsFormatter = "mkfs.ext3"; break;
    case GostCrypt::VolumeCreationOptions::FilesystemType::Ext4:		fsFormatter = "mkfs.ext4"; break;
    case GostCrypt::VolumeCreationOptions::FilesystemType::MacOsExt:	fsFormatter = "newfs_hfs"; break;
    case GostCrypt::VolumeCreationOptions::FilesystemType::UFS:		fsFormatter = "newfs" ; break;
    default: break;
    }

    if (fsFormatter)
    {

        GostCrypt::MountOptions mountOptions ();
        // initialization
        mountOptions.CachePassword = false;
        mountOptions.FilesystemOptions = nullptr;
        mountOptions.FilesystemType = nullptr;
        mountOptions.Keyfiles = shared_ptr();
        mountOptions.MountPoint = nullptr;
        mountOptions.NoHardwareCrypto = true;
        mountOptions.NoKernelCrypto = true;
        mountOptions.PartitionInSystemEncryptionScope = false;
        mountOptions.Password = shared_ptr();
        mountOptions.PreserveTimestamps = false;
        mountOptions.ProtectionKeyfiles = nullptr;
        mountOptions.ProtectionPassword = nullptr;
        mountOptions.Removable = false;
        mountOptions.SharedAccessAllowed = false;
        mountOptions.SlotNumber = 0;
        mountOptions.UseBackupHeaders = false;
        // real parameters
        mountOptions.Path = make_shared <VolumePath> (aCreate->Path);
        mountOptions.NoFilesystem = true;
        mountOptions.Protection = GostCrypt::VolumeProtection::None;
        mountOptions.Password = aCreate->Password;
        mountOptions.Keyfiles = aCreate->Keyfiles;

        shared_ptr <GostCrypt::VolumeInfo> volume = GostCrypt::Core->MountVolume (mountOptions);
        finally_do_arg (shared_ptr <GostCrypt::VolumeInfo>, volume, { GostCrypt::Core->DismountVolume (finally_arg, true); });

        //Thread::Sleep (2000);	// Try to prevent race conditions caused by OS

        // Temporarily take ownership of the device if the user is not an administrator
        GostCrypt::UserId origDeviceOwner ((uid_t) -1);

        GostCrypt::DevicePath virtualDevice = volume->VirtualDevice;

        try
        {
            File file;
            file.Open (virtualDevice, GostCrypt::File::OpenReadWrite);
        }
        catch (...)
        {
            if (!Core->HasAdminPrivileges())
            {
                origDeviceOwner = virtualDevice.GetOwner();
                Core->SetFileOwner (virtualDevice, UserId (getuid()));
            }
        }

        finally_do_arg2 (FilesystemPath, virtualDevice, UserId, origDeviceOwner,
        {
            if (finally_arg2.SystemId != (uid_t) -1)
                GostCrypt::Core->SetFileOwner (finally_arg, finally_arg2);
        });

        // Create filesystem
        list <string> args;

        args.push_back (string (virtualDevice));

        Process::Execute (fsFormatter, args);
    }*/

}

void GraphicUserInterface::stop() {
    GostCrypt::CoreService::Stop();
    GostCrypt::EncryptionThreadPool::Stop();
}

GraphicUserInterface::~GraphicUserInterface()
{
    this->stop();
}
