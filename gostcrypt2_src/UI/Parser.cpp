#include "Parser.h"
#include <termios.h>
#include <unistd.h>

void Parser::parseMount(QCommandLineParser &parser, QSharedPointer <GostCrypt::Core::MountVolumeRequest> options)
{
    parser.addPositionalArgument("mount", "Mounts a volume.", "mount");
    parser.addPositionalArgument("volumepath", "Path of the volume or the device to mount.", "volumepath");
    parser.addOptions({
                          {{"o","options"}, "Additional options to pass to the mount function.", "'-option myoption'"},
                          {"filesystem", "Force a special filesystem to the volume."
                                "Type 'gostcrypt list filesystems' to list the different filesystems supported.", "{fat,ntfs,ext2,...}"},
                          {"no-filesystem", "Don't mount the volume."},
                          {{"t","no-preserve-timestamps"}, "Don't preserves the timestamps."},
                          {{"f", "file", "keyfile"}, "Adds a keyfile.", "file"},
                          {{"p","password"}, "Specify an inline password. (unsafe)", "password"},
                          {{"m","mountpoint"}, "Specify a special mountpoint.", "mountpoint"},
                          {"protection", "Specify a special volume protection", "{none|readonly}"},
                          {{"b","backup-headers"}, "Use backup headers."},
                          {{"u","user"}, "Gives ownership of the folder to this user.", "user"},
                          {{"g","group"}, "Gives ownership of the folder to this group.", "group"},
                      });
    parser.parse(QCoreApplication::arguments());

    if (parser.isSet("help"))
        throw Parser::ParseException(); // throwing an empty exception shows the help only

    // parsing positional arguments

    const QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.size() < 2)
        throw Parser::ParseException(QString("Argument 'volumepath' missed."));
    if (positionalArguments.size() > 2)
        throw Parser::ParseException(QString("Too many arguments specified."));

    options->path.setFile(positionalArguments.at(1));

    // Parsing all options

    if (parser.isSet("options")) {
        const QStringList opts = parser.values("options");
        for(QString option : opts){
            options->fileSystemOptions += option + " ";
        }
    }

    if (parser.isSet("filesystem")) { // filesystemtype
        options->fileSystemType = parser.value("filesystem"); // TODO check if exists
    }

    if (parser.isSet("no-filesystem")) { // nofilesystem
        options->doMount = false;
        if(parser.isSet("filesystem") || parser.isSet("options") || parser.isSet("protection"))
            throw Parser::ParseException(QString("--nofilesystem cannot be used with --filesystem, --protection or --options."));
    } else {
        options->doMount = true;
    }

    if (parser.isSet("no-preserve-timestamps"))
        options->preserveTimestamps = false;

    if (parser.isSet("file")) {
        const QStringList files = parser.values("file");
        options->keyfiles.reset(new QList<QSharedPointer<QFileInfo>>());
        for(QString file : files){
            options->keyfiles->append(QSharedPointer<QFileInfo>(new QFileInfo(file)));
        }
    }

    if (parser.isSet("password")) {
        const QString password = parser.value("password");
        options->password = QSharedPointer<QByteArray>(new QByteArray(password.toUtf8()));

    } else {
        QString password;
        if(askPassword("volume", password))
            options->password.reset(new QByteArray(password.toUtf8()));
    }

    if (parser.isSet("mountpoint")) {
        const QString mountpoint = parser.value("mountpoint");
        options->mountPoint.setFile(mountpoint);
    }

    if (parser.isSet("backup-headers"))
        options->useBackupHeaders = true;

    if (parser.isSet("protection")) { // volume protection
        const QString protection = parser.value("protection");
        options->protection = GostCrypt::Volume::VolumeProtection::None;
        if(protection == "readonly")
                options->protection = GostCrypt::Volume::VolumeProtection::ReadOnly;
        else if(protection != "none")
            throw Parser::ParseException(QString("Protection type not found : "+ protection));
    }

    if(parser.isSet("user")){
        options->mountedForUser = parser.value("user");
    }

    if(parser.isSet("group")){
        options->mountedForGroup = parser.value("group");
    }
}

