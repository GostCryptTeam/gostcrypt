#include "CoreServiceHandler.h"
#include "CoreResponse.h"
#include "CoreParams.h"
#include <QProcess>
#include <QMetaEnum>
#include <QTimer>
#include <QException>
#include <QDebug>

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

		void CoreServiceHandler::receiveSudoPassword(QSharedPointer<QByteArray> password)
		{
			workerProcess.write(*password);
			workerProcess.write("\n");
			password->fill('\0');
			if((workerProcess.state() == QProcess::NotRunning) || workerProcess.waitForFinished(3000))
				throw QException(); //TODO throw Incorrect sudo password exception

			QTimer::singleShot(1000, this, SLOT(checkInitReceived()));
		}

		void CoreServiceHandler::receiveResponse()
		{
			QVariant response;

			qDebug() << "Receiving response(s)";
			workerProcessStream.startTransaction();
			workerProcessStream >> response;
			if(!workerProcessStream.commitTransaction())
				return;
			if(response.canConvert<InitResponse>()) {
					qDebug() << "Worker process initialized";
					processInitialized = true;
					sendRequests();
			}

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
			emit workerProcessFinished();
		}

		void CoreServiceHandler::dbg_bytesWritten(qint64 bytes)
		{
			qDebug() << bytes << " bytes written";
		}

		void CoreServiceHandler::workerProcessStarted()
		{
			qDebug() << "Worker process (" << workerProcess.processId() << ") started.";
			processInitialized = false;
			emit askSudoPassword();
		}

		void CoreServiceHandler::checkInitReceived()
		{
			if(processInitialized)
				return;
			emit askSudoPassword();
		}

		void CoreServiceHandler::startWorkerProcess()
		{
			QStringList args;
			if(workerProcess.state() == QProcess::Starting)
				return;

			qDebug() << "Start worker process";
			workerProcessStream.setDevice(&workerProcess);
			workerProcess.setProgram("/usr/bin/sudo");
			args << "-S" << "-p" << "" << QCoreApplication::applicationFilePath() << "coreservice";
			workerProcess.setArguments(args);
			workerProcess.setProcessChannelMode(QProcess::ForwardedErrorChannel);
			workerProcess.start();
		}
	}
}
