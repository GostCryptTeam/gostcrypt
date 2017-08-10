#include "cmdUserInterface.h"
#include "Core/CoreException.h"

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

    QSharedPointer<GostCrypt::NewCore::CoreBase> Core = GostCrypt::NewCore::getCore(&app);

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

    uint32 value = FirstCMD::Str.indexOf(QRegExp(command, Qt::CaseInsensitive)); // using the mix between a QString tab and a enumeration.

    switch(value){
        case FirstCMD::mount: //"mount":
            {
                QSharedPointer <GostCrypt::NewCore::MountVolumeParams> options;
                options.reset(new GostCrypt::NewCore::MountVolumeParams());
                try {
                    Parser::parseMount(app, parser, options);
                    QSharedPointer<GostCrypt::NewCore::MountVolumeResponse> response;
                    response = Core->mountVolume(options);
                    qStdOut() << "Volume Mounted\n";
                } catch(Parser::ParseException &e) {
                    qStdOut() << e.getMessage() << endl;
                    parser.showHelp();
                } catch(GostCrypt::NewCore::CoreException &e) {
                    qStdOut() << e.qwhat();
                } catch(...) {
                    qStdOut() << "Unknown exception raised \n";
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
                    QSharedPointer<GostCrypt::NewCore::CreateVolumeResponse> response;
                    response = Core->createVolume(options);
                    qStdOut() << "Volume Created\n";
                } catch(Parser::ParseException &e){
                    qStdOut() << e.getMessage() << endl;
                    parser.showHelp();
                } catch(GostCrypt::NewCore::CoreException &e){
                    qStdOut() << e.qwhat();
                } catch(GostCrypt::Exception &e){
                    qStdOut() << e.what();
                } catch(...) {
                    qStdOut() << "Unknown exception raised.";
                }
            }
            break;
        case FirstCMD::umount://"umount":
        case FirstCMD::unmount://"unmount":
        case FirstCMD::dismount://"dismount":
            {
                QSharedPointer<GostCrypt::NewCore::DismountVolumeParams> params(new GostCrypt::NewCore::DismountVolumeParams);
                try {
                    Parser::parseDismount(app, parser, params);
                    Core->dismountVolume(params);
                    qStdOut() << "Volume unmounted\n";
                } catch(Parser::ParseException &e){
                    qStdOut() << e.getMessage() << endl;
                    parser.showHelp();
                } catch(GostCrypt::NewCore::CoreException &e) {
                    qStdOut() << e.qwhat();
                } catch(...) {
                    qStdOut() << "Unknown exception raised.";
                }
            }
            break;
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
                                    qStdOut() << d->devicePath->absoluteFilePath() << "\t" << d->mountPoint->absoluteFilePath() << "\t" << d->size << endl;
									for(QSharedPointer<GostCrypt::NewCore::HostDevice> p : d->partitions) {
                                        qStdOut() << "\t" << p->devicePath->absoluteFilePath() << "\t" << p->mountPoint->absoluteFilePath() << "\t" << p->size << endl;
									}
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
