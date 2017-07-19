#include "GraphicUserInterface.h"

#include <iostream>
#include <QDebug>
#include <QUrl>
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


void GraphicUserInterface::receiveMount(QString aPath, const QString& aPassword)
{
    // aPth n'est pas mis à jour lorsque l'on utilise l'historique ou le drag'n'drop
    aPath = QUrl(aPath).toLocalFile();
    //aPath = "/home/hantoine/volume";
#ifdef QT_DEBUG
    qDebug() << "Monter : " << aPath << " " << aPassword;
#endif
    GostCrypt::MountOptions options;
    options.Password.reset(new GostCrypt::VolumePassword(aPassword.toStdWString()));
    options.Path.reset(new GostCrypt::VolumePath(aPath.toStdString()));
    //options.MountPoint.reset(new DirectoryPath("/home/hantoine/as"));
    try {

        shared_ptr <GostCrypt::VolumeInfo> volumeData = GostCrypt::Core->MountVolume (options);
#ifdef QT_DEBUG
    qDebug() << "VOLUME MONTÉ !! : " << aPath << " '" << aPassword << "'";
#endif
        emit sendVolumeInfos((string)volumeData.get()->MountPoint, volumeData.get()->EncryptionAlgorithmName, (string)volumeData.get()->Path, volumeData.get()->Size);
    } catch (GostCrypt::PasswordIncorrect &e) {
        emit mountVolumePasswordIncorrect();
    } catch (GostCrypt::VolumeAlreadyMounted &e) {
        qDebug() << "Volume already mounted";
        return;
    } catch (GostCrypt::SystemException &e) {
        switch(e.GetErrorCode())
        {
        //not a volume file
        case 22:
            qDebug() << "Bad file";
            emit sendError("Bad file", "Not a GostCrypt volume.");
            emit mountVolumePasswordIncorrect();
            break;
        }
        emit sendError("Exception catch", "An unexpected error occured. \n"
#ifdef QT_DEBUG
        +QString::fromUtf8(e.what())
#endif
        );
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
    bool check = false;
    try {
        for(GostCrypt::SharedPtr<GostCrypt::VolumeInfo> volume : volumes){
            GostCrypt::Core->DismountVolume(volume);
            if(!check) {
                emit confirmSudoPassword();
                check = true;
            }
        }
    }catch(...) {
        emit sendError("Exception catch", "An unexpected error occured. \n"
#ifdef QT_DEBUG
        +QString::fromUtf8(e.what())
#endif
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

void GraphicUserInterface::receiveSudoEndPassword()
{
    mAdminPasswordRequestHandler->quitExec();
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
    QString cmd = "createGSVolume";

    system(cmd.toStdString().c_str());
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

        GostCrypt::MountOptions mountOptions;
        // initialization
        mountOptions.CachePassword = false;
        mountOptions.Keyfiles = shared_ptr<GostCrypt::KeyfileList>();
        mountOptions.MountPoint = shared_ptr<GostCrypt::FilesystemPath>();
        mountOptions.NoHardwareCrypto = true;
        mountOptions.NoKernelCrypto = true;
        mountOptions.PartitionInSystemEncryptionScope = false;
        mountOptions.Password = shared_ptr<GostCrypt::VolumePassword>();
        mountOptions.PreserveTimestamps = false;
        mountOptions.ProtectionKeyfiles = shared_ptr<GostCrypt::KeyfileList>();
        mountOptions.ProtectionPassword = shared_ptr<GostCrypt::VolumePassword>();
        mountOptions.Removable = false;
        mountOptions.SharedAccessAllowed = false;
        mountOptions.SlotNumber = 0;
        mountOptions.UseBackupHeaders = false;
        // real parameters
        mountOptions.Path = make_shared <GostCrypt::VolumePath> (aCreate->Path);
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
            GostCrypt::File file;
            file.Open (virtualDevice, GostCrypt::File::OpenReadWrite);
        }
        catch (...)
        {
            if (!GostCrypt::Core->HasAdminPrivileges())
            {
                origDeviceOwner = virtualDevice.GetOwner();
                GostCrypt::Core->SetFileOwner (virtualDevice, GostCrypt::UserId (getuid()));
            }
        }

        finally_do_arg2 (GostCrypt::FilesystemPath, virtualDevice, GostCrypt::UserId, origDeviceOwner,
        {
            if (finally_arg2.SystemId != (uid_t) -1)
                GostCrypt::Core->SetFileOwner (finally_arg, finally_arg2);
        });

        // Create filesystem
        list <string> args;

        args.push_back (string (virtualDevice));

        GostCrypt::Process::Execute (fsFormatter, args);
    }

}

void GraphicUserInterface::stop() {
    GostCrypt::CoreService::Stop();
    GostCrypt::EncryptionThreadPool::Stop();
}

GraphicUserInterface::~GraphicUserInterface()
{
    this->stop();
}
