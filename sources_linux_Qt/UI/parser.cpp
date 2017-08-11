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

    if (parser.isSet("help"))
        throw Parser::ParseException(); // throwing an empty exception shows the help only

    // parsing positional arguments

    const QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.size() < 2)
        throw Parser::ParseException("Argument 'volumepath' missed.");
    if (positionalArguments.size() > 2)
        throw Parser::ParseException("Too many arguments specified.");

    options->path.reset(new QFileInfo(positionalArguments.at(1)));

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
            throw Parser::ParseException("--nofilesystem cannot be used with --filesystem, --protection or --options.");
    } else {
        options->doMount = true;
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
        options->password = QSharedPointer<QByteArray>(new QByteArray(password.toUtf8()));

    } else {
        QString password;
        if(askPassword("volume", password))
            options->password.reset(new QByteArray(password.toUtf8()));
    }

    if (parser.isSet("mountpoint")) {
        const QString mountpoint = parser.value("mountpoint");
        options->mountPoint.reset(new QFileInfo(mountpoint));
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

    volume->volumepath.reset(new QFileInfo(positionalArguments.at(1)));

    volume->force = false;
    //TODO add force option
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
	else if (volume == "devices")
		*item = Devices;
	else
		throw Parser::ParseException("Unknown item to list.");
}

void Parser::parseCreate(QCoreApplication &app, QCommandLineParser &parser, QSharedPointer <GostCrypt::NewCore::CreateVolumeParams> options)
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
                          {"file-system", "Specify a filesystem. Type 'gostcrypt list filesystems' to see the possibilities.", "filesystem"},
                          {{"hfile-system", "hidden-file-system"}, "Specify a filesystem for the hidden volume. Type 'gostcrypt list filesystems' to see the possibilities.", "filesystem"},
                          //{"cluster-size", "Specify a cluster size different from the default one.", "sizeinbytes"}, // very unsafe, not allowed for now
                          //{"sector-size", "Specify a sector size different from the default one.", "sizeinbytes"}, // very unsafe, not allowed for now
                          {{"s", "size", "volume-size"}, "Sets the wanted size of the created volume. For hidden volumes, the size will be asked later.", "size{B|KB|MB|GB}"},
                          {{"os", "outer-size"}, "Sets the partition size for the outer volume in percentage of the max size. default is 0.5", "percentage"},
                          {{"is", "inner-size"}, "Sets the partition size for the inner volume in percentage of the max size. default is 0.7", "percentage"},
                          {{"t", "type"}, "Sets the volume type.", "{Normal|Hidden}"}
                      });
    //parser.process(app);
    parser.parse(QCoreApplication::arguments());

    if (parser.isSet("help"))
        throw Parser::ParseException(); // if help requested we throw an empty exception, showing the help and exiting

    // parsing positional arguments
    const QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.size() < 2)
        throw Parser::ParseException("Argument 'volumepath' missed.");
    if (positionalArguments.size() > 2)
        throw Parser::ParseException("Too many arguments specified.");

    options->path = QSharedPointer<QFileInfo>(new QFileInfo(positionalArguments.at(1)));

    // Parsing all options

    if(parser.isSet("type")){
        const QString type = parser.value("type");
        if(!type.compare("Hidden",Qt::CaseInsensitive)){
            options->type = GostCrypt::VolumeType::Hidden;
            options->innerVolume.reset(new GostCrypt::NewCore::CreateVolumeParams::VolumeParams());
        } else if(type.compare("Normal",Qt::CaseInsensitive)){
            throw Parser::ParseException("Unknown Volume type. should be {Normal|Hidden}");
        }
    }

    if (parser.isSet("password")) {
        const QString password = parser.value("password");
        options->outerVolume->password.reset(new GostCrypt::VolumePassword(password.toStdWString()));
    }else{
        QString password;
        if(options->type != GostCrypt::VolumeType::Hidden){
            if(askPassword("volume", password))
                options->outerVolume->password.reset(new GostCrypt::VolumePassword(password.toStdWString()));
        } else {
            if(askPassword("outer volume", password))
                options->outerVolume->password.reset(new GostCrypt::VolumePassword(password.toStdWString()));
        }
    }

    if(options->type != GostCrypt::VolumeType::Hidden){
        if (parser.isSet("hpassword") || parser.isSet("hfile") || parser.isSet("hash") || parser.isSet("halgorithm") || parser.isSet("hfile-system"))
            throw Parser::ParseException("Options for hidden volumes should only be used with --type=Hidden");
    } else {
        if (parser.isSet("hpassword")) {
            const QString hpassword = parser.value("hpassword");
            options->innerVolume->password.reset(new GostCrypt::VolumePassword(hpassword.toStdWString()));
        }else{
            QString hpassword;
            if(askPassword("inner volume", hpassword))
                options->innerVolume->password.reset(new GostCrypt::VolumePassword(hpassword.toStdWString()));
        }

        if (parser.isSet("hfile")) { // TODO
            throw Parser::ParseException("keyfiles not implemented yet");
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
            options->innerVolume->filesystem = GostCrypt::NewCore::GetFileSystemTypePlatformNative(); // default value
        }

        if (parser.isSet("inner-size")) {
            const QString number = parser.value("inner-size");
            bool ok = false;
            options->innerVolume->size = number.toDouble(&ok);
            if (!ok)
                throw Parser::ParseException("'size' must be a percentage like 0.5, not 50%.");
        }else{
            options->innerVolume->size = DEFAULT_INNER_SIZE; // default value
        }
    }

    if (parser.isSet("file")) { // TODO
        throw Parser::ParseException("keyfiles not implemented yet");
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

    if (parser.isSet("file-system")) {
        options->outerVolume->filesystem = parser.value("file-system");
    }else{
        options->outerVolume->filesystem = GostCrypt::NewCore::GetFileSystemTypePlatformNative(); // default value
    }

    if (parser.isSet("outer-size")) {
        const QString number = parser.value("outer-size");
        bool ok = false;
        options->outerVolume->size = number.toDouble(&ok);
        if (!ok)
            throw Parser::ParseException("'size' must be a percentage like 0.5, not 50%.");
    }else{
        options->outerVolume->size = DEFAULT_OUTER_SIZE; // default value
    }

    if (parser.isSet("size")) {
        const QString number = parser.value("size");
        bool ok = false;
        options->size = parseSize(number, &ok);
        if (!ok)
            throw Parser::ParseException("'size' must be a number followed by B,KB,MB or GB !");
    }else{
        options->size = DEFAULT_SIZE; // default value is 10Mio
    }
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

    // the only std::cout authorized
    std::cout << "Please enter the " << volume << " password (leave empty if none) : " << std::endl;
    getline(std::cin, pass);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // reset back the terminal
    if(pass == "")
        return false;
    p = QString(pass.c_str());
    return true;
}
