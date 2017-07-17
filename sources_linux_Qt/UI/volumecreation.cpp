#include "volumecreation.h"


VolumeCreation::VolumeCreation()
{
    GostCrypt::EncryptionAlgorithmList eas = GostCrypt::EncryptionAlgorithm::GetAvailableAlgorithms();
    for (GostCrypt::EncryptionAlgorithmList::iterator ea = eas.begin(); ea != eas.end(); ea++)
    {
        if (!(*ea)->IsDeprecated()){ // we don't allow deprecated algorithms when creating a new volume
            algorithms.push_back(*ea);
        }
    }
    creator = new GostCrypt::VolumeCreator();
    memset(&pi, 0, sizeof(GostCrypt::VolumeCreator::ProgressInfo));

    GostCrypt::RandomNumberGenerator::Start();

    infos = shared_ptr<GostCrypt::VolumeCreationOptions>(new GostCrypt::VolumeCreationOptions());
    infos->Quick = false;
    infos->Filesystem = GostCrypt::VolumeCreationOptions::FilesystemType::Ext4;
    infos->FilesystemClusterSize =  4096;
    infos->SectorSize =  512;
}

VolumeCreation::~VolumeCreation()
{
    delete creator;
}

void VolumeCreation::createVolume()
{
    #ifdef QT_DEBUG
        qDebug() << "Creating volume";
    #endif
    // création du volume
    creator->CreateVolume(infos);
    struct timespec ts = { 0, 10 * 1000 * 1000 };
    pi = creator->GetProgressInfo();
    while(pi.CreationInProgress){
        // attente + maj des infos
        nanosleep(&ts, NULL);
        pi = creator->GetProgressInfo();
    }
    try{
        creator->CheckResult();
    }catch(exception &e) {
        // TODO : message pour utilisateur?
        #ifdef QT_DEBUG
            qDebug() << "Error during creation : " << e.what();
        #endif
        return;
    }
    // Format non-FAT filesystem
    const char *fsFormatter = nullptr;

    switch (infos->Filesystem)
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
        mountOptions.Path = make_shared <GostCrypt::VolumePath> (infos->Path);
        mountOptions.NoFilesystem = true;
        mountOptions.Protection = GostCrypt::VolumeProtection::None;
        mountOptions.Password = infos->Password;
        mountOptions.Keyfiles = infos->Keyfiles;

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


    //mVCreator.CreateVolume(GostCrypt::SharedPtr<GostCrypt::VolumeCreationOptions>(infos));
}

void VolumeCreation::abort()
{
// TODO : abort everything
}

QStringList VolumeCreation::getAlgos() const
{
    QStringList algos;
    for (auto ea = algorithms.begin(); ea != algorithms.end(); ea++)
    {
        QString qtString = QString::fromWCharArray(((GostCrypt::EncryptionAlgorithm *)ea->get())->GetName().c_str());
        algos.append(qtString);
    }
    //std::vector<GostCrypt::SharedPtr<GostCrypt::EncryptionAlgorithm> >::const_iterator {aka __gnu_cxx::__normal_iterator<const GostCrypt::SharedPtr<GostCrypt::EncryptionAlgorithm>*, std::vector<GostCrypt::SharedPtr<GostCrypt::EncryptionAlgorithm> > >}
    //std::vector<GostCrypt::SharedPtr<GostCrypt::EncryptionAlgorithm> >::      iterator {aka __gnu_cxx::__normal_iterator<      GostCrypt::SharedPtr<GostCrypt::EncryptionAlgorithm>*, std::vector<GostCrypt::SharedPtr<GostCrypt::EncryptionAlgorithm> > >}
    return algos;
}

QStringList VolumeCreation::getHashs() const
{
    QStringList algos;
    GostCrypt::HashList hl = GostCrypt::Hash::GetAvailableAlgorithms();
    for (GostCrypt::HashList::iterator h = hl.begin(); h != hl.end(); h++)
    {
        if (!(*h)->IsDeprecated()){ // we don't allow deprecated algorithms when creating a new volume
            QString qtString = QString::fromWCharArray(((GostCrypt::Hash *)h->get())->GetName().c_str());
            algos.append(qtString);
        }
    }
    return algos;
}

