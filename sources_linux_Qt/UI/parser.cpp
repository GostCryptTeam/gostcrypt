#include "parser.h"
#include <termios.h>
#include <unistd.h>

void Parser::parseMount(QCoreApplication &app, QCommandLineParser &parser, QSharedPointer <GostCrypt::NewCore::MountVolumeParams> options)
{
    parser.addPositionalArgument("mount", "Mounts a volume.", "mount");
    parser.addPositionalArgument("volumepath", "Path of the volume or the device to mount.", "path");
    parser.addOptions({
                          {{"o","options"}, "Additional options to pass to the mount function.", "'-option myoption'"},
                          {"filesystem", "Force a special filesystem to the volume."
                                "Type 'gostcrypt list filesystems' to list the different filesystems supported.", "{fat,ntfs,ext2,...}"},
                          {"no-filesystem", "Doesn't mount the volume."},
                          {{"t","no-preserve-timestamps"}, "Doesn't preserves the timestamps."},
                          {{"f", "file", "keyfile"}, "Adds a keyfile.", "file"},
                          {{"p","password"}, "Specify an inline password. (unsafe)", "password"},
                          {{"m","mountpoint"}, "Specify a special mountpoint.", "mountpoint"},
                          {"protection", "Specify a special volume protection", "{none|readonly}"},
                          {{"s","shared"}, "Allows shared access."},
                          {{"b","backup-headers"}, "Use backup headers."},
                      });
    parser.process(app);

    // Parsing all options

    if (parser.isSet("help"))
        throw Parser::ParseException(); // throwing an empty exception shows the help only

    if (parser.isSet("options")) {
        const QStringList opts = parser.values("options");
        for(QString option : opts){
            options->fileSystemOptions += option + " ";
        }
    }

    if (parser.isSet("filesystem")) { // filesystemtype
        const QString fs = parser.value("filesystem");
        options->fileSystemType = parseFilesystem(fs);
    }

    if (parser.isSet("no-filesystem")) { // nofilesystem
        options->noFileSystem = true;
        if(parser.isSet("filesystem") || parser.isSet("options") || parser.isSet("protection"))
            throw Parser::ParseException("--nofilesystem cannot be used with --filesystem, --protection or --options.");
    }

    if (parser.isSet("no-preserve-timestamps"))
        options->preserveTimestamps = false;

    if (parser.isSet("file")) {
        const QStringList files = parser.values("file");
        GostCrypt::KeyfileList* keyfiles = new GostCrypt::KeyfileList();
        for(QString file : files){
            keyfiles->push_back(shared_ptr<GostCrypt::Keyfile>(new GostCrypt::Keyfile(file.toStdString())));
        }
        options->keyfiles.reset(keyfiles);
    }

    if (parser.isSet("password")) {
        const QString password = parser.value("password");
        options->password = QSharedPointer<GostCrypt::VolumePassword>(new GostCrypt::VolumePassword(password.toStdWString()));
    } else {
        QString password;
        if(askPassword("volume", password))
            options->password.reset(new GostCrypt::VolumePassword(password.toUtf8().constData(), password.toUtf8().size()));
    }

    if (parser.isSet("mountpoint")) {
        const QString mountpoint = parser.value("mountpoint");
        options->mountPoint.reset(new GostCrypt::DirectoryPath(qPrintable(mountpoint)));
    }

    if (parser.isSet("shared"))
        options->sharedAccessAllowed = true;

    if (parser.isSet("backup-headers"))
        options->useBackupHeaders = true;

    if (parser.isSet("protection")) { // volume protection
        const QString protection = parser.value("protection");
        options->protection = GostCrypt::VolumeProtection::None;
        if(protection == "readonly")
                options->protection = GostCrypt::VolumeProtection::ReadOnly;
        else if(protection != "none")
            throw Parser::ParseException("Protection type not found : "+ protection);
    }

    // parsing positional arguments

    const QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.size() < 2)
        throw Parser::ParseException("Argument 'volumepath' missed.");
    if (positionalArguments.size() > 2)
        throw Parser::ParseException("Too many arguments specified.");

    options->path.reset(new GostCrypt::VolumePath(qPrintable(positionalArguments.at(1))));
}