void Parser::parseDismount(QCommandLineParser &parser, QSharedPointer <GostCrypt::Core::DismountVolumeRequest> volume)
{
	parser.addPositionalArgument("umount", "Mounts a volume.", "{umount|unmount|dismount}");
	parser.addPositionalArgument("volume", "Path of the volume or the device to unmount");
    parser.parse(QCoreApplication::arguments());

	// Parsing all options
	if (parser.isSet("help"))
		throw Parser::ParseException();

	// parsing positional arguments

	const QStringList positionalArguments = parser.positionalArguments();
	if (positionalArguments.size() < 2)
        throw Parser::ParseException(QString("Argument 'volume' missed."));
	if (positionalArguments.size() > 2)
        throw Parser::ParseException(QString("Too many arguments specified."));

    volume->volumePath.setFile(positionalArguments.at(1));
    volume->all = false;
    volume->force = false;
    //TODO add force option
}

void Parser::parseList(QCommandLineParser &parser, Parser::WhatToList *item)
{
    parser.addPositionalArgument("list", "Lists an item. For example you can lists the different algorithms that can be used in the program.", "list");
	parser.addPositionalArgument("item", "Item to list", "{volumes|algorithms|hashs|filesystems}");
    parser.parse(QCoreApplication::arguments());

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
        throw Parser::ParseException(QString("Too many arguments specified."));
	QString volume = positionalArguments.at(1);
    if(volume == "volumes" || volume == "volume")
		*item = Volumes;
    else if (volume == "algorithms" || volume == "algorithm")
		*item = Algorithms;
    else if (volume == "hashs" || volume == "hash")
		*item = Hashs;
    else if (volume == "devices" || volume == "device")
		*item = Devices;
	else
        throw Parser::ParseException(QString("Unknown item to list."));
}

