#include "CmdLineInterface.h"
#include "NewCore/CoreException.h"

const QStringList CmdLineInterface::FirstCMD::Str = MK_ALL_COMMANDS(MK_STRTAB);

CmdLineInterface::CmdLineInterface(QObject *parent) : QObject(parent)
{

}

QTextStream &CmdLineInterface::qStdOut() {
    static QTextStream ts( stdout );
    return ts;
}

int CmdLineInterface::start(int argc, char **argv)
{
    MyApplication app(argc, argv);
    core = GostCrypt::NewCore::getCore();

    MyApplication::setApplicationName("GostCrypt");

    /* Connecting all signals */

    /* Connecting responses from Core to display from this */
    app.connect(core.data(), SIGNAL(sendCreateVolume(QSharedPointer<GostCrypt::NewCore::CreateVolumeResponse>)), this, SLOT(printCreateVolume(QSharedPointer<GostCrypt::NewCore::CreateVolumeResponse>)));
    app.connect(core.data(), SIGNAL(sendMountVolume(QSharedPointer<GostCrypt::NewCore::MountVolumeResponse>)), this, SLOT(printMountVolume(QSharedPointer<GostCrypt::NewCore::MountVolumeResponse>)));
    app.connect(core.data(), SIGNAL(sendDismountVolume(QSharedPointer<GostCrypt::NewCore::DismountVolumeResponse>)), this, SLOT(printDismountVolume(QSharedPointer<GostCrypt::NewCore::DismountVolumeResponse>)));
    app.connect(core.data(), SIGNAL(sendGetMountedVolumes(QSharedPointer<GostCrypt::NewCore::GetMountedVolumesResponse>)), this, SLOT(printGetMountedVolumes(QSharedPointer<GostCrypt::NewCore::GetMountedVolumesResponse>)));

    /* Connecting request from here to request switch from Core */
    app.connect(this, SIGNAL(request(QVariant)), core.data(), SLOT(request(QVariant)));

    /* Connecting few exit signals to close the program apropriately */
    app.connect(this, SIGNAL(exit()), core.data(), SLOT(exit()));
    app.connect(&app, SIGNAL(exit()), core.data(), SLOT(exit()));
    app.connect(core.data(), SIGNAL(exited()), &app, SLOT(quit()));

    /* Connecting the signals to get the sudo request from Core and send it to Core */
    app.connect(core.data(), SIGNAL(askSudoPassword()), this, SLOT(askSudoPassword()));
    app.connect(this, SIGNAL(sendSudoPassword(QSharedPointer<QByteArray>)), core.data(), SLOT(receiveSudoPassword(QSharedPointer<QByteArray>)));

    /* Processing the commands passed */
    try {
        processRequest();
    } catch(GostCrypt::NewCore::CoreException &e) {
        qDebug().noquote() << e.qwhat();
        return -1;
    } catch (QException &e) { // TODO : handle exceptions here
        qDebug() << e.what();
        return -1;
    }

    return app.exec();
}

