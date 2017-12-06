#include "GraphicInterface.h"

const QStringList GraphicInterface::FirstGI::Str = GI_ALL_COMMANDS(GI_STRTAB);

/*! converts byte to MB, GB, KB */
QString formatSize(uint64 sizeInByte) {
    if (sizeInByte < 1024) return QString(QString("<font color=#6e9f45>")
        + QString::number(sizeInByte)
        + QString("</font>")
        + QString(" B"));

    else if (sizeInByte < 1048576) return QString("<font color=#6e9f45>")
        + QString::number((float)sizeInByte / (float)1024, 'f', 1)
        + QString("</font>")
        + QString(" KB");

    else if (sizeInByte < 1073741824) return QString("<font color=#6e9f45>")
        + QString::number((float)sizeInByte / (float)1048576, 'f', 1)
        + QString("</font>")
        + QString(" MB");

    else return QString("<font color=#6e9f45>")
        + QString::number((float)sizeInByte / (float)1073741824, 'f', 1)
        + QString("</font>")
        + QString(" GB");
}

GraphicInterface::GraphicInterface(MyGuiApplication* aApp, QObject *parent)
    : QObject(parent)
{
    mApp = aApp;
    mApp->setGI(this);
}

int GraphicInterface::start()
{
    mApp->setWindowIcon(QIcon(":/logo_gostcrypt.png"));

    core = GostCrypt::Core::getCore();

    mApp->setApplicationName("GostCrypt");

    QQmlContext* ctx = mEngine.rootContext();

    ctx->setContextProperty("ConnectSignals", this);
    ctx->setContextProperty("UserSettings", &mSettings);
    ctx->setContextProperty("DragWindowProvider", &mDrag);
    ctx->setContextProperty("Translation", (QObject*)&mTranslation);
    qmlRegisterType<SecureTextField>("gostcrypt.ui.secureInput", 1, 0, "SecureTextField");

    mEngine.load(QUrl(QStringLiteral("qrc:/UI/main.qml")));

    /* Connecting all signals */
    QMetaObject::invokeMethod(this, "connectSignals", Qt::QueuedConnection);

    return mApp->exec();
}

