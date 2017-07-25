#include "parser.h"

void Parser::parseMount(QCoreApplication &app, QCommandLineParser &parser, GostCrypt::NewCore::MountVolumeParams *options)
{
    parser.addPositionalArgument("mount", "Mounts a volume.", "mount");
    parser.addPositionalArgument("volumepath", "Path of the volume or the device to mount.", "path");
    parser.addOptions({
                          {{"p","password"}, "Specify an inline password.", "password"},
                          {{"f", "file", "keyfile"}, "Adds a keyfile.", "file"},
                          {{"c","cache-password"}, "Enable password caching."},
                          {{"o","options"}, "Additional options to pass to the mount function.", "'-option myoption'"},
                          // FilesystemType ?
                          // nofilesystem
                          // PartitionInSystemEncryptionScope
                          {{"h","enable-hardware-crypto"}, "Enables the hardware cryptography."},
                          {{"k","enable-kernel-crypto"}, "Enables the kernel cryptography."},
                          {{"t","not-preserve-timestamps"}, "Doesn't preserves the timestamps."},
                          {{"m","mountpoint"}, "Specify a special mountpoint.", "mountpoint"},
                          //{"v", "Specify a special volume protection", "{None|ReadOnly|HiddenVolumeReadOnly}"},
                          // protectionPassword
                          // protectionKeyfiles
                          {{"r","removable"},"Makes the drive removable."},
                          {{"s","shared"}, "Allows shared access."},
                          {{"b","backup-headers"}, "Use backup headers."},
                          {{"n","slot-number"}, "Use a specific slot number.", "slot"}
                      });
    parser.process(app);

    // Parsing all options

    /*if (parser.isSet("help"))
        throw new Parser::ParseException(); // throwing an empty exception shows the help only

    if (parser.isSet("password")) {
        const QString password = parser.value("password");
        options->Password = shared_ptr<GostCrypt::VolumePassword>(new GostCrypt::VolumePassword(password.toStdWString()));
    } else {
        string password;
        cout << "Please enter the volume password (leave empty if none) : " << std::endl;
        getline(cin, password);
        if(password != "")
            options->Password.reset(new GostCrypt::VolumePassword(password.c_str(), password.size()));
    }

    if (parser.isSet("file")) {
        const QStringList files = parser.values("file");
        GostCrypt::KeyfileList* keyfiles = new list<shared_ptr<GostCrypt::Keyfile>>();
        for(QString file : files){
            keyfiles->push_back(shared_ptr<GostCrypt::Keyfile>(new GostCrypt::Keyfile(file.toStdString())));
        }
        options->Keyfiles.reset(keyfiles);
    }

    if (parser.isSet("cache-password"))
        options->CachePassword = true;

    if (parser.isSet("options")) {
        const QStringList opts = parser.values("options");
        for(QString option : opts){
            options->FilesystemOptions.append((option + " ").toStdWString());
        }
    }*/

    /*if (parser.isSet("")) { // filesystemtype
        // force filesystem thype
    }*/

    /*if (parser.isSet("")) { // nofilesystem
        // set to true if no filesystem on drive
    }*/

    /*if (parser.isSet("")) { // PartitionInSystemEncryptionScope

    }*/

    /*if (parser.isSet("h")) // enable-hardware-crypto
        options->NoHardwareCrypto = false;

    if (parser.isSet("enable-kernel-crypto"))
        options->NoKernelCrypto = false;

    if (parser.isSet("not-preserve-timestamps"))
        options->PreserveTimestamps = false;

    if (parser.isSet("mountpoint")) {
        const QString mountpoint = parser.value("mountpoint");
        options->MountPoint.reset(new GostCrypt::DirectoryPath(qPrintable(mountpoint)));
    }*/

    /*if (parser.isSet("")) { // volume protection

    }*/

    /*if (parser.isSet("")) { // protectionPassword

    }*/

    /*if (parser.isSet("")) { // protectionKeyfiles

    }*/

    /*if (parser.isSet("removable"))
        options->Removable = true;

    if (parser.isSet("shared"))
        options->SharedAccessAllowed = true;

    if (parser.isSet("backup-headers"))
        options->UseBackupHeaders = true;

    if (parser.isSet("slot-number")) {
        const QString number = parser.value("number");
        bool ok = false;
        options->SlotNumber = number.toInt(&ok);
        if (!ok) throw new Parser::ParseException("'slot-number' must be a number !");
    }

    // parsing positional arguments

    const QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.size() < 2)
        throw new Parser::ParseException("Argument 'volumepath' missed.");
    if (positionalArguments.size() > 2)
        throw new Parser::ParseException("Too many arguments specified.");

    options->Path.reset(new GostCrypt::VolumePath(qPrintable(positionalArguments.at(1))));*/
}

