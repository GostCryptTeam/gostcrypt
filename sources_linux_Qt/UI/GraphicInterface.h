
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

class MyGuiApplication : public QGuiApplication {
Q_OBJECT
public:
    MyGuiApplication(int& argc, char** argv) : QGuiApplication(argc, argv) {}
    bool notify(QObject* receiver, QEvent* event);
signals:
    void exit();
};

class GraphicInterface : public QObject {
    Q_OBJECT
public:
    explicit GraphicInterface(MyGuiApplication* aApp, QObject *parent = nullptr);
    int start();

public slots:
    void receiveSignal(QVariant);
   /* void sGetAllMountedVolumes();
    void sAutoMount(const QString&);
    void sDismount(const QString&);
    void sDismountAll();
    void sConnectSudo();
    void sConnectEndSudo();*/


signals:
    void connectFinished();

private:
    Q_INVOKABLE void connectSignals(/*QObject* aQml*/);
    QGuiApplication* mApp;
    QQmlApplicationEngine mEngine;
    QSharedPointer<GostCrypt::NewCore::CoreBase> core;
    UserSettings mSettings;
    DragWindowProvider mDrag;
    TranslationApp mTranslation;
    VolumeCreation mWizard;
};

#endif
