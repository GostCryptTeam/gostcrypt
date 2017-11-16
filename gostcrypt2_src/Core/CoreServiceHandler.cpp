#include "CoreServiceHandler.h"
#include "CoreResponse.h"
#include "CoreRequest.h"
#include <QProcess>
#include <QMetaEnum>
#include <QTimer>
#include <QDebug>
#include "CoreException.h"
#include "Service.h"

namespace GostCrypt {
    namespace Core {

		CoreServiceHandler::CoreServiceHandler() : ServiceHandler("/usr/bin/sudo", {"-k", "-S", "-p", "", QCoreApplication::applicationFilePath(), "coreservice"})
		{
			//Sudo might not require the password in cache
			connect(this, SIGNAL(started()), this, SIGNAL(askSudoPassword()));
		}

		void CoreServiceHandler::receiveSudoPassword(QSharedPointer<QByteArray> password)
		{
			// No need for password then
			if(isInitialized())
				return;

			writeToStdin(password, true);
			password->fill('\0');

			QTimer::singleShot(1000, this, SLOT(checkInitReceived()));
		}

		void CoreServiceHandler::checkInitReceived()
		{
			if(isInitialized())
				return;
			emit askSudoPassword();
		}

		void CoreServiceHandler::processCrashedBeforeInitialization()
		{
			throw IncorrectSudoPasswordException();
		}
	}
}
