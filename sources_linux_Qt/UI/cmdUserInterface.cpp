#include "cmdUserInterface.h"

void AdminPasswordCLIRequestHandler::operator() (string &passwordStr) {
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // hide input

    cout << "Please enter your sudo password: " << std::end;
    getline(cin, passwordStr);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // reset back the termineal
}

int handleCLI(int argc, char ** argv){
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;

    GostCrypt::CoreService::Start();
    // Start encryption thread pool
    GostCrypt::EncryptionThreadPool::Start();

    GostCrypt::Core->Init();
    GostCrypt::Core->SetApplicationExecutablePath (QCoreApplication::applicationFilePath().toStdWString());
    GostCrypt::Core->SetAdminPasswordCallback (shared_ptr<GostCrypt::GetStringFunctor>(new AdminPasswordCLIRequestHandler()));

    QCoreApplication::setApplicationName("GostCrypt");
    parser.setApplicationDescription("This is the command line interface for the GostCrypt library.");
    parser.addHelpOption();
    parser.addPositionalArgument("command", "The command to execute.","{mount|create|umount|test|dismountall|automount|backupheaders|createkeyfiles|list}");

    // Call parse() to find out the positional arguments.
    parser.parse(QCoreApplication::arguments());

    const QStringList args = parser.positionalArguments();
    const QString command = args.isEmpty() ? QString() : args.first();

    parser.clearPositionalArguments();

    value = FirstCMD::Str.indexOf(QRegExp(command, Qt::CaseInsensitive));

    switch(value){
        case FirstCMD::mount: //"mount":
            {
                GostCrypt::NewCore::MountVolumeParams options;
                try {
                    Parser::parseMount(app, parser, &options);

                } catch(Parser::ParseException &e){
                    std::cout << qPrintable(e.getMessage()) << std::endl;
                    parser.showHelp();
                } catch(...) {
                    std::cout << "Unknown exception raised.";
                }
            }


            if(isOk(parseMount(app, parser, &mountoptions, &error), parser, error)){
                try {
                    try {
                        shared_ptr <GostCrypt::VolumeInfo> volumeData = GostCrypt::Core->MountVolume (mountoptions);
                        std::cout << "Volume mounted." << std::endl;
                        std::cout << string(volumeData->Path) << " "
                                  << QString::fromWCharArray(volumeData->EncryptionAlgorithmName.c_str()).toStdString() << " "
                                  << string(volumeData->MountPoint) << " "
                                  << convertSize(volumeData->Size).toStdString() << std::endl;
                    } catch (GostCrypt::PasswordIncorrect &e) {
                        std::cout << "Wrong password or bad GostCrypt volume." << std::endl;
                    } catch (GostCrypt::VolumeAlreadyMounted &e) {
                        std::cout << "Volume has already been mounted." << std::endl;
                    }
                } catch (GostCrypt::SystemException e) {
                    switch(e.GetErrorCode())
                    {
                    //not a volume file
                    case 22:
                        std::cout << "Bad file, Not a GostCrypt volume." << std::endl;
                        break;
                    }
                    std::cout << "An unexpected error occured." << std::endl;
                }
            }
            break;
        case FirstCMD::createvolume://"create-volume":
        case FirstCMD::create://"create":
            if(isOk(parseCreate(app, parser, volumecreator, &error), parser, error)){
                try {
                    if(creationoptions.Type == Hidden){
                        std::cout << "Hidden volumes not supported yet." << std::endl;
                        break;
                    }
                    volumecreator->createVolume();
                } catch (GostCrypt::SystemException e) {
                    std::cout << "An unexpected error occured." << std::endl;
                }
            }
            break;
        case FirstCMD::umount://"umount":
        case FirstCMD::unmount://"unmount":
        case FirstCMD::dismount://"dismount":
            if(isOk(parseDismount(app, parser, &volume, &error), parser, error)){
                try {
                    GostCrypt::VolumePath path = GostCrypt::VolumePath(volume.toStdString());
                    shared_ptr<GostCrypt::VolumeInfo> v = GostCrypt::Core->GetMountedVolume(path);
                    if(v) GostCrypt::Core->DismountVolume(v);
                } catch (GostCrypt::SystemException e) {
                    std::cout << "An unexpected error occured." << std::endl;
                }
            }
        case FirstCMD::test://"test":
            std::cout << "Option not supported." << std::endl; // TODO
            break;
        case FirstCMD::dismountall://"dismount-all":
            try {
                volumes = GostCrypt::Core->GetMountedVolumes();
                for(GostCrypt::SharedPtr<GostCrypt::VolumeInfo> volume : volumes){
                    GostCrypt::Core->DismountVolume(volume);
                }
                std::cout << "All volumes dismounted." << std::endl;
            } catch (GostCrypt::SystemException e){
                std::cout << "An unexpected error occured." << std::endl;
            }
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
            if(isOk(parseList(app,parser,&item,&error),parser,error)){
                switch(item){
                    case Volumes:
                        volumes = GostCrypt::Core->GetMountedVolumes();
                        std::cout << "File Path                 \tAlgorithm      \tMount Point        \tVolume Size" << std::endl;
                        for(GostCrypt::SharedPtr<GostCrypt::VolumeInfo> v : volumes){
                            std::cout << string(v->Path) << "\t"
                                      << QString::fromWCharArray(v->EncryptionAlgorithmName.c_str()).toStdString() << "\t"
                                      << string(v->MountPoint) << "\t"
                                      << convertSize(v->Size).toStdString() << std::endl;
                        }
                        //break;
                    case Algorithms:
                        encryptionAlgos = GostCrypt::EncryptionAlgorithm::GetAvailableAlgorithms();
                        std::cout << "           \tName        \tKeySize" << std::endl;
                        for(GostCrypt::SharedPtr<GostCrypt::EncryptionAlgorithm> ea : encryptionAlgos){
                            if(ea->IsDeprecated()) std::cout << "DEPRECATED:\t"; else std::cout << "           \t";
                            std::cout << QString::fromWCharArray(ea->GetName().c_str()).toStdString();
                            std::cout << "\t" << ea->GetKeySize() << "B";
                            /*try {
                                std::cout << "\t" << QString::fromWCharArray(ea->GetMode()->GetName().c_str()).toStdString();
                            } catch (GostCrypt::NotInitialized &e){}*/
                            std::cout << std::endl;
                        }
                        //break;
                    case Hashs:
                        hashList = GostCrypt::Pkcs5Kdf::GetAvailableAlgorithms();
                        std::cout << "           \tName        \tIterations" << std::endl;
                        for(GostCrypt::SharedPtr<GostCrypt::Pkcs5Kdf> hash : hashList){
                            if(hash->IsDeprecated()) std::cout << "DEPRECATED: "; else std::cout << "           \t";
                            std::cout << QString::fromWCharArray(hash->GetName().c_str()).toStdString() << "\t";
                            std::cout << "\t" << hash->GetIterationCount() << std::endl;
                        }
                        break;
                    case FileSystems:
                        // all are from enum FileSystemType of VolumeCreator.h
                        std::cout << "None, FAT, NTFS, Ext2, Ext3, Ext4, MacOsExt, UFS" << std::endl;
                }
            }
            break;
        default:
            std::cout << "Not a valid command. Try --help for help.";
    }

    return 0;
}
