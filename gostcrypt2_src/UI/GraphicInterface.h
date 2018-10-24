#ifndef GRAPHIC_INTERFACE
#define GRAPHIC_INTERFACE

#include <QObject>
#include <QVariant>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "Core/CoreRequest.h"
#include "Core/CoreBase.h"
#include "UserInterface.h"
#include "DragWindowProvider.h"
#include "UserSettings.h"
#include "TranslationApp.h"
#include "SecureTextField.h"
#include <QUrl>
#include <QString>
#include "Parser.h"

#define GI_ENUM(name) name // TODO move to external file ?
#define GI_STRTAB(name) #name
#define GI_ALL_COMMANDS(func) { \
    func(mount), \
    func(createvolume), \
    func(dismount), \
    func(dismountall), \
    func(automount), \
    func(backupheaders), \
    func(createkeyfiles), \
    func(mountedvolumes), \
    func(algorithms), \
    func(filesystems), \
    func(hashs), \
    func(devices), \
    func(openmountpoint), \
    func(closeallsts), \
    func(changepassword), \
    func(clearvolumehistory), \
    func(openguide), \
    func(onlinehelp), \
    func(beginnertuto), \
    func(faq), \
    func(website), \
    func(download), \
    func(news), \
    func(benchmark), \
    func(versionhistory), \
    func(contact) \
}
#define GI_KEY(variant, key) variant.toMap().value(key)
#define DEC_QML_PRINT_SIGNAL(requestName) void sprint ## requestName (QVariantList l);
#define CONNECT_QML_SIGNAL(requestName) mApp->connect(core.data(), SIGNAL(send ## requestName (QSharedPointer<GostCrypt::Core::requestName ## Response>)), this, SLOT(print ## requestName (QSharedPointer<GostCrypt::Core::requestName ## Response>)))
#define QML_SIGNAL(requestName, params) s ## requestName (params);

class GraphicInterface;

/**
 * @brief
 * This class inherits QApplication and allows you to
 * retrieve events such as exceptions in order to manage
 * them graphically
 */
class MyGuiApplication : public QGuiApplication
{
    Q_OBJECT
 public:
    /**
     * @brief Initializes the QML application and class variables
     *
     * @param argc the number of arguments in the main program
     * @param argv arguments of the main program
     */
    MyGuiApplication(int& argc, char** argv) : QGuiApplication(argc, argv), mGI(nullptr) {}
    /**
     * @brief Retrieve exceptions and forward them to the interface for display
     *
     * @param receiver Object that receives the event
     * @param event Event that has occurred and must be transmitted
     * @return bool Boolean indicating whether the event has been transmitted or not
     */
    bool notify(QObject* receiver, QEvent* event);
    /**
     * @brief Retrieve a pointer on the graphical interface object
     *
     * @param gi Pointer to the currently instantiated QML interface
     */
    void setGI(GraphicInterface* gi) { mGI = gi; }
 signals:
    /**
     * @brief This signal makes it possible to finish the program neatly
     * by allowing the interface to tell the core that it will end
     */
    void askExit();
 private:
    GraphicInterface*
    mGI; /**< Pointer that will contain the address of the instantiated QML interface */
};

/**
 * @brief
 * This class displays a graphical interface using
 * QML for GostCrypt
 */
class GraphicInterface : public UserInterface
{
    Q_OBJECT
 public:
    /**
     * @brief Graphical user interface constructor, based on the MyGuiApplication class that inherits from QApplication
     *
     * @param aApp Parent object of the GUI: allows you to retrieve signals and start a QApplication
     * @param parent The object from which the class derives, which has useful methods for making certain calculations
     */
    explicit GraphicInterface(MyGuiApplication* aApp, QObject* parent = nullptr);
    /**
     * @brief Creates the graphical interface and sends all the necessary objects to it in context
     *
     * @param argc The number of arguments in the main program
     * @param argv Arguments of the main program
     * @return int Returns what the "exec" method returns at the end of the interface execution
     */
    int start(int argc, char** argv);
    /**
     * @brief Converts A size in bytes into a formatted character string (or not formatted)
     *
     * @param sizeInByte Size in bytes to convert
     * @param withFontColor This boolean allows you to specify whether you want an output formatted in HTML (true) or not (false)
     * @return QString Character string of the size converted to the desired format
     */
    static QString formatSize(quint64 sizeInByte, bool withFontColor = true);

