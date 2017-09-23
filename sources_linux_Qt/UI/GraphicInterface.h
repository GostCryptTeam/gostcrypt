#ifndef GRAPHIC_INTERFACE
#define GRAPHIC_INTERFACE

#include <QObject>
#include <QVariant>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QGuiApplication>
#include "NewCore/CoreRequest.h"
#include "NewCore/CoreBase.h"
#include "DragWindowProvider.h"
#include "volumecreation.h"
#include "UserSettings.h"
#include "TranslationApp.h"
#include <QUrl>
#include <QString>

#define GI_ENUM(name) name // TODO move to external file ?
#define GI_STRTAB(name) #name
#define GI_ALL_COMMANDS(func) { \
    func(mount), \
    func(createvolume), \
    func(create), \
    func(dismount), \
    func(dismountall), \
    func(automount), \
    func(backupheaders), \
    func(createkeyfiles), \
    func(mountedvolumes), \
    func(openmountpoint) \
}
#define GI_KEY(variant, key) variant.toMap().value(key)

class GraphicInterface;

class MyGuiApplication : public QGuiApplication {
Q_OBJECT
public:
    MyGuiApplication(int& argc, char** argv) : QGuiApplication(argc, argv) {}
    bool notify(QObject* receiver, QEvent* event);
    void setGI(GraphicInterface* gi) { mGI = gi; }
signals:
    void exit();
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
    void printGetMountedVolumes(QSharedPointer<GostCrypt::NewCore::GetMountedVolumesResponse>);
    void printDismountVolume(QSharedPointer<GostCrypt::NewCore::DismountVolumeResponse>);
    void printVolumeMounted(QSharedPointer<GostCrypt::NewCore::MountVolumeResponse>);
    void askSudoPassword();
    //void sendSudoStatus(); TODO

signals:
    void request(QVariant request);
    void connectFinished();
    //Signals that are called after the Core response :
    void sPrintGetMountedVolumes(QVariantList volumes);
    void sPrintDismountVolume();
    void sPrintVolumeMounted();
    void sendSudoPassword(QString password);
    void getSudoPassword();
    void volumePasswordIncorrect();
    void exit();
    void sendError(QString aTitle, QString aContent);

private:
    Q_INVOKABLE void connectSignals();
    MyGuiApplication* mApp;
    QQmlApplicationEngine mEngine;
    QSharedPointer<GostCrypt::NewCore::CoreBase> core;
    UserSettings mSettings;
    DragWindowProvider mDrag;
    TranslationApp mTranslation;
    VolumeCreation mWizard;

    struct FirstGI {
        enum Enum GI_ALL_COMMANDS(GI_ENUM);
        static const QStringList Str;
    };
};

#endif
