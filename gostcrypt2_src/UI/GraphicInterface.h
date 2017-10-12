#ifndef GRAPHIC_INTERFACE
#define GRAPHIC_INTERFACE

#include <QObject>
#include <QVariant>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "NewCore/CoreRequest.h"
#include "NewCore/CoreBase.h"
#include "DragWindowProvider.h"
#include "volumecreation.h"
#include "UserSettings.h"
#include "TranslationApp.h"
#include "SecureTextField.h"
#include <QUrl>
#include <QString>

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
    func(hashs), \
    func(devices), \
    func(openmountpoint) \
}
#define GI_KEY(variant, key) variant.toMap().value(key)
#define DEC_PRINT_SLOT(requestName) void print ## requestName (QSharedPointer<GostCrypt::NewCore::requestName ## Response> r)
#define CONNECT_QML_SIGNAL(requestName) mApp->connect(core.data(), SIGNAL(send ## requestName (QSharedPointer<GostCrypt::NewCore::requestName ## Response>)), this, SLOT(print ## requestName (QSharedPointer<GostCrypt::NewCore::requestName ## Response>)))
#define QML_SIGNAL(requestName) emit sPrint ## requestName ();

#define DEFAULT_ALGORITHM "Gost Grasshopper"
#define DEFAULT_KDF "HMAC-Whirlpool"
#define DEFAULT_SIZE 10485760 // 10Mio
#define DEFAULT_OUTER_SIZE 1.0
#define DEFAULT_INNER_SIZE 0.7

class GraphicInterface;

class MyGuiApplication : public QApplication {
Q_OBJECT
public:
    MyGuiApplication(int& argc, char** argv) : QApplication(argc, argv) {}
    bool notify(QObject* receiver, QEvent* event);
    void setGI(GraphicInterface* gi) { mGI = gi; }
signals:
    void askExit();
private:
    GraphicInterface* mGI;
};

class GraphicInterface : public QObject {
    Q_OBJECT
public:
    explicit GraphicInterface(MyGuiApplication* aApp, QObject *parent = nullptr);
    int start();

private slots:
    /*!
     * \brief receiveSignal
     * Binds the signals from QML to the Core side
     */
    void receiveSignal(QString, QVariant);
    /*!
     * \brief printGetMountedVolumes
     * retrieves the list of mounted volumes sent from the core
     * \param result a pointer of the list of mounted volumes
     */
    DEC_PRINT_SLOT(CreateVolume);
    DEC_PRINT_SLOT(MountVolume);
    DEC_PRINT_SLOT(DismountVolume);
    DEC_PRINT_SLOT(GetMountedVolumes);
    DEC_PRINT_SLOT(GetEncryptionAlgorithms);
    DEC_PRINT_SLOT(GetDerivationFunctions);
    DEC_PRINT_SLOT(GetHostDevices);
    DEC_PRINT_SLOT(CreateKeyFile);
    DEC_PRINT_SLOT(ChangeVolumePassword);
    void askSudoPassword();
    //void sendSudoStatus(); TODO

signals:
    void request(QVariant request);
    void exit();
    void sendSudoPassword(QString password);
    void connectFinished();
    //Signals that are called after the Core response :
    void sPrintCreateVolume();
    void sPrintDismountVolume();
    void sPrintMountVolume();
    void sPrintGetMountVolume(QVariantList volumes);
    void sPrintGetEncryptionAlgorithms(QVariantList algos);
    void sPrintDerivationFunctions(QVariantList functions);
    void sPrintHostDevices(QVariantList hostDevices);
    void sPrintCreateKeyFile(QString keyfile);
    void sPrintChangeVolumePassword();

    void getSudoPassword();
    void volumePasswordIncorrect();
    void sendError(QString aTitle, QString aContent);

private:
    Q_INVOKABLE void connectSignals();
    MyGuiApplication* mApp;
    QQmlApplicationEngine mEngine;
    QSharedPointer<GostCrypt::NewCore::CoreBase> core;
    UserSettings mSettings;
    DragWindowProvider mDrag;
    TranslationApp mTranslation;

    struct FirstGI {
        enum Enum GI_ALL_COMMANDS(GI_ENUM);
        static const QStringList Str;
    };
};

#endif
