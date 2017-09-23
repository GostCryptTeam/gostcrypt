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
#include "NewCore/CoreRequest.h"
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
    func(dismountall), \
    func(createkeyfiles), \
    func(list), \
}

#define CONNECT_SIGNAL(requestName) app.connect(core.data(), SIGNAL(send ## requestName (QSharedPointer<GostCrypt::NewCore::requestName ## Response>)), this, SLOT(print ## requestName (QSharedPointer<GostCrypt::NewCore::requestName ## Response>)))
#define DEC_PRINT_SLOT(requestName) void print ## requestName (QSharedPointer<GostCrypt::NewCore::requestName ## Response> r)


// redefines the notify function of QCoreApplication to catch all exceptions at once
class MyApplication : public QCoreApplication {
Q_OBJECT
public:
    MyApplication(int& argc, char** argv) : QCoreApplication(argc, argv) {}
    bool notify(QObject* receiver, QEvent* event);
signals:
    void askExit();
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