void Parser::parseDismount(QCoreApplication &app, QCommandLineParser &parser, GostCrypt::NewCore::DismountVolumeParams *volume)
{
    parser.addPositionalArgument("umount", "Mounts a volume.", "{umount|unmount|dismount}");
    parser.addPositionalArgument("volume", "Path of the volume or the device to unmount");
    parser.process(app);

    // Parsing all options

    if (parser.isSet("help"))
        throw new Parser::ParseException();

    // parsing positional arguments

    const QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.size() < 2)
        throw new Parser::ParseException("Argument 'volume' missed.");
    if (positionalArguments.size() > 2)
        throw new Parser::ParseException("Too many arguments specified.");

    //*volume = positionalArguments.at(1);
}

void Parser::parseList(QCoreApplication &app, QCommandLineParser &parser, Parser::WhatToList *item)
{
    parser.addPositionalArgument("list", "Mounts a volume.", "list");
    parser.addPositionalArgument("item", "Item to list", "{volumes|algorithms|hashs|filesystems}");
    parser.process(app);

    // Parsing all options

    if (parser.isSet("help"))
        throw new Parser::ParseException();

    // parsing positional arguments

    const QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.size() < 2){
        *item = Volumes;
        return;
    }
    if (positionalArguments.size() > 2)
        throw new Parser::ParseException("Too many arguments specified.");
    QString volume = positionalArguments.at(1);
    if(volume == "volumes")
        *item = Volumes;
    else if (volume == "algorithms")
        *item = Algorithms;
    else if (volume == "hashs")
        *item = Hashs;
    if (volume == "filesystems")
        *item = FileSystems;
    else
        throw new Parser::ParseException("Unknown item to list.");
}

