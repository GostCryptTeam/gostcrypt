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

#define DEFAULT_ALGORITHM "Gost Grasshopper"
#define DEFAULT_KDF "HMAC-Whirlpool"
#define DEFAULT_SIZE 10485760 // 10Mio
#define DEFAULT_OUTER_SIZE 1.0
#define DEFAULT_INNER_SIZE 0.7

class GraphicInterface;

/**
 * @brief
 *
 */
class MyGuiApplication : public QApplication {
Q_OBJECT
public:
    /**
     * @brief
     *
     * @param argc
     * @param argv
     */
    MyGuiApplication(int& argc, char** argv) : QApplication(argc, argv), mGI(nullptr) {}
    /**
     * @brief
     *
     * @param receiver
     * @param event
     * @return bool
     */
    bool notify(QObject* receiver, QEvent* event);
    /**
     * @brief
     *
     * @param gi
     */
    void setGI(GraphicInterface* gi) { mGI = gi; }
signals:
    /**
     * @brief
     *
     */
    void askExit();
private:
    GraphicInterface* mGI; /**< TODO: describe */
};

/**
 * @brief
 *
 */
class GraphicInterface : public UserInterface {
    Q_OBJECT
public:
    /**
     * @brief
     *
     * @param aApp
     * @param parent
     */
    explicit GraphicInterface(MyGuiApplication* aApp, QObject *parent = nullptr);
    /**
     * @brief
     *
     * @param argc
     * @param argv
     * @return int
     */
    int start(int argc, char **argv);
    /**
     * @brief
     *
     * @param sizeInByte
     * @return QString
     */
    static QString formatSize(quint64 sizeInByte, bool withFontColor = true);

private slots:
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
    DEC_PRINT_SLOT(ProgressUpdate);
    DEC_PRINT_SLOT(BenchmarkAlgorithms);
    /**
     * @brief
     *
     */
    virtual void askSudoPassword();

signals:
    /**
     * @brief
     *
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
     *
     */
    void getSudoPassword();
    /**
     * @brief
     *
     */
    void volumePasswordIncorrect();
    //void printSendError(QString aTitle, QString aContent);

private:
    /**
     * @brief
     *
     */
    Q_INVOKABLE void connectSignals();
    MyGuiApplication* mApp; /**< TODO: describe */
    QQmlApplicationEngine mEngine; /**< TODO: describe */
    UserSettings mSettings; /**< TODO: describe */
    DragWindowProvider mDrag; /**< TODO: describe */
    TranslationApp mTranslation; /**< TODO: describe */

    /**
     * @brief
     *
     */
    struct UI {
        /**
         * @brief
         *
         */
        enum Enum GI_ALL_COMMANDS(GI_ENUM);
        static const QStringList Str; /**< TODO: describe */
    };
};

#endif