void Parser::parseDismount(QCoreApplication &app, QCommandLineParser &parser, QSharedPointer <GostCrypt::NewCore::DismountVolumeParams> volume)
{
    parser.addPositionalArgument("umount", "Mounts a volume.", "{umount|unmount|dismount}");
    parser.addPositionalArgument("volume", "Path of the volume or the device to unmount");
    parser.process(app);

    // Parsing all options

    if (parser.isSet("help"))
        throw Parser::ParseException();

    // parsing positional arguments

    const QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.size() < 2)
        throw Parser::ParseException("Argument 'volume' missed.");
    if (positionalArguments.size() > 2)
        throw Parser::ParseException("Too many arguments specified.");

    volume->volumepath = positionalArguments.at(1);
}

void Parser::parseList(QCoreApplication &app, QCommandLineParser &parser, Parser::WhatToList *item)
{
    parser.addPositionalArgument("list", "Mounts a volume.", "list");
    parser.addPositionalArgument("item", "Item to list", "{volumes|algorithms|hashs|filesystems}");
    parser.process(app);

    // Parsing all options

    if (parser.isSet("help"))
        throw Parser::ParseException();

    // parsing positional arguments

    const QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.size() < 2){
        *item = Volumes;
        return;
    }
    if (positionalArguments.size() > 2)
        throw Parser::ParseException("Too many arguments specified.");
    QString volume = positionalArguments.at(1);
    if(volume == "volumes")
        *item = Volumes;
    else if (volume == "algorithms")
        *item = Algorithms;
    else if (volume == "hashs")
        *item = Hashs;
    else if (volume == "filesystems")
        *item = FileSystems;
    else
        throw Parser::ParseException("Unknown item to list.");
}