void GraphicInterface::receiveSignal(QString command, QVariant aContent)
{
#ifdef QT_DEBUG
    qDebug() << "Command requested by QML = " << command;
#endif
    //using the mix between a QString tab and a enumeration.
    uint32 value = FirstGI::Str.indexOf(QRegExp(command, Qt::CaseInsensitive));

    switch(value){
    case FirstGI::mountedvolumes: //"mountedvolumes" command
        {
            QSharedPointer <GostCrypt::Core::GetMountedVolumesRequest> options(new GostCrypt::Core::GetMountedVolumesRequest);
            options->id = GostCrypt::Core::ProgressTrackingParameters(GI_KEY(aContent, "id").toInt());
            emit request(QVariant::fromValue(options));
        }
        break;
    case FirstGI::openmountpoint: //"openmountpoint" command
        {
#ifdef QT_DEBUG
        qDebug() << QUrl(GI_KEY(aContent, "path").toString());
#endif
        QDesktopServices::openUrl(QUrl(GI_KEY(aContent, "path").toString()));
        }
        break;
    case FirstGI::automount: //"automount" command
        {
            //TODO : automount
        }
        break;
    case FirstGI::dismountall: //"dismountall" command
        {
            QSharedPointer<GostCrypt::Core::DismountVolumeRequest> options(new GostCrypt::Core::DismountVolumeRequest);
            options->id = GostCrypt::Core::ProgressTrackingParameters(GI_KEY(aContent, "id").toInt());
            emit request(QVariant::fromValue(options));
        }
        break;
    case FirstGI::dismount: //"dismount" command
        {
            QSharedPointer<GostCrypt::Core::DismountVolumeRequest> options(new GostCrypt::Core::DismountVolumeRequest);
            options->id = GostCrypt::Core::ProgressTrackingParameters(GI_KEY(aContent, "id").toInt());
            if(GI_KEY(aContent, "volumepath") != "") {
                options.data()->volumePath.reset(new QFileInfo(QFileInfo(GI_KEY(aContent, "volumepath").toString())));
                emit request(QVariant::fromValue(options));
            }
        }
        break;
    case FirstGI::mount: //"mount"
        {
            QStringList keyfilesList;
            for(int i = 0; i<GI_KEY(aContent, "nb-keyfiles").toInt(); i++)
                keyfilesList.append(GI_KEY(aContent, "keyfile"+QString::number(i)).toString());

            QSharedPointer <GostCrypt::Core::MountVolumeRequest> options(new GostCrypt::Core::MountVolumeRequest);
            options->id = GostCrypt::Core::ProgressTrackingParameters(GI_KEY(aContent, "id").toInt());
            QString canonicalPath = GI_KEY(aContent, "path").toUrl().path();
            options->path.reset(new QFileInfo(canonicalPath));
            options->password.reset(new QByteArray(GI_KEY(aContent, "password").toString().toLocal8Bit()));
            options->doMount = true;

            //Mount options
            qDebug() << GI_KEY(aContent, "use-mount-option").toBool();
            if(GI_KEY(aContent, "use-mount-option").toBool())
            {
                options->preserveTimestamps = GI_KEY(aContent, "timestamp").toBool();
                switch(GI_KEY(aContent, "protection").toInt())
                {
                case 0:
                    options->protection = GostCrypt::VolumeProtection::None;
                    break;
                case 1:
                    options->protection = GostCrypt::VolumeProtection::ReadOnly;
                    break;
                case 2:
                    options->protection = GostCrypt::VolumeProtection::HiddenVolumeReadOnly;
                    break;
                }
                options->useBackupHeaders = GI_KEY(aContent, "backup-headers").toBool();
                options->sharedAccessAllowed = GI_KEY(aContent, "shared").toBool();
                options->mountedForUser = GI_KEY(aContent, "user").toString();
                options->mountedForGroup = GI_KEY(aContent, "group").toString();
            }

            //Adding Keyfiles
            options->keyfiles.reset(new QList<QSharedPointer<QFileInfo>>());
            for(QString file : keyfilesList)
                options->keyfiles->append(QSharedPointer<QFileInfo>(new QFileInfo(file)));

            emit request(QVariant::fromValue(options));
        }
        break;
    case FirstGI::createvolume: //"create-volume"
        {
            QSharedPointer <GostCrypt::Core::CreateVolumeRequest> options(new GostCrypt::Core::CreateVolumeRequest);
            options->id = GostCrypt::Core::ProgressTrackingParameters(GI_KEY(aContent, "id").toInt());
            //Detection of the volume type
            int type = GI_KEY(aContent, "type").toInt(); //UI returns 0 for normal and 1 for Hidden
            if(type == GostCrypt::VolumeType::Normal)
            {
                options->path = QSharedPointer<QFileInfo>(new QFileInfo(GI_KEY(aContent, "path").toString()));
                options->type = GostCrypt::VolumeType::Normal; //Setting the volume Type
                options->outerVolume->password.reset(new QByteArray(GI_KEY(aContent, "password").toString().toUtf8())); //Setting the outer volume password
                //options->innerVolume->password.reset(new QByteArray(GI_KEY(aContent, "hpassword").toString().toUtf8())); //hidden pwd
                //CLUSTER : very unsafe, not allowed for now!

                options->outerVolume->keyfiles.reset(new QList<QSharedPointer<QFileInfo>>()); //outer volume keyfile(s)
                QStringList keyfilesList;
                for(int i = 0; i<GI_KEY(aContent, "nb-keyfiles").toInt(); i++)
                    keyfilesList.append(GI_KEY(aContent, "keyfile"+QString::number(i)).toString());
                for(QString file : keyfilesList) //Adding the keyfile(s) to the outer volume object
                    options->outerVolume->keyfiles->append(QSharedPointer<QFileInfo>(new QFileInfo(QUrl(file).path())));

                options->outerVolume->keyfiles = nullptr; //Keyfiles not implemented yet. TODO

                options->outerVolume->volumeHeaderKdf = GI_KEY(aContent, "hash").toString(); //Outer volume hash
                options->outerVolume->encryptionAlgorithm = GI_KEY(aContent, "algorithm").toString(); //Outer volume algorithm
                options->outerVolume->filesystem = GI_KEY(aContent, "filesystem").toString(); //Outer volume file system
                options->outerVolume->size = 1.0; //GI_KEY(aContent, "outer-size").toReal(); //Relative size of the outer volume
                bool ok = false;
                QString s = GI_KEY(aContent, "size").toString();
                options->size = Parser::parseSize(s, &ok); //Total volume file size
            }
            else if(type == GostCrypt::VolumeType::Hidden)
            {
                options->path = QSharedPointer<QFileInfo>(new QFileInfo(GI_KEY(aContent, "path").toString()));
                options->type = GostCrypt::VolumeType::Hidden; //Setting the volume Type
                options->size = QFile(options->path->canonicalFilePath()).size();

                //Outer volume path and password/keyfile(s)
                options->outerVolume->password.reset(new QByteArray(GI_KEY(aContent, "password").toString().toUtf8())); //Setting the outer volume password
                options->outerVolume->keyfiles.reset(new QList<QSharedPointer<QFileInfo>>()); //outer volume keyfile(s)
                QStringList keyfilesList;
                for(int i = 0; i<GI_KEY(aContent, "nb-keyfiles").toInt(); i++)
                    keyfilesList.append(GI_KEY(aContent, "keyfile"+QString::number(i)).toString());
                for(QString file : keyfilesList) //Adding the keyfile(s) to the outer volume object
                    options->outerVolume->keyfiles->append(QSharedPointer<QFileInfo>(new QFileInfo(QUrl(file).path())));

                options->outerVolume->keyfiles = nullptr; //Keyfiles not implemented yet. TODO

                //Inner volume information
                options->innerVolume.reset(new GostCrypt::Core::CreateVolumeRequest::VolumeParams());
                options->innerVolume->encryptionAlgorithm = GI_KEY(aContent, "halgorithm").toString(); //Inner volume algorithm
                options->innerVolume->volumeHeaderKdf = GI_KEY(aContent, "hhash").toString(); //Inner volume algorithm
                options->innerVolume->filesystem = GI_KEY(aContent, "hfilesystem").toString(); //Inner volume file system
                options->innerVolume->size = GI_KEY(aContent, "inner-size").toReal(); //Relative size of the inner volume
                options->innerVolume->password.reset(new QByteArray(GI_KEY(aContent, "hpassword").toString().toUtf8())); //Setting the inner volume password
                QStringList hkeyfilesList;
                for(int i = 0; i<GI_KEY(aContent, "nb-hkeyfiles").toInt(); i++)
                    hkeyfilesList.append(GI_KEY(aContent, "hkeyfile"+QString::number(i)).toString());
                for(QString file : hkeyfilesList) //Adding the keyfile(s) to the outer volume object
                    options->innerVolume->keyfiles->append(QSharedPointer<QFileInfo>(new QFileInfo(QUrl(file).path())));
                qDebug() << GI_KEY(aContent, "hpassword").toString().toUtf8();
                options->innerVolume->keyfiles = nullptr; //Keyfiles not implemented yet. TODO
            }
            emit request(QVariant::fromValue(options));
        }
        break;
    case FirstGI::algorithms: //"algorithms":
        {
            QSharedPointer<GostCrypt::Core::GetEncryptionAlgorithmsRequest> options(new GostCrypt::Core::GetEncryptionAlgorithmsRequest);
            options->id = GostCrypt::Core::ProgressTrackingParameters(GI_KEY(aContent, "id").toInt());
            emit request(QVariant::fromValue(options));
        }
        break;
    case FirstGI::hashs: //"hashs":
        {
            QSharedPointer<GostCrypt::Core::GetDerivationFunctionsRequest> options(new GostCrypt::Core::GetDerivationFunctionsRequest);
            options->id = GostCrypt::Core::ProgressTrackingParameters(GI_KEY(aContent, "id").toInt());
            emit request(QVariant::fromValue(options));
        }
        break;
    case FirstGI::devices: //"devices":
        {
            QSharedPointer<GostCrypt::Core::GetHostDevicesRequest> options(new GostCrypt::Core::GetHostDevicesRequest);
            options->id = GostCrypt::Core::ProgressTrackingParameters(GI_KEY(aContent, "id").toInt());
            emit request(QVariant::fromValue(options));
        }
        break;
    case FirstGI::createkeyfiles: //"createkeyfiles":
        {
            QString keyfile;
            keyfile = GI_KEY(aContent, "keyfile").toUrl().path();
            // TODO : use KDF
            // TODO multiple keyfiles not supported yet
            QSharedPointer <GostCrypt::Core::CreateKeyFileRequest> options(new GostCrypt::Core::CreateKeyFileRequest());
            options->file.reset(new QFileInfo(keyfile));
            options->id = GostCrypt::Core::ProgressTrackingParameters(GI_KEY(aContent, "id").toInt());
            emit request(QVariant::fromValue(options));
        }
        break;
    case FirstGI::filesystems: //"filesystems"
        {
            QVariantList filesystem;
            filesystem << GostCrypt::Core::GetFileSystemTypePlatformNative(); // default value
            emit QML_SIGNAL(printGetFileSystem, filesystem)
        }
        break;
    }
}