void Parser::parseCreate(QCoreApplication &app, QCommandLineParser &parser, GostCrypt::NewCore::CreateVolumeParams *options)
{
    parser.addPositionalArgument("create", "Creates a volume.", "create");
    parser.addPositionalArgument("volumepath", "Path of the volume to create", "path");
    parser.addOptions({
                          {{"p","password"}, "Specify an inline password.", "password"},
                          {"hidden-password", "Specify an inline hidden password.", "hiddenpassword"},
                          {{"f", "file", "keyfile"}, "Adds a keyfile.", "file"},
                          {{"hfile", "hkeyfile"}, "Adds a keyfile for the hidden volume.", "file"},
                          {{"h", "hash"}, "Chooses the hash function. Type 'gostcrypt list hashs' to see the possibilities.", "hashfunction"},
                          {"hhash", "Chooses the hash function for the hidden volume. Type 'gostcrypt list hashs' to see the possibilities.", "hashfunction"},
                          {{"a", "algorithm"}, "Chooses the encryption algorithm. Type 'gostcrypt list algorithms' to see the possibilities.", "algorithm"},
                          {"halgorithm", "Chooses the encryption algorithm for the hidden volume. Type 'gostcrypt list algorithms' to see the possibilities.", "algorithm"},
                          {{"q","Quick"}, "Enable quick formatting."},
                          {"file-system", "Specify a filesystem. Type 'gostcrypt list filesystems' to see the possibilities."},
                          {"cluster-size", "Specify a cluster size different from the default one.", "sizeinbytes"},
                          {"sector-size", "Specify a sector size different from the default one.", "sizeinbytes"},
                          {{"s", "size", "volume-size"}, "Sets the wanted size of the created volume.", "size{B|KB|MB|GB}"},
                          {{"t", "type"}, "Sets the volume type.", "{Normal|Hidden}"}
                      });
    parser.process(app);

    // Parsing all options
    /*options->Keyfiles.reset(nullptr);
    options->Password.reset(nullptr);

    if (parser.isSet("help"))
        throw new Parser::ParseException();

    if (parser.isSet("password")) {
        const QString password = parser.value("password");
        options->setPassword(password);
    }else{
        string password;
        cout << "Please enter the volume password (leave empty if none) : " << std::endl;
        getline(cin, password);
        if(password != "")
            options->setPassword(QString::fromStdString(password));
    }

    if (parser.isSet("file")) {
        options->setKeyFiles("ho ho"); // TODO DO SOMETHING
        throw new Parser::ParseException("WELL WE HAVE A PROBLEM HERE");
    }

    if (parser.isSet("hash")) { // TODO : use some magic function to get it faster
        const QString myhash = parser.value("hash");
        GostCrypt::Pkcs5KdfList hashList = GostCrypt::Pkcs5Kdf::GetAvailableAlgorithms();
        for(GostCrypt::SharedPtr<GostCrypt::Pkcs5Kdf> hash : hashList){
            if(QString::fromWCharArray(hash->GetName().c_str()).toStdString() == myhash){
                // TODO : should we allow deprecated values ?
                options->VolumeHeaderKdf.reset(hash);
                options->setVolumeHeaderKdf(myhash);
                break;
            }
        }
        if(options->VolumeHeaderKdf.get() == nullptr)
            throw new Parser::ParseException(myhash + " not found !");
    }

    if (parser.isSet("algorithm")) { // TODO : use some magic function to get it faster
        const QString algo = parser.value("algorithm");
        GostCrypt::EncryptionAlgorithmList encryptionAlgos = GostCrypt::EncryptionAlgorithm::GetAvailableAlgorithms();
        for(GostCrypt::SharedPtr<GostCrypt::EncryptionAlgorithm> algorithm : encryptionAlgos){
            if(QString::fromWCharArray(algorithm->GetName().c_str()).toStdString() == algo){
                // TODO : should we allow deprecated values ?
                options->EA.reset(algorithm);
                break;
            }
        }
        if(options->EA.get() == nullptr)
            throw new Parser::ParseException("algorithm " + algo + " not found !");
    }

    if (parser.isSet("Quick"))
        options->Quick = true;
    else
        options->Quick = false;

    if (parser.isSet("file-system")) {
        uint32 i;
        const QString fs = parser.value("file-system");
        const uint32 nbfilesystems = 8;
        const QString* filesystems = { "None", "FAT", "NTFS", "Ext2", "Ext3", "Ext4", "MacOsExt", "UFS" };
        for(i=0; i < nbfilesystems; i++){
            if(filesystems[i] == fs){
                options->Filesystem = GostCrypt::VolumeCreationOptions::FilesystemType(i+1);
                break;
            }
        }
        if(i>=nbfilesystems)
            throw new Parser::ParseException("Filesystem " + fs + " not found !");
    } else {
        options->Filesystem = GostCrypt::VolumeCreationOptions::FilesystemType::GetPlatformNative();
    }

    if (parser.isSet("cluster-size")) {
        const QString number = parser.value("cluster-size");
        bool ok = false;
        options->FilesystemClusterSize = number.toInt(&ok);
        if (!ok)
            throw new Parser::ParseException("'cluster-size' must be a number !");
    }

    if (parser.isSet("sector-size")) {
        const QString number = parser.value("sector-size");
        bool ok = false;
        options->SectorSize = number.toInt(&ok);
        if (!ok)
            throw new Parser::ParseException("'sector-size' must be a number !");
    }

    if (parser.isSet("size")) {
        const QString number = parser.value("size");
        bool ok = false;
        options->Size = parseSize(number, &ok);
        if (!ok)
            throw new Parser::ParseException("'size' must be a number followed by B,KB,MB or GB !");
    }

    if (parser.isSet("type")) {
        const QString type = parser.value("type");
        if(type == "Normal" || type == "normal"){
            options->Type = GostCrypt::VolumeType::Normal;
        } else if(type == "Hidden" || type == "hidden") {
            options->Type = GostCrypt::VolumeType::Hidden;
        } else
            throw new Parser::ParseException("'type' must be one of {Normal|Hidden} !");
    }

    // parsing positional arguments

    const QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.size() < 2)
        throw new Parser::ParseException("Argument 'volumepath' missed.");
    if (positionalArguments.size() > 2)
        throw new Parser::ParseException("Too many arguments specified.");

    options->Path.reset(new GostCrypt::VolumePath(qPrintable(positionalArguments.at(1))));*/
}

quint64 parseSize(QString s, bool *ok){
    s.data()[s.size()-1]='\0';
    if(ok)
        *ok = true;
    if(s.data()[s.size()-2].isNumber())
        return s.toInt();
    if(s.data()[s.size()-2] == "K"){
        s.data()[s.size()-2]='\0';
        return s.toInt()*1024;
    }
    if(s.data()[s.size()-2] == "M"){
        s.data()[s.size()-2]='\0';
        return s.toInt()*1024*1024;
    }
    if(s.data()[s.size()-2] == "G"){
        s.data()[s.size()-2]='\0';
        return s.toInt()*1024*1024*1024;
    }
    if(ok)
        *ok = false;
    return 0;
}