 private slots:  // NOLINT
    /*!
     * \brief receiveSignal
     * Binds the signals from QML to the Core side
     */
    void receiveSignal(QString, QVariant);
    DEC_PRINT_SLOT(CreateVolume);
    DEC_PRINT_SLOT(MountVolume);
    DEC_PRINT_SLOT(DismountVolume);
    DEC_PRINT_SLOT(GetMountedVolumes);
    DEC_PRINT_SLOT(GetEncryptionAlgorithms);
    DEC_PRINT_SLOT(GetDerivationFunctions);
    DEC_PRINT_SLOT(GetHostDevices);
    DEC_PRINT_SLOT(CreateKeyFile);
    DEC_PRINT_SLOT(ChangeVolumePassword);
    DEC_PRINT_SLOT(BenchmarkAlgorithms);

    /**
     * @brief Display the new progress for the corresponding request in the UI
     */
    virtual void printProgressUpdate(quint32 requestId, qreal progress);

    /**
     * @brief
     * Slot called when the core needs the user to enter the sudo password.
     */
    virtual void askSudoPassword();

 signals:
    /**
     * @brief
     * Control signal sent to the interface to indicate that the signals are well connected
     */
    void connectFinished();

    //Signals that are called after the Core response :
    DEC_QML_PRINT_SIGNAL(CreateVolume)
    DEC_QML_PRINT_SIGNAL(MountVolume)
    DEC_QML_PRINT_SIGNAL(DismountVolume)
    DEC_QML_PRINT_SIGNAL(GetMountedVolumes)
    DEC_QML_PRINT_SIGNAL(GetEncryptionAlgorithms)
    DEC_QML_PRINT_SIGNAL(GetDerivationFunctions)
    DEC_QML_PRINT_SIGNAL(GetHostDevices)
    DEC_QML_PRINT_SIGNAL(GetFileSystem)
    DEC_QML_PRINT_SIGNAL(CreateKeyFile)
    DEC_QML_PRINT_SIGNAL(ChangeVolumePassword)
    DEC_QML_PRINT_SIGNAL(ProgressUpdate)
    DEC_QML_PRINT_SIGNAL(SendError)
    DEC_QML_PRINT_SIGNAL(BenchmarkAlgorithms)
    DEC_QML_PRINT_SIGNAL(BackupHeaderComplete)


    /**
     * @brief
     * Signal to request the sudo password from the user
     */
    void getSudoPassword();
    /**
     * @brief
     * Signal to indicate to the interface that the entered password is incorrect (volume)
     */
    void volumePasswordIncorrect();

 private:
    /**
     * @brief
     * Method callable from QML to connect signals between the interface and the Core
     */
    Q_INVOKABLE void connectSignals();
    MyGuiApplication*
    mApp; /**< Object inheriting the QApplication, the interface works thanks to this object */
    QQmlApplicationEngine
    mEngine; /**< QML rendering engine used to render the display, contains the main context */
    UserSettings mSettings; /**< User preferences */
    DragWindowProvider mDrag; /**< Class that allows you to move the window (which is borderless) */
    TranslationApp mTranslation; /**< Class to translate the interface into different languages */

    /**
     * @brief
     * Structure that contains all actions that can be done from the interface to parse them with a switch/case in the recovery function
     */
    struct UI
    {
        /**
         * @brief
         * Generates all possible actions (see top of GraphicInterface.h for details)
         */
        enum Enum GI_ALL_COMMANDS(GI_ENUM);
        static const QStringList Str; /**< List of all QML actions generated */
    };
};

#endif