void GraphicInterface::printGetMountedVolumes(QSharedPointer<GostCrypt::Core::GetMountedVolumesResponse> response)
{
#ifdef QT_DEBUG
    qDebug() << "[Debug] : Receiving the list of mounted volumes.";
#endif
    QVariantList list;
    for(auto v = response->volumeInfoList.begin(); v < response->volumeInfoList.end(); ++v)
    {
           QVariantMap vol;
           if((*v)->mountPoint)
                vol.insert("mountPoint", (*v)->mountPoint->absoluteFilePath());
           else
               vol.insert("mountPoint", tr("Not mounted"));
           vol.insert("algo", (*v)->encryptionAlgorithmName);
           vol.insert("volumePath", (*v)->volumePath->filePath());
           vol.insert("volumeSize", formatSize((*v)->size));
           list.append(vol);
    }
    emit QML_SIGNAL(printGetMountedVolumes, list)
}

void GraphicInterface::printDismountVolume(QSharedPointer<GostCrypt::Core::DismountVolumeResponse> response)
{
    (void)response;
    emit QML_SIGNAL(printDismountVolume, QVariantList());
}

void GraphicInterface::printMountVolume(QSharedPointer<GostCrypt::Core::MountVolumeResponse> response)
{
    (void)response;
    emit QML_SIGNAL(printMountVolume, QVariantList());
}

