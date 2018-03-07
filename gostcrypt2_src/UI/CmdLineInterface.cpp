#include "CmdLineInterface.h"
#include "Core/CoreException.h"
#include "FuseService/FuseException.h"
#include "Core/CoreResponse.h"

const QStringList CmdLineInterface::FirstCMD::Str = MK_ALL_COMMANDS(MK_STRTAB);

CmdLineInterface::CmdLineInterface(QObject* parent) : UserInterface(parent)
{

}

QTextStream& CmdLineInterface::qStdOut()
{
    static QTextStream ts(stdout);
    return ts;
}

int CmdLineInterface::start(int argc, char** argv)
{
    MyApplication app(argc, argv);
    core = GostCrypt::Core::getCore();

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
    CONNECT_SIGNAL(BenchmarkAlgorithms);

    app.connect(core.data(), SIGNAL(sendProgressUpdate(quint32,qreal)), this, SLOT(printProgressUpdate(quint32,qreal)));

    /*app.connect(core.data(), SIGNAL(sendCreateVolume(QSharedPointer<GostCrypt::Core::CreateVolumeResponse>)), this, SLOT(printCreateVolume(QSharedPointer<GostCrypt::Core::CreateVolumeResponse>)));*/

    /* Connecting request from here to request switch from Core */
    app.connect(this, SIGNAL(request(QVariant)), core.data(), SLOT(request(QVariant)));

    /* Connecting few exit signals to close the program apropriately */
    app.connect(this, SIGNAL(exit()), core.data(), SLOT(exit()));
    app.connect(core.data(), SIGNAL(exited()), &app, SLOT(quit()));

    /* Connecting the signals to get the sudo request from Core and send it to Core */
    app.connect(core.data(), SIGNAL(askSudoPassword()), this, SLOT(askSudoPassword()));
    app.connect(this, SIGNAL(sendSudoPassword(QString)), core.data(),
                SLOT(receiveSudoPassword(QString)));

    app.connect(&app, SIGNAL(askExit()), this, SIGNAL(exit()));

    /* Processing the commands passed */
    try
    {
        processRequest();
    }
    catch (Parser::ParseException& e)
    {
        qWarning().noquote()  << e.getMessage();
        parser.showHelp();
        return -1;
    }
    catch (GostCrypt::GostCryptException& e)
    {
        qWarning().noquote() << e.displayedMessage();
        return -1;
    }
    catch (QException& e)     // TODO : handle exceptions here
    {
        qWarning().noquote() << e.what();
        return -1;
    }

    return app.exec();
}

