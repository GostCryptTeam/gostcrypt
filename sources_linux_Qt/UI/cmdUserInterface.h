#ifndef CMDUSERINTERFACE_H
#define CMDUSERINTERFACE_H

#include <QCommandLineParser>
#include <iostream>
#include <termios.h>
#include <unistd.h>

#include "NewCore/CoreBase.h"
#include "NewCore/CoreParams.h"
#include "NewCore/CoreResponse.h"
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

struct FirstCMD {
	enum Enum MK_ALL_COMMANDS(MK_ENUM);
	static const QStringList Str;
};


int handleCLI(int argc, char ** argv); // some kind of main for commands

#endif
