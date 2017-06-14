#include "GraphicUserInterface.h"

#include <iostream>
#include <QDebug>
#include <QCoreApplication>

GraphicUserInterface::GraphicUserInterface(QObject * parent)
    :   QObject(parent),
        mAdminPasswordRequestHandler(this)
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

    GostCrypt::Core->SetAdminPasswordCallback (shared_ptr <GostCrypt::GetStringFunctor> (&mAdminPasswordRequestHandler));
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
        shared_ptr <GostCrypt::VolumeInfo> volumeData = GostCrypt::Core->MountVolume (options);
        emit sendVolumeInfos((string)volumeData.get()->AuxMountPoint, volumeData.get()->EncryptionAlgorithmName, (string)volumeData.get()->Path, volumeData.get()->Size);
    } catch (GostCrypt::SystemException e) {
        qDebug() << "Exception catch";
    }
}

void GraphicUserInterface::receiveAutoMount()
{
#ifdef QT_DEBUG
    qDebug() << "Monter auto";
#endif
    // Voir Main/Forms/MainFrame.cpp:530
}

void GraphicUserInterface::receiveDismountAll()
{
#ifdef QT_DEBUG
    qDebug() << "Tout démonter";
#endif
    GostCrypt::VolumeInfoList volumes = GostCrypt::Core->GetMountedVolumes();
    for(GostCrypt::SharedPtr<GostCrypt::VolumeInfo> volume : volumes){
        GostCrypt::Core->DismountVolume(volume);
    }
}

void GraphicUserInterface::receiveSudoPassword(const QString &aPwd)
{
    mAdminPasswordRequestHandler.sendPassword(aPwd);
}

void GraphicUserInterface::receiveDismount(const QString& aStr)
{
#ifdef QT_DEBUG
    qDebug() << "On démonte " << aStr;
#endif
    GostCrypt::VolumePath path = GostCrypt::VolumePath(aStr.toStdString());
    GostCrypt::SharedPtr<GostCrypt::VolumeInfo> volume = GostCrypt::Core->GetMountedVolume(path);
    if(volume) GostCrypt::Core->DismountVolume(volume);
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
    const char *fsFormatter = nullptr;

    /*switch (aCreate->Filesystem)
    {
    case VolumeCreationOptions::FilesystemType::Ext2:		fsFormatter = "mkfs.ext2"; break;
    case VolumeCreationOptions::FilesystemType::Ext3:		fsFormatter = "mkfs.ext3"; break;
    case VolumeCreationOptions::FilesystemType::Ext4:		fsFormatter = "mkfs.ext4"; break;
    case VolumeCreationOptions::FilesystemType::MacOsExt:	fsFormatter = "newfs_hfs"; break;
    case VolumeCreationOptions::FilesystemType::UFS:		fsFormatter = "newfs" ; break;
    default: break;
    }

    if (fsFormatter)
    {
        wxBusyCursor busy;

        MountOptions mountOptions (Gui->GetPreferences().DefaultMountOptions);
        mountOptions.Path = make_shared <VolumePath> (SelectedVolumePath);
        mountOptions.NoFilesystem = true;
        mountOptions.Protection = VolumeProtection::None;
        mountOptions.Password = Password;
        mountOptions.Keyfiles = Keyfiles;

        shared_ptr <VolumeInfo> volume = Core->MountVolume (mountOptions);
        finally_do_arg (shared_ptr <VolumeInfo>, volume, { Core->DismountVolume (finally_arg, true); });

        Thread::Sleep (2000);	// Try to prevent race conditions caused by OS

        // Temporarily take ownership of the device if the user is not an administrator
        UserId origDeviceOwner ((uid_t) -1);

        DevicePath virtualDevice = volume->VirtualDevice;

        try
        {
            File file;
            file.Open (virtualDevice, File::OpenReadWrite);
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
                Core->SetFileOwner (finally_arg, finally_arg2);
        });

        // Create filesystem
        list <string> args;

        if (SelectedFilesystemType == VolumeCreationOptions::FilesystemType::MacOsExt && VolumeSize >= 10 * BYTES_PER_MB)
            args.push_back ("-J");

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