void CmdLineInterface::processRequest(){
    parser.setApplicationDescription("This is the command line interface for the GostCrypt library.");
    parser.addHelpOption();
    parser.addPositionalArgument("command", "The command to execute.","{mount|create|umount|test|dismountall|automount|backupheaders|createkeyfiles|list}");

    parser.parse(QCoreApplication::arguments());

    const QStringList args = parser.positionalArguments();
    const QString command = args.isEmpty() ? QString() : args.first();

    if(args.length() == 0 && parser.isSet("help"))
        parser.showHelp();

    if(args.length() == 0) // nothing to do. Probably launched with --help
        return;

    parser.clearPositionalArguments();

    uint32 value = FirstCMD::Str.indexOf(QRegExp(command, Qt::CaseInsensitive)); // using the mix between a QString tab and a enumeration.

    switch(value){
        case FirstCMD::mount: //"mount":
            {
                QSharedPointer <GostCrypt::NewCore::MountVolumeParams> options(new GostCrypt::NewCore::MountVolumeParams);
                Parser::parseMount(parser, options);
                emit request(QVariant::fromValue(options));
            }
            break;
        case FirstCMD::createvolume://"create-volume":
        case FirstCMD::create://"create":
            {
                QSharedPointer <GostCrypt::NewCore::CreateVolumeParams> options(new GostCrypt::NewCore::CreateVolumeParams);
                Parser::parseCreate(parser, options);
                emit request(QVariant::fromValue(options));
            }
            break;
        case FirstCMD::umount://"umount":
        case FirstCMD::unmount://"unmount":
        case FirstCMD::dismount://"dismount":
            {
                QSharedPointer<GostCrypt::NewCore::DismountVolumeParams> options(new GostCrypt::NewCore::DismountVolumeParams);
                Parser::parseDismount(parser, options);
                emit request(QVariant::fromValue(options));
            }
            break;
        /*case FirstCMD::test://"test":
            qStdOut() << "Option not supported." << endl; // TODO
            break;
        case FirstCMD::dismountall://"dismountall":
            {
                try {
                    Core->dismountVolume();
                    qStdOut() << "All volumes dismounted." << endl;
                } catch(GostCrypt::NewCore::CoreException &e) {
                    qStdOut() << e.qwhat();
                } catch(...) {
                    qStdOut() << "Unknown exception raised.";
                }
            }
            break;
        case FirstCMD::automount://"automount":
            qStdOut() << "Option not supported." << endl; // TODO
            break;
        case FirstCMD::backupheaders://"backupheaders":
            qStdOut() << "Option not supported." << endl; // TODO
            break;
        case FirstCMD::createkeyfiles://"createkeyfiles":
            {
                QStringList files;
                Parser::parseCreateKeyFiles(parser, files);
                QSharedPointer <GostCrypt::NewCore::CreateKeyFileParams> params;
                params.reset(new GostCrypt::NewCore::CreateKeyFileParams());
                for(QStringList::Iterator file = files.begin(); file != files.end(); file++){
                    params->file.reset(new QFileInfo(*file));
                    Core->createKeyFile(params);
                }
                qStdOut() << "All keyfiles created." << endl;
            }
            break;
        case FirstCMD::list://"list":
            {
                Parser::WhatToList item;
                try {
                    Parser::parseList(parser, &item);
                    switch(item){
                        case Parser::Volumes:
                            {
                                QSharedPointer <GostCrypt::NewCore::GetMountedVolumesParams> params;
                                QSharedPointer <GostCrypt::NewCore::GetMountedVolumesResponse> response;
                                params.reset(new GostCrypt::NewCore::GetMountedVolumesParams());
                                response = Core->getMountedVolumes(params);
                                for(auto v = response->volumeInfoList.begin(); v < response->volumeInfoList.end(); ++v){
                                    qStdOut() << QString::fromStdString(string((*v)->Path)) << "\t";
                                    qStdOut() << QString::fromStdString(string((*v)->MountPoint)) << "\t";
                                    qStdOut() << (*v)->Size << "\t";
                                    qStdOut() << QString::fromStdWString((*v)->EncryptionAlgorithmName) << endl;
                                }
                            }
                            break;
                        case Parser::Algorithms:
                            {
                                QSharedPointer <GostCrypt::NewCore::GetEncryptionAlgorithmsResponse> response(new GostCrypt::NewCore::GetEncryptionAlgorithmsResponse);
                                response = Core->getEncryptionAlgorithms();
                                for(QString algo : response->algorithms) {
                                    qStdOut() << algo << endl;
                                }
                            }
                            break;
                        case Parser::Hashs:
                            {
                                QSharedPointer <GostCrypt::NewCore::GetDerivationFunctionsResponse> response(new GostCrypt::NewCore::GetDerivationFunctionsResponse);
                                response = Core->getDerivationFunctions();
                                for(QString algo : response->algorithms) {
                                    qStdOut() << algo << endl;
                                }
                            }
                            break;
                        case Parser::FileSystems:
                            {
                                QSharedPointer <GostCrypt::NewCore::GetFileSystemsTypesSupportedResponse> response(new GostCrypt::NewCore::GetFileSystemsTypesSupportedResponse);
                                response = Core->getFileSystemsTypesSupported();
                                for(QString fst : response->filesystems) {
                                    qStdOut() << fst << endl;
                                }
                            }
                            break;
                        case Parser::Devices:
                            {
                                QSharedPointer<GostCrypt::NewCore::GetHostDevicesResponse> response(new GostCrypt::NewCore::GetHostDevicesResponse);
                                response = Core->getHostDevices();
                                for(QSharedPointer<GostCrypt::NewCore::HostDevice> d : response->hostDevices) {
                                    qStdOut() << d->devicePath->absoluteFilePath() << "\t";
                                    if(d->mountPoint)
                                        qStdOut() << d->mountPoint->absoluteFilePath() << "\t";
                                    else
                                        qStdOut() << "no mountpoint\t";
                                    qStdOut() << d->size << endl;
                                    for(QSharedPointer<GostCrypt::NewCore::HostDevice> p : d->partitions) {
                                        qStdOut() << "\t" << p->devicePath->absoluteFilePath() << "\t";
                                        if(p->mountPoint)
                                            qStdOut() << p->mountPoint->absoluteFilePath() << "\t";
                                        else
                                            qStdOut() << "no mountpoint\t";
                                        qStdOut() << p->size << endl;
                                    }
                                }
                            }
                    }

                } catch(Parser::ParseException &e){
                    qStdOut() << e.getMessage() << endl;
                    parser.showHelp();
                }
            }
            break;*/
        default:
            qStdOut() << "Not a valid command. Try --help for help." << endl;
    }
}