QString VolumeCreation::getAlgosDescription(const int &aIndex)
{
    if(aIndex >= 0 && aIndex < algorithms.size()){
        QString qtString = QString::fromWCharArray(algorithms[aIndex]->GetDescription().c_str());
        return qtString;
    }else{
        return "Couldn't retreive description.";
    }
}

QString VolumeCreation::getfreeSpace()
{
    return "19.8 GB"; //mVCreator. TODO
}

void VolumeCreation::setPath(const QString &value)
{
    infos->Path = GostCrypt::VolumePath(value.toStdWString());
}

void VolumeCreation::setType(const int &value)
{
    if(value)
        infos->Type = GostCrypt::VolumeType::Hidden;
    else
        infos->Type = GostCrypt::VolumeType::Normal;
}

void VolumeCreation::setSize(const uint64 &value)
{
    infos->Size = value; //uint64
}

void VolumeCreation::setPassword(const QString &value)
{
    infos->Password =  shared_ptr<GostCrypt::VolumePassword>(new GostCrypt::VolumePassword(value.toStdWString()));
}

void VolumeCreation::setKeyFiles(const QString &value)
{
    qDebug() << "Unsupported option ! Program may crash later...";
}

void VolumeCreation::setVolumeHeaderKdf(const QString &value)
{
    shared_ptr<GostCrypt::Hash> hash = getHash(value); // TODO should check if nullptr
    infos->VolumeHeaderKdf =  GostCrypt::Pkcs5Kdf::GetAlgorithm ((*hash.get()));
    GostCrypt::RandomNumberGenerator::SetHash (hash);
}

void VolumeCreation::setEA(const QString &value)
{
    shared_ptr<GostCrypt::EncryptionAlgorithm> ea = getAlgo(value); // TODO should check if nullptr
    infos->EA = ea->GetNew();
}

void VolumeCreation::setQuick(const bool value)
{
    infos->Quick = value;
}

void VolumeCreation::setFileSystem(const int &value)
{
    infos->Filesystem = GostCrypt::VolumeCreationOptions::FilesystemType::Ext4;
}

void VolumeCreation::setFileSystemClusterSize(const uint32 &value) // 4096
{
    infos->FilesystemClusterSize = value; // TODO : check if other values are possible
}

void VolumeCreation::setSectorSize(const uint32 &value) // should always be 512 !
{
    infos->SectorSize = value; //uint32 // TODO : isn't that value computed automatically ?
}

float VolumeCreation::getPercent()
{
    return 0.0f;
}

int VolumeCreation::getTimeRemaining()
{
    return 0;
}

int VolumeCreation::getSpeed()
{
    return 0;
}

shared_ptr<GostCrypt::EncryptionAlgorithm> VolumeCreation::getAlgo(QString name)
{
    for (auto ea = algorithms.begin(); ea != algorithms.end(); ea++)
    {
        QString qtString = QString::fromWCharArray(((GostCrypt::EncryptionAlgorithm *)ea->get())->GetName().c_str());
        if(qtString.compare(name) == 0)
            return ((GostCrypt::EncryptionAlgorithm *)ea->get())->GetNew();
    }
    return shared_ptr<GostCrypt::EncryptionAlgorithm>();
}

shared_ptr<GostCrypt::Hash> VolumeCreation::getHash(QString name)
{
    GostCrypt::HashList hl = GostCrypt::Hash::GetAvailableAlgorithms();
    for (GostCrypt::HashList::iterator h = hl.begin(); h != hl.end(); h++)
    {
        if (!(*h)->IsDeprecated()){ // we don't allow deprecated algorithms when creating a new volume
            QString qtString = QString::fromWCharArray(((GostCrypt::Hash *)h->get())->GetName().c_str());
            if(qtString.compare(name) == 0)
                return ((GostCrypt::Hash *)h->get())->GetNew();
        }
    }
    return shared_ptr<GostCrypt::Hash>();
}

