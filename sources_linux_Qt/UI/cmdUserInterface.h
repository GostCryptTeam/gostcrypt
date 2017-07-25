#ifndef CMDUSERINTERFACE_H
#define CMDUSERINTERFACE_H

#include <QCommandLineParser>
#include <iostream>
#include <termios.h>
#include <unistd.h>

#include "Core/Core.h"
#include "Core/Unix/CoreService.h"
#include "Platform/SystemLog.h"
#include "Volume/Pkcs5Kdf.h"
#include "parser.h"
#include "cmdUserInterface.h"
#include "volumecreation.h"

class AdminPasswordCLIRequestHandler : public GostCrypt::GetStringFunctor
{
public:
    AdminPasswordCLIRequestHandler() {}
    virtual void operator() (string &passwordStr);
};

#define MK_ENUM(name) name
#define MK_STRTAB(name) #name
#define MK_ALL(func) { \
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

struct FirstCMD {
    enum Enum MK_ALL(MK_ENUM);
    static const QStringList Str MK_ALL(MK_STRTAB);
};


int handleCLI(int argc, char ** argv); // some kind of main for commands

#endif
