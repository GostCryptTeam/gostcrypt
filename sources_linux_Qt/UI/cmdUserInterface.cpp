#include "cmdUserInterface.h"

const QStringList FirstCMD::Str = MK_ALL_COMMANDS(MK_STRTAB);

QTextStream &qStdOut() {
    static QTextStream ts( stdout );
    return ts;
}

void AdminPasswordCLIRequestHandler::operator() (string &passwordStr) {
	termios oldt;
	tcgetattr(STDIN_FILENO, &oldt);
	termios newt = oldt;
	newt.c_lflag &= ~ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &newt); // hide input

    qStdOut() << "Please enter your sudo password: " << endl;
    getline(cin, passwordStr);

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // reset back the termineal
}

int handleCLI(int argc, char ** argv){
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;

    QSharedPointer<GostCrypt::NewCore::CoreBase> Core = GostCrypt::NewCore::getCore();

    QCoreApplication::setApplicationName("GostCrypt");
    parser.setApplicationDescription("This is the command line interface for the GostCrypt library.");
    parser.addHelpOption();
    parser.addPositionalArgument("command", "The command to execute.","{mount|create|umount|test|dismountall|automount|backupheaders|createkeyfiles|list}");

    // Call parse() to find out the positional arguments.
    parser.parse(QCoreApplication::arguments());

    const QStringList args = parser.positionalArguments();
    const QString command = args.isEmpty() ? QString() : args.first();

    if(args.length() == 0 && parser.isSet("help"))
        parser.showHelp();

    if(args.length() == 0) // nothing to do. Probably launched with --help
        return 0;

    parser.clearPositionalArguments();

    uint32 value = FirstCMD::Str.indexOf(QRegExp(command, Qt::CaseInsensitive));

    switch(value){
        case FirstCMD::mount: //"mount":
            {
                QSharedPointer <GostCrypt::NewCore::MountVolumeParams> options;
                options.reset(new GostCrypt::NewCore::MountVolumeParams());
                try {
                    Parser::parseMount(app, parser, options);
                    QSharedPointer<GostCrypt::NewCore::MountVolumeResponse> response;
                    response = Core->mountVolume(options);
                    qStdOut() << "Volume Mounted"; // maybe add some more display
                } /*catch (GostCrypt::PasswordIncorrect &e) { // TODO make this work
                    qStdOut() << "Wrong password or bad GostCrypt volume." << endl;
                } catch (GostCrypt::VolumeAlreadyMounted &e) {
                    qStdOut() << "Volume has already been mounted." << endl;
                }*/ catch(Parser::ParseException &e){
                    qStdOut() << e.getMessage() << endl;
                    parser.showHelp();
                } catch(...) {
                    qStdOut() << "Unknown exception raised.";
                }
            }
            break;
        case FirstCMD::createvolume://"create-volume":
        case FirstCMD::create://"create":
            {
                QSharedPointer <GostCrypt::NewCore::CreateVolumeParams> options;
                options.reset(new GostCrypt::NewCore::CreateVolumeParams());
                try {
                    Parser::parseCreate(app, parser, options);
                    //QSharedPointer<GostCrypt::NewCore::CreateVolumeResponse> response;
                    // TODO : call Core !
                } catch(Parser::ParseException &e){
                    qStdOut() << e.getMessage() << endl;
                    parser.showHelp();
                } catch(...) {
                    qStdOut() << "Unknown exception raised.";
                }
            }
            break;
        case FirstCMD::umount://"umount":
        case FirstCMD::unmount://"unmount":
        case FirstCMD::dismount://"dismount":
            qStdOut() << "Option not supported." << endl; // TODO
        case FirstCMD::test://"test":
            qStdOut() << "Option not supported." << endl; // TODO
            break;
        case FirstCMD::dismountall://"dismount-all":
            qStdOut() << "Option not supported." << endl; // TODO
            break;
        case FirstCMD::automount://"auto-mount":
            qStdOut() << "Option not supported." << endl; // TODO
            break;
        case FirstCMD::backupheaders://"backup-headers":
            qStdOut() << "Option not supported." << endl; // TODO
            break;
        case FirstCMD::createkeyfiles://"create-keyfiles":
            qStdOut() << "Option not supported." << endl; // TODO
            break;
        case FirstCMD::list://"list":
            {
                Parser::WhatToList item;
                try {
                    Parser::parseList(app,parser,&item);
                    switch(item){
                        case Parser::Volumes:
                            {
                                QSharedPointer <GostCrypt::NewCore::GetMountedVolumesParams> params;
                                QSharedPointer <GostCrypt::NewCore::GetMountedVolumesResponse> response;
                                params.reset(new GostCrypt::NewCore::GetMountedVolumesParams());
                                response = Core->getMountedVolumes(params);
                                //qStdOut() << "";
                                for(auto v = response->volumeInfoList.begin(); v < response->volumeInfoList.end(); ++v){
                                    qStdOut() << "----------------------------------------------------------------" << endl; // TODO : upgrade display
                                    qStdOut() << "Name: " << QString::fromStdString(string((*v)->Path))             << endl;
                                    qStdOut() << "Mountpoint: " << QString::fromStdString(string((*v)->MountPoint)) << endl;
                                    qStdOut() << "Size: " << (*v)->Size                                             << endl;
                                    //qStdOut() << "Algorithm: " << string((*v)->EncryptionAlgorithmName) << endl;
                                }
                            }
                            break;
                        case Parser::Algorithms:
                            qStdOut() << "Option not supported." << endl; // TODO
                            break;
                        case Parser::Hashs:
                            qStdOut() << "Option not supported." << endl; // TODO
                            break;
                        case Parser::FileSystems:
                            // TODO call core for what is compatible on the system
                            qStdOut() << "Option not supported." << endl; // TODO
                            break;
                        case Parser::Devices:
							QSharedPointer<GostCrypt::NewCore::GetHostDevicesResponse> response;
							response = Core->getHostDevices();
							for(QSharedPointer<GostCrypt::NewCore::HostDevice> d : response->hostDevices) {
								qStdOut() << d->devicePath.canonicalFilePath() << "\t" << d->mountPoint.canonicalFilePath() << "\t" << d->size << endl;
								for(QSharedPointer<GostCrypt::NewCore::HostDevice> p : d->partitions) {
									qStdOut()<< "\t" << p->devicePath.canonicalFilePath() << "\t" << p->mountPoint.canonicalFilePath() << "\t" << p->size << endl;
								}
							}
                    }

                } catch(Parser::ParseException &e){
                    qStdOut() << e.getMessage() << endl;
                    parser.showHelp();
                }
            }
            break;
        default:
            qStdOut() << "Not a valid command. Try --help for help." << endl;
    }

    return 0;
}