void Parser::parseCreate(QCommandLineParser &parser, QSharedPointer <GostCrypt::Core::CreateVolumeRequest> options)
{
    parser.addPositionalArgument("create", "Creates a volume.", "create");
    parser.addPositionalArgument("volumepath", "Path of the volume to create", "path"); // TODO add default values to description
    parser.addOptions({
                          {{"p","password"}, "Specify an inline password.", "password"},
                          {{"hpassword","hidden-password"}, "Specify an inline hidden password.", "password"},
                          {{"f", "file", "keyfile"}, "Adds a keyfile.", "file"},
                          {{"hfile", "hkeyfile"/*,"hidden-file", "hidden-keyfile"*/}, "Adds a keyfile for the hidden volume.", "file"},
                          {"hash", "Chooses the hash function. Type 'gostcrypt list hashs' to see the possibilities.", "hashfunction"},
                          {{"hhash","hidden-hash"}, "Chooses the hash function for the hidden volume. Type 'gostcrypt list hashs' to see the possibilities.", "hashfunction"},
                          {{"a", "algorithm"}, "Chooses the encryption algorithm. Type 'gostcrypt list algorithms' to see the possibilities.", "algorithm"},
                          {{"halgorithm", "hidden-algorithm"}, "Chooses the encryption algorithm for the hidden volume. Type 'gostcrypt list algorithms' to see the possibilities.", "algorithm"},
                          {"filesystem", "Specify a filesystem. Type 'gostcrypt list filesystems' to see the possibilities.", "filesystem"},
                          {{"hfile-system", "hidden-file-system"}, "Specify a filesystem for the hidden volume. Type 'gostcrypt list filesystems' to see the possibilities.", "filesystem"},
                          //{"cluster-size", "Specify a cluster size different from the default one.", "sizeinbytes"}, // very unsafe, not allowed for now
                          //{"sector-size", "Specify a sector size different from the default one.", "sizeinbytes"}, // very unsafe, not allowed for now
                          {{"s", "size", "volume-size"}, "Sets the wanted size of the created volume. For hidden volumes, the size will be asked later.", "size{B|KB|MB|GB}"},
                          {{"os", "outer-size"}, "Sets the partition size for the outer volume in percentage of the max size. default is 0.5", "percentage"},
                          {{"is", "inner-size"}, "Sets the partition size for the inner volume in percentage of the max size. default is 0.7", "percentage"},
                          {{"t", "type"}, "Sets the volume type.", "{Normal|Hidden}"}
                      });

    parser.parse(QCoreApplication::arguments());

    if (parser.isSet("help"))
        throw Parser::ParseException(); // if help requested we throw an empty exception, showing the help and exiting

    // parsing positional arguments
    const QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.size() < 2)
        throw Parser::ParseException(QString("Argument 'volumepath' missed."));
    if (positionalArguments.size() > 2)
        throw Parser::ParseException(QString("Too many arguments specified."));

    options->path.setFile(positionalArguments.at(1));

    // Parsing all options

    if(parser.isSet("type")){
        const QString type = parser.value("type");
        if(!type.compare("Hidden",Qt::CaseInsensitive)){
            options->type = GostCrypt::Volume::VolumeType::Hidden;
            options->innerVolume.reset(new GostCrypt::Core::CreateVolumeRequest::VolumeParams());
        } else if(type.compare("Normal",Qt::CaseInsensitive)){
            throw Parser::ParseException(QString("Unknown Volume type. should be {Normal|Hidden}"));
        }
    }

    if (parser.isSet("password")) {
        const QString password = parser.value("password");
        options->outerVolume->password.reset(new QByteArray(password.toUtf8()));
    }else{
        QString password;
        if(options->type != GostCrypt::Volume::VolumeType::Hidden){
            if(askPassword("volume", password))
                options->outerVolume->password.reset(new QByteArray(password.toUtf8()));
        } else {
            if(askPassword("outer volume", password))
                options->outerVolume->password.reset(new QByteArray(password.toUtf8()));
        }
    }

    if(options->type != GostCrypt::Volume::VolumeType::Hidden){
        if (parser.isSet("hpassword") || parser.isSet("hfile") || parser.isSet("hash") || parser.isSet("halgorithm") || parser.isSet("hfile-system"))
            throw Parser::ParseException(QString("Options for hidden volumes should only be used with --type=Hidden"));
    } else {
        if (parser.isSet("hpassword")) {
            const QString hpassword = parser.value("hpassword");
            options->innerVolume->password.reset(new QByteArray(hpassword.toUtf8()));
        }else{
            QString hpassword;
            if(askPassword("inner volume", hpassword))
                options->innerVolume->password.reset(new QByteArray(hpassword.toUtf8()));
        }

        if (parser.isSet("hfile")) { // TODO
            throw Parser::ParseException(QString("keyfiles not implemented yet"));
        }

        if (parser.isSet("hhash")) {
            options->innerVolume->volumeHeaderKdf = parser.value("hhash");
        }else{
            options->innerVolume->volumeHeaderKdf = DEFAULT_KDF; // default value
        }

        if (parser.isSet("halgorithm")) {
            options->innerVolume->encryptionAlgorithm = parser.value("halgorithm");
        }else{
            options->innerVolume->encryptionAlgorithm = DEFAULT_ALGORITHM; // default value
        }

        if (parser.isSet("hfile-system")) {
            options->innerVolume->filesystem = parser.value("hfile-system");
        }else{
            options->innerVolume->filesystem = GostCrypt::Core::GetFileSystemTypePlatformNative(); // default value
        }

        if (parser.isSet("inner-size")) {
            const QString number = parser.value("inner-size");
            bool ok = false;
            options->innerVolume->size = number.toDouble(&ok);
            if (!ok)
                throw Parser::ParseException(QString("'size' must be a percentage like 0.5, not 50%."));
        }else{
            options->innerVolume->size = DEFAULT_INNER_SIZE; // default value
        }
    }

    if (parser.isSet("file")) { // TODO
        throw Parser::ParseException(QString("keyfiles not implemented yet"));
    }

    if (parser.isSet("hash")) {
        options->outerVolume->volumeHeaderKdf = parser.value("hash");
    }else{
        options->outerVolume->volumeHeaderKdf = DEFAULT_KDF; // default value
    }

    if (parser.isSet("algorithm")) {
        options->outerVolume->encryptionAlgorithm = parser.value("algorithm");
    }else{
        options->outerVolume->encryptionAlgorithm = DEFAULT_ALGORITHM; // default value
    }

    if (parser.isSet("filesystem")) {
        options->outerVolume->filesystem = parser.value("filesystem");
    }else{
        options->outerVolume->filesystem = GostCrypt::Core::GetFileSystemTypePlatformNative(); // default value
    }

    if (parser.isSet("outer-size")) {
        const QString number = parser.value("outer-size");
        bool ok = false;
        options->outerVolume->size = number.toDouble(&ok);
        if (!ok)
            throw Parser::ParseException(QString("'size' must be a percentage like 0.5, not 50%."));
    }else{
        options->outerVolume->size = DEFAULT_OUTER_SIZE; // default value
    }

    if (parser.isSet("size")) {
        const QString number = parser.value("size");
        bool ok = false;
        options->size = UserInterface::parseSize(number, &ok);
        if (!ok)
            throw Parser::ParseException(QString("'size' must be a number followed by B,KB,MB or GB !"));
    }else{
        options->size = DEFAULT_SIZE; // default value is 10Mio
    }
}

