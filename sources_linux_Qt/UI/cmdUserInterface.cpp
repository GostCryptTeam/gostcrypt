#include "cmdUserInterface.h"

const QStringList FirstCMD::Str = MK_ALL_COMMANDS(MK_STRTAB);

void AdminPasswordCLIRequestHandler::operator() (string &passwordStr) {
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // hide input

    std::cout << "Please enter your sudo password: " << std::endl;
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
                    std::cout << "Volume Mounted"; // maybe add some more display
                } /*catch (GostCrypt::PasswordIncorrect &e) { // TODO make this work
                    std::cout << "Wrong password or bad GostCrypt volume." << std::endl;
                } catch (GostCrypt::VolumeAlreadyMounted &e) {
                    std::cout << "Volume has already been mounted." << std::endl;
                }*/ catch(Parser::ParseException &e){
                    std::cout << qPrintable(e.getMessage()) << std::endl;
                    parser.showHelp();
                } catch(...) {
                    std::cout << "Unknown exception raised.";
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
                    std::cout << qPrintable(e.getMessage()) << std::endl;
                    parser.showHelp();
                } catch(...) {
                    std::cout << "Unknown exception raised.";
                }
            }
            break;
        case FirstCMD::umount://"umount":
        case FirstCMD::unmount://"unmount":
        case FirstCMD::dismount://"dismount":
            std::cout << "Option not supported." << std::endl; // TODO
        case FirstCMD::test://"test":
            std::cout << "Option not supported." << std::endl; // TODO
            break;
        case FirstCMD::dismountall://"dismount-all":
            std::cout << "Option not supported." << std::endl; // TODO
            break;
        case FirstCMD::automount://"auto-mount":
            std::cout << "Option not supported." << std::endl; // TODO
            break;
        case FirstCMD::backupheaders://"backup-headers":
            std::cout << "Option not supported." << std::endl; // TODO
            break;
        case FirstCMD::createkeyfiles://"create-keyfiles":
            std::cout << "Option not supported." << std::endl; // TODO
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
                                    std::cout << "----------------------------------------------------" << std::endl;
                                    std::cout << "Name: " << string((*v)->Path)                         << std::endl;
                                    std::cout << "Mountpoint: " << string((*v)->MountPoint)             << std::endl;
                                    std::cout << "Size: " << (*v)->Size                                 << std::endl;
                                    //std::cout << "Algorithm: " << string((*v)->EncryptionAlgorithmName) << std::endl;
                                }
                            }
                            break;
                        case Parser::Algorithms:
                            std::cout << "Option not supported." << std::endl; // TODO
                            break;
                        case Parser::Hashs:
                            std::cout << "Option not supported." << std::endl; // TODO
                            break;
                        case Parser::FileSystems:
                            // all are from enum FileSystemType of GostCrypt::NewCore::FileSystemType
                            for(QString fs : GostCrypt::NewCore::FilesystemType::Str)
                                std::cout << qPrintable(fs) << " ";
                            std::cout << std::endl;
                    }

                } catch(Parser::ParseException &e){
                    std::cout << qPrintable(e.getMessage()) << std::endl;
                    parser.showHelp();
                }
            }
            break;
        default:
            std::cout << "Not a valid command. Try --help for help." << std::endl;
    }

    return 0;
}