void GraphicInterface::printCreateVolume(QSharedPointer<GostCrypt::Core::CreateVolumeResponse> response)
{
    (void)response;
    emit QML_SIGNAL(printCreateVolume, QVariantList())
}

void GraphicInterface::printGetEncryptionAlgorithms(QSharedPointer<GostCrypt::Core::GetEncryptionAlgorithmsResponse> response)
{
    QVariantList list;
    for(auto k : response->algorithms) list << k;
    emit QML_SIGNAL(printGetEncryptionAlgorithms, list)
}

void GraphicInterface::printGetDerivationFunctions(QSharedPointer<GostCrypt::Core::GetDerivationFunctionsResponse> response)
{
    QVariantList list;
    for(auto k : response->algorithms) list << k;
    emit QML_SIGNAL(printGetDerivationFunctions, list)
}

void GraphicInterface::printProgressUpdate(QSharedPointer<GostCrypt::Core::ProgressUpdateResponse> r)
{
    QVariantList list;
    QVariantMap progress;
    progress.insert("id", r->requestId);
    progress.insert("progress", r->progress);
    list.append(progress);
    emit QML_SIGNAL(printProgressUpdate, list)
}

void GraphicInterface::printGetHostDevices(QSharedPointer<GostCrypt::Core::GetHostDevicesResponse> response)
{
    QVariantList list;
    for(auto v = response->hostDevices.begin(); v < response->hostDevices.end(); ++v)
    {
        if((*v)->mountPoint)
        {
            QVariantMap device;
            device.insert("mountPoint", (*v)->mountPoint->absoluteFilePath());
            device.insert("path", (*v)->devicePath->absoluteFilePath());
            device.insert("size", formatSize((*v)->size));
            list.append(device);
        }
        for(QSharedPointer<GostCrypt::Core::HostDevice> p : (*v)->partitions) {
            if(p->mountPoint)
            {
                QVariantMap device;
                device.insert("mountPoint", p->mountPoint->absoluteFilePath());
                device.insert("path", p->devicePath->absoluteFilePath());
                device.insert("size", formatSize(p->size));
                list.append(device);
            }
        }
    }
    emit QML_SIGNAL(printGetHostDevices, list)
}