void Parser::parseCreate(QCoreApplication &app, QCommandLineParser &parser, QSharedPointer <GostCrypt::NewCore::CreateVolumeParams> options)
{
    parser.addPositionalArgument("create", "Creates a volume.", "create");
    parser.addPositionalArgument("volumepath", "Path of the volume to create", "path");
    parser.addOptions({
                          {{"p","password"}, "Specify an inline password.", "password"},
                          {{"hpassword","hidden-password"}, "Specify an inline hidden password.", "password"},
                          {{"f", "file", "keyfile"}, "Adds a keyfile.", "file"},
                          {{"hfile", "hkeyfile"/*,"hidden-file", "hidden-keyfile"*/}, "Adds a keyfile for the hidden volume.", "file"},
                          {{"h", "hash"}, "Chooses the hash function. Type 'gostcrypt list hashs' to see the possibilities.", "hashfunction"},
                          {{"hhash","hidden-hash"}, "Chooses the hash function for the hidden volume. Type 'gostcrypt list hashs' to see the possibilities.", "hashfunction"},
                          {{"a", "algorithm"}, "Chooses the encryption algorithm. Type 'gostcrypt list algorithms' to see the possibilities.", "algorithm"},
                          {{"halgorithm", "hidden-algorithm"}, "Chooses the encryption algorithm for the hidden volume. Type 'gostcrypt list algorithms' to see the possibilities.", "algorithm"},
                          //{{"q","Quick"}, "Enable quick formatting."}, // available only for the software itself ?
                          {"file-system", "Specify a filesystem. Type 'gostcrypt list filesystems' to see the possibilities."},
                          {{"hfile-system", "hidden-file-system"}, "Specify a filesystem for the hidden volume. Type 'gostcrypt list filesystems' to see the possibilities."},
                          //{"cluster-size", "Specify a cluster size different from the default one.", "sizeinbytes"}, // very unsafe, not allowed for now
                          //{"sector-size", "Specify a sector size different from the default one.", "sizeinbytes"}, // very unsafe, not allowed for now
                          {{"s", "size", "volume-size"}, "Sets the wanted size of the created volume. For hidden volumes, the size will be asked later.", "size{B|KB|MB|GB}"},
                          {{"t", "type"}, "Sets the volume type.", "{Normal|Hidden}"}
                      });
    //parser.process(app);
    parser.parse(QCoreApplication::arguments());

    // Parsing all options

    if (parser.isSet("help"))
        throw Parser::ParseException();

    if(parser.isSet("type")){
        const QString type = parser.value("type");
        if(!type.compare("Hidden",Qt::CaseInsensitive)){
            type = GostCrypt::VolumeType::Hidden;
        } else if(type.compare("Normal",Qt::CaseInsensitive)){
            throw Parser::ParseException("Unknown Volume type. should be {Normal|Hidden}");
        }
    }

    /*if (parser.isSet("password")) {
        const QString password = parser.value("password");
        options->outerVolume->password.reset(new VolumePassword(password));
    }else{

        if(password != "")
            options->setPassword(QString::fromStdString(password));
    }

    if (parser.isSet("file")) {
        options->setKeyFiles("ho ho"); // TODO DO SOMETHING
        throw Parser::ParseException("WELL WE HAVE A PROBLEM HERE");
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
            throw Parser::ParseException(myhash + " not found !");
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
            throw Parser::ParseException("algorithm " + algo + " not found !");
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
            throw Parser::ParseException("Filesystem " + fs + " not found !");
    } else {
        options->Filesystem = GostCrypt::VolumeCreationOptions::FilesystemType::GetPlatformNative();
    }

    if (parser.isSet("cluster-size")) {
        const QString number = parser.value("cluster-size");
        bool ok = false;
        options->FilesystemClusterSize = number.toInt(&ok);
        if (!ok)
            throw Parser::ParseException("'cluster-size' must be a number !");
    }

    if (parser.isSet("sector-size")) {
        const QString number = parser.value("sector-size");
        bool ok = false;
        options->SectorSize = number.toInt(&ok);
        if (!ok)
            throw Parser::ParseException("'sector-size' must be a number !");
    }

    if (parser.isSet("size")) {
        const QString number = parser.value("size");
        bool ok = false;
        options->size = parseSize(number, &ok);
        if (!ok)
            throw Parser::ParseException("'size' must be a number followed by B,KB,MB or GB !");
    }

    /*if (parser.isSet("type")) {
        const QString type = parser.value("type");
        if(type == "Normal" || type == "normal"){
            options->Type = GostCrypt::VolumeType::Normal;
        } else if(type == "Hidden" || type == "hidden") {
            options->Type = GostCrypt::VolumeType::Hidden;
        } else
            throw Parser::ParseException("'type' must be one of {Normal|Hidden} !");
    }*/

    // parsing positional arguments

    const QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.size() < 2)
        throw Parser::ParseException("Argument 'volumepath' missed.");
    if (positionalArguments.size() > 2)
        throw Parser::ParseException("Too many arguments specified.");

    options->path = GostCrypt::VolumePath(qPrintable(positionalArguments.at(1)));
}

quint64 Parser::parseSize(QString s, bool *ok){
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

bool Parser::askPassword(string volume, QString &p){
    string pass;
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // hide input

    std::cout << "Please enter the " << volume << " password (leave empty if none) : " << std::endl;
    getline(std::cin, pass);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // reset back the terminal
    if(pass == "")
        return false;
    p = QString(pass.c_str());
    return true;
}

GostCrypt::NewCore::FilesystemType::Enum Parser::parseFilesystem(QString fs){
    int r = -1;
    r = GostCrypt::NewCore::FilesystemType::Str.indexOf(QRegExp(fs, Qt::CaseInsensitive));
    if(r == -1)
        throw Parser::ParseException("Unknown filesystem : "+fs);
    return GostCrypt::NewCore::FilesystemType::Enum(r);
}