void CmdLineInterface::processRequest()
{
    parser.setApplicationDescription("This is the command line interface for the GostCrypt library.");
    parser.addHelpOption();
    parser.addPositionalArgument("command", "The command to execute.",
                                 "{mount|create|umount|test|dismountall|automount|backupheaders|createkeyfiles|list}");

    parser.parse(QCoreApplication::arguments());

    const QStringList args = parser.positionalArguments();
    const QString command = args.isEmpty() ? QString() : args.first();

    if (args.length() == 0)  // nothing to do. Probably launched with --help
    {
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
  benchmark     \tTest the speed of the differents Ciphers. \n\
");
        parser.showHelp();
    }

    /*if(args.length() == 0)
        return;*/

    parser.clearPositionalArguments();

    quint32 value = FirstCMD::Str.indexOf(QRegExp(command,
                                          Qt::CaseInsensitive)); // using the mix between a QString tab and a enumeration.

    switch (value)
    {
        case FirstCMD::mount: //"mount":
        {
            QSharedPointer <GostCrypt::Core::MountVolumeRequest> options(new
                    GostCrypt::Core::MountVolumeRequest);
            Parser::parseMount(parser, options);
            emit request(QVariant::fromValue(options));
        }
        break;
        case FirstCMD::createvolume://"create-volume":
        case FirstCMD::create://"create":
        {
            QSharedPointer <GostCrypt::Core::CreateVolumeRequest> options(new
                    GostCrypt::Core::CreateVolumeRequest);
            Parser::parseCreate(parser, options);
            emit request(QVariant::fromValue(options));
        }
        break;
        case FirstCMD::umount://"umount":
        case FirstCMD::unmount://"unmount":
        case FirstCMD::dismount://"dismount":
        {
            QSharedPointer<GostCrypt::Core::DismountVolumeRequest> options(new
                    GostCrypt::Core::DismountVolumeRequest);
            Parser::parseDismount(parser, options);
            emit request(QVariant::fromValue(options));
        }
        break;
        case FirstCMD::dismountall://"dismountall":
        {
            QSharedPointer<GostCrypt::Core::DismountVolumeRequest> options(new
                    GostCrypt::Core::DismountVolumeRequest);
            options->all = true;
            emit request(QVariant::fromValue(options)); // dismount-all is just a dismount with no params.
        }
        break;
        case FirstCMD::createkeyfiles://"createkeyfiles":
        {
            QStringList files;
            Parser::parseCreateKeyFiles(parser, files); // TODO multiple keyfiles not supported yet
            QSharedPointer <GostCrypt::Core::CreateKeyFileRequest> options(new
                    GostCrypt::Core::CreateKeyFileRequest());
            for (QStringList::Iterator file = files.begin(); file != files.end(); file++)
            {
                options->file.setFile(*file);
                emit request(QVariant::fromValue(options));
            }
        }
        break;
        case FirstCMD::list://"list":
        {
            Parser::WhatToList item;
            Parser::parseList(parser, &item);
            switch (item)
            {
                case Parser::Volumes:
                {
                    QSharedPointer<GostCrypt::Core::GetMountedVolumesRequest> options(new
                            GostCrypt::Core::GetMountedVolumesRequest);
                    emit request(QVariant::fromValue(options));
                }
                break;
                case Parser::Algorithms:
                {
                    QSharedPointer<GostCrypt::Core::GetEncryptionAlgorithmsRequest> options(
                        new GostCrypt::Core::GetEncryptionAlgorithmsRequest);
                    emit request(QVariant::fromValue(options));
                }
                break;
                case Parser::Hashs:
                {
                    QSharedPointer<GostCrypt::Core::GetDerivationFunctionsRequest> options(
                        new GostCrypt::Core::GetDerivationFunctionsRequest);
                    emit request(QVariant::fromValue(options));
                }
                break;
                case Parser::Devices:
                {
                    QSharedPointer<GostCrypt::Core::GetHostDevicesRequest> options(new
                            GostCrypt::Core::GetHostDevicesRequest);
                    emit request(QVariant::fromValue(options));
                }
            }
        }
        break;
        case FirstCMD::benchmark:
        {
            QSharedPointer<GostCrypt::Core::BenchmarkAlgorithmsRequest> options(new
                    GostCrypt::Core::BenchmarkAlgorithmsRequest);
            Parser::parseBenchmark(parser, options);
            emit request(QVariant::fromValue(options));
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

bool MyApplication::notify(QObject* receiver, QEvent* event)
{
    bool done = true;
    try
    {
        done = QCoreApplication::notify(receiver, event);
    }
    catch (GostCrypt::GostCryptException& e)
    {
        qWarning().noquote() << e.displayedMessage();
        emit askExit();
    }
    catch (QException& e)     // TODO : handle exceptions here
    {
        qWarning().noquote() << e.what();
        emit askExit();
    }
    return done;
}

QString CmdLineInterface::formatSize(quint64 sizeInByte)
{
    return  UserInterface::formatSize(sizeInByte, false);
}

void CmdLineInterface::printProgressUpdate(quint32 requestId, qreal progress)
{
    Q_UNUSED(requestId);
    qStdOut() << "\r" << progress * 100 << "%";
    qStdOut().flush();
}

void CmdLineInterface::printCreateVolume(QSharedPointer<GostCrypt::Core::CreateVolumeResponse> r)
{
    qStdOut() << "\rVolume Created." << endl;
    (void)r;
    emit exit();
}

void CmdLineInterface::printMountVolume(QSharedPointer<GostCrypt::Core::MountVolumeResponse> r)
{
    qStdOut() << "\rVolume Mounted." << endl;
    (void)r;
    emit exit();
}

void CmdLineInterface::printDismountVolume(QSharedPointer<GostCrypt::Core::DismountVolumeResponse>
        r)
{
    qStdOut() << "\rVolume Dismounted." << endl;
    (void)r;
    emit exit();
}

void CmdLineInterface::printGetMountedVolumes(
    QSharedPointer<GostCrypt::Core::GetMountedVolumesResponse> r)
{
    qStdOut() << "\r";
    for (QSharedPointer<GostCrypt::Volume::VolumeInformation> v : r->volumeInfoList)
    {
        qStdOut() << v->volumePath.absoluteFilePath() << "\t";
        qStdOut() << ((v->mountPoint.absoluteFilePath().isEmpty()) ? QString("-") :
                      v->mountPoint.absoluteFilePath()) << "\t";
        qStdOut() << v->size << "\t";
        qStdOut() << v->encryptionAlgorithmName << endl;
    }
    emit exit();
}

void CmdLineInterface::printGetEncryptionAlgorithms(
    QSharedPointer<GostCrypt::Core::GetEncryptionAlgorithmsResponse> r)
{
    qStdOut() << "\r";
    for (QString algo : r->algorithms)
    {
        qStdOut() << algo << endl;
    }
    emit exit();
}

void CmdLineInterface::printGetDerivationFunctions(
    QSharedPointer<GostCrypt::Core::GetDerivationFunctionsResponse> r)
{
    qStdOut() << "\r";
    for (QString algo : r->algorithms)
    {
        qStdOut() << algo << endl;
    }
    emit exit();
}

void CmdLineInterface::printGetHostDevices(QSharedPointer<GostCrypt::Core::GetHostDevicesResponse>
        r)
{
    qStdOut() << "\r";
    for (QSharedPointer<GostCrypt::Core::HostDevice> d : r->hostDevices)
    {
        qStdOut() << d->devicePath.absoluteFilePath() << "\t";
        if (!d->mountPoint.absoluteFilePath().isEmpty())
        {
            qStdOut() << d->mountPoint.absoluteFilePath() << "\t";
        }
        else
        {
            qStdOut() << "no mountpoint\t";
        }
        qStdOut() << d->size << endl;
        for (QSharedPointer<GostCrypt::Core::HostDevice> p : d->partitions)
        {
            qStdOut() << "\t" << p->devicePath.absoluteFilePath() << "\t";
            if (!p->mountPoint.absoluteFilePath().isEmpty())
            {
                qStdOut() << p->mountPoint.absoluteFilePath() << "\t";
            }
            else
            {
                qStdOut() << "no mountpoint\t";
            }
            qStdOut() << p->size << endl;
        }
    }
    emit exit();
}

void CmdLineInterface::printCreateKeyFile(QSharedPointer<GostCrypt::Core::CreateKeyFileResponse> r)
{
    qStdOut() << "\rKeyFile Created." << endl;
    (void)r;
    emit exit(); // TODO only exit when all keyfiles are created
}

void CmdLineInterface::printChangeVolumePassword(
    QSharedPointer<GostCrypt::Core::ChangeVolumePasswordResponse> r)
{
    qStdOut() << "\rPassword successfully changed." << endl;
    (void)r;
    emit exit();
}

void CmdLineInterface::printBenchmarkAlgorithms(
    QSharedPointer<GostCrypt::Core::BenchmarkAlgorithmsResponse> r)
{
    QStringList categories = { " Algorithm         ", " Encryption Speed ", " Decryption Speed ", " Mean Speed "};
    qStdOut() << "+";
    for (const auto& i : categories)
    {
        for (int j = 0; j < i.length(); ++j)
        {
            qStdOut() << "-";
        }
        qStdOut() << "+";
    }
    qStdOut() << "\n";
    qStdOut() << "|";
    for (const auto& i : categories)
    {
        qStdOut() << i << "|";
    }
    qStdOut() << "\n+";
    for (const auto& i : categories)
    {
        for (int j = 0; j < i.length(); ++j)
        {
            qStdOut() << "-";
        }
        qStdOut() << "+";
    }
    qStdOut() << "\n";
    //Print each algorithm result
    for (int i = 0; i < r->algorithmsNames.size(); ++i)
    {
        //Align center
        qStdOut() << "| ";
        qStdOut() << r->algorithmsNames.at(i);
        int offset = categories.at(0).length() - r->algorithmsNames.at(i).length();
        for (int j = 0; j < offset - 1; ++j) { qStdOut() << " "; }
        qStdOut() << "|";

        QString encSize = formatSize(r->encryptionSpeed.at(i));
        qStdOut() << " ";
        qStdOut() << encSize << "/s";
        offset = categories.at(1).length() - encSize.length() - 2;
        for (int j = 0; j < offset - 1; ++j) { qStdOut() << " "; }
        qStdOut() << "|";

        QString decSize = formatSize(r->decryptionSpeed.at(i));
        qStdOut() << " ";
        qStdOut() << decSize << "/s";
        offset = categories.at(2).length() - decSize.length() - 2;
        for (int j = 0; j < offset - 1; ++j) { qStdOut() << " "; }
        qStdOut() << "|";

        QString meanSize = formatSize(r->meanSpeed.at(i));
        qStdOut() << " ";
        qStdOut() << meanSize << "/s";
        offset = categories.at(3).length() - meanSize.length() - 2;
        for (int j = 0; j < offset - 1; ++j) { qStdOut() << " "; }
        qStdOut() << "|\n+";

        for (const auto& i : categories)
        {
            for (int j = 0; j < i.length(); ++j)
            {
                qStdOut() << "-";
            }
            qStdOut() << "+";
        }
        qStdOut() << "\n";
    }

    emit exit();
}
