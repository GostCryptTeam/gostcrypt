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

    core = GostCrypt::NewCore::getCore();

    mApp->setApplicationName("GostCrypt");

    QQmlContext* ctx = mEngine.rootContext();

    ctx->setContextProperty("ConnectSignals", this);
    ctx->setContextProperty("UserSettings", &mSettings);
    ctx->setContextProperty("DragWindowProvider", &mDrag);
    ctx->setContextProperty("Wizard", &mWizard);
    ctx->setContextProperty("Translation", (QObject*)&mTranslation);

    mEngine.load(QUrl(QStringLiteral("qrc:/UI/main.qml")));

    /* Connecting all signals */
    QMetaObject::invokeMethod(this, "connectSignals", Qt::QueuedConnection);

    return mApp->exec();
}

/*!
 * \brief GraphicInterface::receiveSignal
 * This slot is called when the QML sends a signal
 * Analyzes the signal content to call the right
 * GostCrypt function
 */
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
            QSharedPointer <GostCrypt::NewCore::GetMountedVolumesRequest> options(new GostCrypt::NewCore::GetMountedVolumesRequest);
            emit request(QVariant::fromValue(options));
        }
        break;
    case FirstGI::openmountpoint: //"openmountpoint" command
        {
#ifdef Q_WS_WIN
            if (QFileInfo(aContent["path"]).isDir())
                QProcess::startDetached("explorer", QStringList(GI_KEY(aContent, "path").toString()));
            else
#else
        QDesktopServices::openUrl(QUrl(GI_KEY(aContent, "path").toString()));
#endif
        }
        break;
    case FirstGI::automount: //"automount" command
        {
            //TODO : automount
        }
        break;
    case FirstGI::dismountall: //"dismountall" command
        {
            QSharedPointer<GostCrypt::NewCore::DismountVolumeRequest> options(new GostCrypt::NewCore::DismountVolumeRequest);
            emit request(QVariant::fromValue(options));
        }
        break;
    case FirstGI::dismount: //"dismount" command
        {
            QSharedPointer<GostCrypt::NewCore::DismountVolumeRequest> options(new GostCrypt::NewCore::DismountVolumeRequest);
            if(GI_KEY(aContent, "volumepath") != "") {
                options.data()->volumePath.reset(new QFileInfo(QFileInfo(GI_KEY(aContent, "volumepath").toString())));
                emit request(QVariant::fromValue(options));
            }else{
                //TODO : send error to QML
            }
        }
        break;
    case FirstGI::listoffavorites: //"listoffavorites"
        {
            QVariantList list = mSettings.getFavoritesVolumes();
            for(QVariant volume : list)
            {
                qDebug() << volume.toString();
            }
        }
        break;
    case FirstGI::mount: //"mount"
        {
        qDebug() << "AH";
            QSharedPointer <GostCrypt::NewCore::MountVolumeRequest> options(new GostCrypt::NewCore::MountVolumeRequest);
            options->path.reset(new QFileInfo(GI_KEY(aContent, "path").toString()));
            options->password.reset(new QByteArray(GI_KEY(aContent, "password").toString().toLocal8Bit()));
            emit request(QVariant::fromValue(options));
        }
        break;
    }
}

void GraphicInterface::printGetMountedVolumes(QSharedPointer<GostCrypt::NewCore::GetMountedVolumesResponse> response)
{
#ifdef QT_DEBUG
    qDebug() << "[Debug] : Receiving the list of mounted volumes.";
#endif
    QVariantList list;
    for(auto v = response->volumeInfoList.begin(); v < response->volumeInfoList.end(); ++v)
    {
       QVariantMap vol;
       vol.insert("mountPoint", (*v)->mountPoint->filePath());
       vol.insert("algo", (*v)->encryptionAlgorithmName);
       vol.insert("volumePath", (*v)->volumePath->filePath());
       vol.insert("volumeSize", formatSize((*v)->size));
       list.append(vol);
    }
    sPrintGetMountedVolumes(list);
}

void GraphicInterface::printDismountVolume(QSharedPointer<GostCrypt::NewCore::DismountVolumeResponse> response)
{
    (void)response;
    emit sPrintDismountVolume();
}

void GraphicInterface::askSudoPassword()
{
#ifdef QT_DEBUG
    qDebug() << "Sudo password asked";
#endif
    emit getSudoPassword();
}

/*!
 * \brief GraphicInterface::connectSignals Binds the signals between the
 * GostCrypt core and the QML interface
 */
void GraphicInterface::connectSignals()
{
    /************* QML -----> C++ ************
     * Connects every signals from QML to the
     * "receiveSignal" slot of
     * the current class
    ******************************************/
    QObject* qml = mEngine.rootObjects().first();
    connect(qml, SIGNAL(qmlRequest(QString, QVariant)), this, SLOT(receiveSignal(QString,QVariant)));

    /***** GraphicInterface -----> Core ******/
    //Connecting request from here to request switch from Core
    mApp->connect(this, SIGNAL(request(QVariant)), core.data(), SLOT(request(QVariant)));

    /***** Core -----> GraphicInterface ******/
    mApp->connect(core.data(), SIGNAL(sendGetMountedVolumes(QSharedPointer<GostCrypt::NewCore::GetMountedVolumesResponse>)), this, SLOT(printGetMountedVolumes(QSharedPointer<GostCrypt::NewCore::GetMountedVolumesResponse>)));
    mApp->connect(core.data(), SIGNAL(sendDismountVolume(QSharedPointer<GostCrypt::NewCore::DismountVolumeResponse>)), this, SLOT(printDismountVolume(QSharedPointer<GostCrypt::NewCore::DismountVolumeResponse>)));


    /* Connecting the signals to get the sudo request from Core and send it to Core */
    mApp->connect(core.data(), SIGNAL(askSudoPassword()), this, SLOT(askSudoPassword()));
    mApp->connect(qml, SIGNAL(sendSudoPassword(QString)), core.data(), SLOT(receiveSudoPassword(QString)));
    //mApp->connect(core.data(), SIGNAL(sudoPasswordSuccess()), this, SLOT(sendSudoStatus()));


    /* Connecting few exit signals to close the program apropriately */
    mApp->connect(this, SIGNAL(exit()), core.data(), SLOT(exit()));
    mApp->connect(mApp, SIGNAL(exit()), core.data(), SLOT(exit()));
    mApp->connect(core.data(), SIGNAL(exited()), mApp, SLOT(quit()));


    //Notifying the QML that the signals are binded
    emit connectFinished();
}

bool MyGuiApplication::notify(QObject *receiver, QEvent *event)
{
    bool done = true;
    try {
        done = QCoreApplication::notify(receiver, event);
    } catch(GostCrypt::NewCore::CoreException &e) {
        //CmdLineInterface::qStdOut() << e.displayedMessage();
        emit mGI->sendError("Exception catch", "An unexpected error occured. \n"
#ifdef QT_DEBUG
        +QString::fromUtf8(e.what())
#endif
        );
        emit exit();
    } catch (QException &e) { // TODO : handle exceptions here
       // CmdLineInterface::qStdOut() << e.what();
        emit exit();
    }
    return done;
}