void Parser::parseCreateKeyFiles(QCommandLineParser &parser, QStringList &files)
{
    parser.addPositionalArgument("createkeyfiles", "Creates a keyfile", "createkeyfiles");
    parser.addPositionalArgument("paths", "Paths of the keyfiles to create", "path0 [path1] ...");
    parser.parse(QCoreApplication::arguments());

    // Parsing all options

    if (parser.isSet("help"))
        throw Parser::ParseException();

    // parsing positional arguments

    const QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.size() < 2)
        throw Parser::ParseException(QString("Please Specify the path of at least one keyfile to create."));
    if (positionalArguments.size() >= 2)
        for(int i=1; i<positionalArguments.size(); i++)
            files.append(positionalArguments.at(i));
}

void Parser::parseBenchmark(QCommandLineParser &parser, QSharedPointer<GostCrypt::Core::BenchmarkAlgorithmsRequest> options)
{

    parser.addPositionalArgument("benchmark","execute the benchmark of the differents cipher algorithms", "benchmark");
    parser.addOption({{"s","size"}, "Precise the size of the buffer to benchmark", "datasize"} );
    parser.parse(QCoreApplication::arguments());

    if (parser.isSet("help"))
        throw Parser::ParseException(); // if help requested we throw an empty exception, showing the help and exiting

    // parsing positional arguments
    const QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.size() > 1)
        throw Parser::ParseException(QString("Too many arguments specified."));

    if (!parser.isSet("size")) {
       options->bufferSize = 1024;
       return;
    }

    bool isOk;
    options->bufferSize = UserInterface::parseSize(parser.value("size"), &isOk);
    if (!isOk)
       throw Parser::ParseException(QString("'size' must be a number followed by B,KB,MB or GB !"));
}

bool Parser::askPassword(std::string volume, QString &p){
    std::string pass;
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // hide input

    // the only std::cout authorized
    std::cout << "Please enter the " << volume << " password (leave empty if none) : " << std::endl;
    getline(std::cin, pass);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // reset back the terminal
    if(pass == "")
        return false;
    p = QString(pass.c_str());
    return true;
}
