#include "CmdLineInterface.h"
#include "NewCore/CoreException.h"
#include "NewFuseService/FuseException.h"
#include "NewCore/CoreResponse.h"

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
    CONNECT_SIGNAL(CreateVolume);
    CONNECT_SIGNAL(MountVolume);
    CONNECT_SIGNAL(DismountVolume);
    CONNECT_SIGNAL(GetMountedVolumes);
    CONNECT_SIGNAL(GetEncryptionAlgorithms);
    CONNECT_SIGNAL(GetDerivationFunctions);
    CONNECT_SIGNAL(GetHostDevices);
    CONNECT_SIGNAL(CreateKeyFile);
    CONNECT_SIGNAL(ChangeVolumePassword);
    CONNECT_SIGNAL(ProgressUpdate);

    /*app.connect(core.data(), SIGNAL(sendCreateVolume(QSharedPointer<GostCrypt::NewCore::CreateVolumeResponse>)), this, SLOT(printCreateVolume(QSharedPointer<GostCrypt::NewCore::CreateVolumeResponse>)));*/

    /* Connecting request from here to request switch from Core */
    app.connect(this, SIGNAL(request(QVariant)), core.data(), SLOT(request(QVariant)));

    /* Connecting few exit signals to close the program apropriately */
    app.connect(this, SIGNAL(exit()), core.data(), SLOT(exit()));
    app.connect(core.data(), SIGNAL(exited()), &app, SLOT(quit()));

    /* Connecting the signals to get the sudo request from Core and send it to Core */
    app.connect(core.data(), SIGNAL(askSudoPassword()), this, SLOT(askSudoPassword()));
    app.connect(this, SIGNAL(sendSudoPassword(QString)), core.data(), SLOT(receiveSudoPassword(QString)));

	app.connect(&app, SIGNAL(askExit()), this, SIGNAL(exit()));

    /* Processing the commands passed */
    try {
        processRequest();
    } catch(Parser::ParseException &e) {
        qDebug().noquote() << e.getMessage();
        parser.showHelp();
        return -1;
    } catch(GostCrypt::NewCore::GostCryptException &e) {
        qDebug().noquote() << e.displayedMessage();
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

    if(args.length() == 0) { // nothing to do. Probably launched with --help
        parser.setApplicationDescription("This is the command line interface for the GostCrypt library.\n\
\n\
Commands:\n\
  mount         \tMounts a volume.\n\
  create        \tCreates a volume.\n\
  umount        \tUnmounts a volume.\n\
  test          \tTest the algrithms.\n\
  dismountall   \tUnmounts all mounted volumes.\n\
  automount     \tTries to auto-mounts the connected drives.\n\
  backupheaders \tExports the header of a volume to make a backup.\n\
  createkeyfiles\tCreates a random file that can be used as a key.\n\
  list          \tLists the volumes, derivation functions of algorithms that can be used.\n\
");
        parser.showHelp();
    }

    /*if(args.length() == 0)
        return;*/

    parser.clearPositionalArguments();

    uint32 value = FirstCMD::Str.indexOf(QRegExp(command, Qt::CaseInsensitive)); // using the mix between a QString tab and a enumeration.

    switch(value){
        case FirstCMD::mount: //"mount":
            {
                QSharedPointer <GostCrypt::NewCore::MountVolumeRequest> options(new GostCrypt::NewCore::MountVolumeRequest);
                Parser::parseMount(parser, options);
                emit request(QVariant::fromValue(options));
            }
            break;
        case FirstCMD::createvolume://"create-volume":
        case FirstCMD::create://"create":
            {
                QSharedPointer <GostCrypt::NewCore::CreateVolumeRequest> options(new GostCrypt::NewCore::CreateVolumeRequest);
                Parser::parseCreate(parser, options);
                emit request(QVariant::fromValue(options));
            }
            break;
        case FirstCMD::umount://"umount":
        case FirstCMD::unmount://"unmount":
        case FirstCMD::dismount://"dismount":
            {
                QSharedPointer<GostCrypt::NewCore::DismountVolumeRequest> options(new GostCrypt::NewCore::DismountVolumeRequest);
                Parser::parseDismount(parser, options);
                emit request(QVariant::fromValue(options));
            }
            break;
        case FirstCMD::dismountall://"dismountall":
            {
                QSharedPointer<GostCrypt::NewCore::DismountVolumeRequest> options(new GostCrypt::NewCore::DismountVolumeRequest);
                emit request(QVariant::fromValue(options)); // dismount-all is just a dismount with no params.
            }
            break;
        case FirstCMD::createkeyfiles://"createkeyfiles":
            {
                QStringList files;
                Parser::parseCreateKeyFiles(parser, files); // TODO multiple keyfiles not supported yet
                QSharedPointer <GostCrypt::NewCore::CreateKeyFileRequest> options(new GostCrypt::NewCore::CreateKeyFileRequest());
                for(QStringList::Iterator file = files.begin(); file != files.end(); file++) {
                    options->file.reset(new QFileInfo(*file));
                    emit request(QVariant::fromValue(options));
                }
            }
            break;
        case FirstCMD::list://"list":
            {
                Parser::WhatToList item;
                Parser::parseList(parser, &item);
                switch(item){
                    case Parser::Volumes:
                        {
                            QSharedPointer<GostCrypt::NewCore::GetMountedVolumesRequest> options(new GostCrypt::NewCore::GetMountedVolumesRequest);
                            emit request(QVariant::fromValue(options));
                        }
                        break;
                    case Parser::Algorithms:
                        {
                            QSharedPointer<GostCrypt::NewCore::GetEncryptionAlgorithmsRequest> options(new GostCrypt::NewCore::GetEncryptionAlgorithmsRequest);
                            emit request(QVariant::fromValue(options));
                        }
                        break;
                    case Parser::Hashs:
                        {
                            QSharedPointer<GostCrypt::NewCore::GetDerivationFunctionsRequest> options(new GostCrypt::NewCore::GetDerivationFunctionsRequest);
                            emit request(QVariant::fromValue(options));
                        }
                        break;
                    case Parser::Devices:
                        {
                            QSharedPointer<GostCrypt::NewCore::GetHostDevicesRequest> options(new GostCrypt::NewCore::GetHostDevicesRequest);
                            emit request(QVariant::fromValue(options));
                        }
                }
            }
            break;
        /*case FirstCMD::test://"test":
            qStdOut() << "Option not supported." << endl; // TODO
            break;
        case FirstCMD::automount://"automount":
            qStdOut() << "Option not supported." << endl; // TODO
            break;
        case FirstCMD::backupheaders://"backupheaders":
            qStdOut() << "Option not supported." << endl; // TODO
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
    emit sendSudoPassword(QString(*password));
}

bool MyApplication::notify(QObject *receiver, QEvent *event)
{
    bool done = true;
    try {
        done = QCoreApplication::notify(receiver, event);
    } catch(GostCrypt::NewCore::GostCryptException &e) {
        CmdLineInterface::qStdOut() << e.displayedMessage();
        emit askExit();
    } catch (QException &e) { // TODO : handle exceptions here
        CmdLineInterface::qStdOut() << e.what();
        emit askExit();
    }
    return done;
}

void CmdLineInterface::printProgressUpdate(QSharedPointer<GostCrypt::NewCore::ProgressUpdateResponse> r) {
    qStdOut() << "\r" << r->progress*100 << "%";
    qStdOut().flush();
}

void CmdLineInterface::printCreateVolume(QSharedPointer<GostCrypt::NewCore::CreateVolumeResponse> r)
{
    qStdOut() << "\rVolume Created." << endl;
    (void)r;
    emit exit();
}

void CmdLineInterface::printMountVolume(QSharedPointer<GostCrypt::NewCore::MountVolumeResponse> r)
{
    qStdOut() << "\rVolume Mounted." << endl;
    (void)r;
    emit exit();
}

void CmdLineInterface::printDismountVolume(QSharedPointer<GostCrypt::NewCore::DismountVolumeResponse> r)
{
    qStdOut() << "\rVolume Dismounted." << endl;
    (void)r;
    emit exit();
}

void CmdLineInterface::printGetMountedVolumes(QSharedPointer<GostCrypt::NewCore::GetMountedVolumesResponse> r)
{
    if(!r)
        throw MissingParamException("response");
    qStdOut() << "\r";
    for(QSharedPointer<GostCrypt::NewCore::VolumeInformation> v : r->volumeInfoList){
        qStdOut() << v->volumePath->absoluteFilePath() << "\t";
        qStdOut() << ((v->mountPoint.isNull()) ? QString("-") : v->mountPoint->absoluteFilePath()) << "\t";
        qStdOut() << v->size << "\t";
        qStdOut() << v->encryptionAlgorithmName << endl;
    }
    emit exit();
}

void CmdLineInterface::printGetEncryptionAlgorithms(QSharedPointer<GostCrypt::NewCore::GetEncryptionAlgorithmsResponse> r)
{
    qStdOut() << "\r";
    for(QString algo : r->algorithms) {
        qStdOut() << algo << endl;
    }
    emit exit();
}

void CmdLineInterface::printGetDerivationFunctions(QSharedPointer<GostCrypt::NewCore::GetDerivationFunctionsResponse> r)
{
    qStdOut() << "\r";
    for(QString algo : r->algorithms) {
        qStdOut() << algo << endl;
    }
    emit exit();
}

void CmdLineInterface::printGetHostDevices(QSharedPointer<GostCrypt::NewCore::GetHostDevicesResponse> r)
{
    qStdOut() << "\r";
    for(QSharedPointer<GostCrypt::NewCore::HostDevice> d : r->hostDevices) {
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
    emit exit();
}

void CmdLineInterface::printCreateKeyFile(QSharedPointer<GostCrypt::NewCore::CreateKeyFileResponse> r)
{
    qStdOut() << "\rKeyFile Created." << endl;
    (void)r;
    emit exit(); // TODO only exit when all keyfiles are created
}

void CmdLineInterface::printChangeVolumePassword(QSharedPointer<GostCrypt::NewCore::ChangeVolumePasswordResponse> r)
{
    qStdOut() << "\rPassword successfully changed." << endl;
    (void)r;
    emit exit();
}
