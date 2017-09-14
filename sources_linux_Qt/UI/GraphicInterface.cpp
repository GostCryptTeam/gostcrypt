#include "GraphicInterface.h"

///*! converts byte to MB, GB, KB */
//QString formatSize(uint64 sizeInByte) {
//    if (sizeInByte < 1024) return QString(QString("<font color=#6e9f45>")
//        + QString::number(sizeInByte)
//        + QString("</font>")
//        + QString(" B"));

//    else if (sizeInByte < 1048576) return QString("<font color=#6e9f45>")
//        + QString::number((float)sizeInByte / (float)1024, 'f', 1)
//        + QString("</font>")
//        + QString(" KB");

//    else if (sizeInByte < 1073741824) return QString("<font color=#6e9f45>")
//        + QString::number((float)sizeInByte / (float)1048576, 'f', 1)
//        + QString("</font>")
//        + QString(" MB");

//    else return QString("<font color=#6e9f45>")
//        + QString::number((float)sizeInByte / (float)1073741824, 'f', 1)
//        + QString("</font>")
//        + QString(" GB");
//}

GraphicInterface::GraphicInterface(MyGuiApplication* aApp, QObject *parent)
    : QObject(parent)
{
    mApp = aApp;
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
    //connectSignals(engine.rootObjects().first());
    QMetaObject::invokeMethod(this, "connectSignals", Qt::QueuedConnection);

    return mApp->exec();
}

//void GraphicInterface::sGetAllMountedVolumes()
//{
//    /*GostCrypt::VolumeInfoList volumes = mGUI->receiveGetAllVolumes();
//    for(GostCrypt::SharedPtr<GostCrypt::VolumeInfo> volume : volumes){
//        this->subWindowSendVolumeInfos(volume->MountPoint, volume->EncryptionAlgorithmName, volume->Path, volume->Size);
//    }*/
//}

/*!
 * \brief GraphicInterface::receiveSignal
 * This slot is called when the QML sends a signal
 * Analyzes the signal content to call the right
 * GostCrypt function
 */
void GraphicInterface::receiveSignal(QVariant aContent)
{
    qDebug() << "Signal reçu de qml = " << aContent.toStringList();
}

/*!
 * \brief GraphicInterface::connectSignals Binds the signals between the
 * GostCrypt core and the QML interface
 */
void GraphicInterface::connectSignals(/*QObject* aQml*/)
{
    QObject* qml = mEngine.rootObjects().first();
    //Connects every signals from QML to the "receiveSignal" slot of
    //the current class
    qDebug() << "A";
    //QML -----> C++
    connect(qml, SIGNAL(qmlTest(QVariant)), this, SLOT(receiveSignal(QVariant)));

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
        emit exit();
    } catch (QException &e) { // TODO : handle exceptions here
       // CmdLineInterface::qStdOut() << e.what();
        emit exit();
    }
    return done;
}
