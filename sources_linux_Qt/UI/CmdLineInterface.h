#ifndef CMDLINEINTERFACE_H
#define CMDLINEINTERFACE_H

#include <QObject>
#include <QVariant>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include "Parser.h"
#include "NewCore/CoreParams.h"
#include "NewCore/CoreBase.h"

#define MK_ENUM(name) name // TODO move to external file ?
#define MK_STRTAB(name) #name
#define MK_ALL_COMMANDS(func) { \
    func(mount), \
    func(createvolume), \
    func(create), \
    func(umount), \
    func(unmount), \
    func(dismount), \
    func(test), \
    func(dismountall), \
    func(automount), \
    func(backupheaders), \
    func(createkeyfiles), \
    func(list) \
}

// redefines the notify function of QCoreApplication to catch all exceptions at once
class MyApplication : public QCoreApplication {
Q_OBJECT
public:
    MyApplication(int& argc, char** argv) : QCoreApplication(argc, argv) {}
    bool notify(QObject* receiver, QEvent* event);
signals:
    void exit();
};

class CmdLineInterface : public QObject {
Q_OBJECT
public:
    explicit CmdLineInterface(QObject *parent = nullptr);
    static QTextStream &qStdOut();
    int start(int argc, char **argv);
signals:
    void request(QVariant request);
    void exit();
    void sendSudoPassword(QSharedPointer<QByteArray> password);
private slots:
    void printCreateVolume(QSharedPointer<GostCrypt::NewCore::CreateVolumeResponse> res);
    void printMountVolume(QSharedPointer<GostCrypt::NewCore::MountVolumeResponse> res);
    void printDismountVolume(QSharedPointer<GostCrypt::NewCore::DismountVolumeResponse> res);
    void printGetMountedVolumes(QSharedPointer<GostCrypt::NewCore::GetMountedVolumesResponse> res);
    void askSudoPassword();

private:
    void processRequest();

    QSharedPointer<GostCrypt::NewCore::CoreBase> core;
    QCommandLineParser parser;

    struct FirstCMD {
        enum Enum MK_ALL_COMMANDS(MK_ENUM);
        static const QStringList Str;
    };
};

#endif // CMDLINEINTERFACE_H
