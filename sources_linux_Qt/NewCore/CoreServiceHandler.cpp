#include "CoreServiceHandler.h"
#include "CoreResponse.h"
#include "CoreParams.h"
#include <QProcess>
#include <QMetaEnum>
#include <QTimer>
#include <QDebug>
#include "CoreException.h"

namespace GostCrypt {
    namespace NewCore {

		CoreServiceHandler::CoreServiceHandler()
		{

			connect(&workerProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(receiveResponse()));
			connect(&workerProcess, SIGNAL(started()), this, SLOT(workerProcessStarted()));
			connect(&workerProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(workerProcessExited(int,QProcess::ExitStatus)));
			connect(&workerProcess, SIGNAL(bytesWritten(qint64)), this, SLOT(dbg_bytesWritten(qint64)));
		}

		void CoreServiceHandler::sendToCoreService(QVariant request)
		{
			this->waitingRequests.enqueue(QVariant::fromValue(request));
			sendRequests();
		}

		bool CoreServiceHandler::isRunning()
		{
			return (workerProcess.state() != QProcess::NotRunning);
		}

		void CoreServiceHandler::exit()
		{
			ExitParams request;
			askedToQuit = true;
			sendToCoreService(QVariant::fromValue(request));
		}

		void CoreServiceHandler::receiveSudoPassword(QSharedPointer<QByteArray> password)
		{
			// No need for password then
			if(processInitialized)
				return;

			workerProcess.write(*password);
			workerProcess.write("\n");
			password->fill('\0');
			QTimer::singleShot(1000, this, SLOT(checkInitReceived()));
		}

		void CoreServiceHandler::receiveResponse()
		{
			QVariant response;

			workerProcessStream.startTransaction();
			workerProcessStream >> response;
			if(!workerProcessStream.commitTransaction())
				return;
			if(response.canConvert<InitResponse>()) {
					qDebug() << "Worker process initialized";
					processInitialized = true;
					sendRequests();
					return;
			}
			qDebug() << "Receiving reseponse: " << response.typeName();
			emit sendResponse(response);
		}

		void CoreServiceHandler::sendRequests()
		{
			if(workerProcess.state() != QProcess::Running)
				return startWorkerProcess();

			while(!waitingRequests.isEmpty())
				workerProcessStream << waitingRequests.dequeue();
			qDebug() << "Requests sent";
		}

		void CoreServiceHandler::workerProcessExited(int exitCode, QProcess::ExitStatus exitStatus)
		{
			(void)exitCode;
			QMetaEnum metaEnum = QMetaEnum::fromType<QProcess::ExitStatus>();
			qDebug() << "Worker process finished:" << metaEnum.valueToKey(exitStatus);
			if(askedToQuit) {
				emit exited();
			} else if (!processInitialized) {
				throw IncorrectSudoPasswordException();
			} else {
				throw WorkerProcessCrashedException();
			}
		}

		void CoreServiceHandler::dbg_bytesWritten(qint64 bytes)
		{
			qDebug() << bytes << " bytes written";
		}

		void CoreServiceHandler::workerProcessStarted()
		{
			qDebug() << "Worker process (" << workerProcess.processId() << ") started.";
			processInitialized = false;
			//Sudo might already have the password in cache
			emit askSudoPassword();
		}

		void CoreServiceHandler::checkInitReceived()
		{
			if(processInitialized || (workerProcess.state() == QProcess::NotRunning))
				return;
			emit askSudoPassword();
		}

		void CoreServiceHandler::startWorkerProcess()
		{
			QStringList args;
			askedToQuit = false;
			if(workerProcess.state() == QProcess::Starting)
				return;

			qDebug() << "Start worker process";
			workerProcessStream.setDevice(&workerProcess);
			//*
			workerProcess.setProgram("/usr/bin/sudo");
			args << "-k" << "-S" << "-p" << "" << QCoreApplication::applicationFilePath() << "coreservice";
			//*/
			/*
			workerProcess.setProgram(QCoreApplication::applicationFilePath());
			args << "coreservice";
			//*/
			workerProcess.setArguments(args);
			workerProcess.setProcessChannelMode(QProcess::ForwardedErrorChannel);
			workerProcess.start();
		}
	}
}