void GraphicInterface::printCreateKeyFile(QSharedPointer<GostCrypt::Core::CreateKeyFileResponse> response)
{
    (void)response;
    emit QML_SIGNAL(printCreateKeyFile, QVariantList())
}

void GraphicInterface::printChangeVolumePassword(QSharedPointer<GostCrypt::Core::ChangeVolumePasswordResponse> response)
{
    (void)response;
    emit QML_SIGNAL(printChangeVolumePassword, QVariantList());
}


void GraphicInterface::askSudoPassword()
{
#ifdef QT_DEBUG
    qDebug() << "Sudo password asked";
#endif
    emit getSudoPassword();
}

void GraphicInterface::connectSignals()
{
    /************* QML -----> C++ ************
     * Connects every signals from QML to the
     * "receiveSignal" slot of
     * the current class
    ******************************************/
    QObject* qml = mEngine.rootObjects().first();
    connect(qml, SIGNAL(sendQmlRequest(QString, QVariant)), this, SLOT(receiveSignal(QString,QVariant)));
    connect(qml, SIGNAL(appQuit()), core.data(), SLOT(exit()));

    /***** GraphicInterface -----> Core ******/
    //Connecting request from here to request switch from Core
    mApp->connect(this, SIGNAL(request(QVariant)), core.data(), SLOT(request(QVariant)));

    /***** Core -----> GraphicInterface ******/
    CONNECT_QML_SIGNAL(CreateVolume);
    CONNECT_QML_SIGNAL(MountVolume);
    CONNECT_QML_SIGNAL(DismountVolume);
    CONNECT_QML_SIGNAL(GetMountedVolumes);
    CONNECT_QML_SIGNAL(GetEncryptionAlgorithms);
    CONNECT_QML_SIGNAL(GetDerivationFunctions);
    CONNECT_QML_SIGNAL(GetHostDevices);
    CONNECT_QML_SIGNAL(CreateKeyFile);
    CONNECT_QML_SIGNAL(ChangeVolumePassword);
    CONNECT_QML_SIGNAL(ProgressUpdate);

    /* Connecting the signals to get the sudo request from Core and send it to Core */
    mApp->connect(core.data(), SIGNAL(askSudoPassword()), this, SLOT(askSudoPassword()));
    mApp->connect(qml, SIGNAL(sendSudoPassword(QString)), core.data(), SLOT(receiveSudoPassword(QString)));


    /* Connecting few exit signals to close the program apropriately */
    mApp->connect(this, SIGNAL(exit()), core.data(), SLOT(exit()));
    mApp->connect(core.data(), SIGNAL(exited()), mApp, SLOT(quit()));

    mApp->connect(mApp, SIGNAL(askExit()), this, SIGNAL(exit()));

    //Notifying the QML that the signals are binded
    emit connectFinished();
}

bool MyGuiApplication::notify(QObject *receiver, QEvent *event)
{
    QVariantList response;
    bool done = true;
    try {
        done = QCoreApplication::notify(receiver, event);
    } catch(GostCrypt::Core::IncorrectVolumePassword &e) {
       emit mGI->volumePasswordIncorrect();
    } catch (GostCrypt::Core::CoreException &e) {
        response << e.getName() << "An unexpected error occured. \n"
#ifdef QT_DEBUG
        +e.getMessage()
#endif
        ;
        emit mGI->QML_SIGNAL(printSendError, response)
    } catch (QException &e) { // TODO : handle exceptions here

        response << "Exception catch" << "An unexpected error occured. \n"
#ifdef QT_DEBUG
        << QString::fromUtf8(e.what())
#endif
        ;
        emit mGI->QML_SIGNAL(printSendError, response)
    }
    return done;
}