void CmdLineInterface::askSudoPassword()
{
    QSharedPointer<QByteArray> password(new QByteArray("a", 1));

    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // hide input

    qStdOut() << "Please enter your sudo password: " << endl;

    // Temporaire mot de passe toujours en mémoire
    QFile inputFile;
    QTextStream in(&inputFile);
    inputFile.open(stdin, QFile::ReadOnly);
    QString line = in.readLine();
    inputFile.close();
    *password = line.toUtf8();
    line.fill('\0');

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // reset back the previous configuration
    emit sendSudoPassword(password);
}

bool MyApplication::notify(QObject *receiver, QEvent *event)
{
    bool done = true;
    try {
        done = QCoreApplication::notify(receiver, event);
    } catch(GostCrypt::NewCore::CoreException &e) {
        CmdLineInterface::qStdOut() << e.displayedMessage();
        emit exit();
    } catch (QException &e) { // TODO : handle exceptions here
		CmdLineInterface::qStdOut() << e.what();
        emit exit();
    }
    return done;
}

void CmdLineInterface::printCreateVolume(QSharedPointer<GostCrypt::NewCore::CreateVolumeResponse> r)
{
    qStdOut() << "Volume Created." << endl;
    (void)r;
    emit exit();
}

void CmdLineInterface::printMountVolume(QSharedPointer<GostCrypt::NewCore::MountVolumeResponse> r)
{
    qStdOut() << "Volume Mounted." << endl;
    (void)r;
    emit exit();
}

void CmdLineInterface::printDismountVolume(QSharedPointer<GostCrypt::NewCore::DismountVolumeResponse> r)
{
    qStdOut() << "Volume Dismounted." << endl;
    (void)r;
    emit exit();
}

void CmdLineInterface::printGetMountedVolumes(QSharedPointer<GostCrypt::NewCore::GetMountedVolumesResponse> r)
{
    if(!r)
        qStdOut() << "Invalid response received." << endl;
    for(auto v = r->volumeInfoList.begin(); v < r->volumeInfoList.end(); ++v){
        qStdOut() << QString::fromStdString(string((*v)->Path)) << "\t";
        qStdOut() << QString::fromStdString(string((*v)->MountPoint)) << "\t";
        qStdOut() << (*v)->Size << "\t";
        qStdOut() << QString::fromStdWString((*v)->EncryptionAlgorithmName) << endl;
    }
    emit exit();
}
