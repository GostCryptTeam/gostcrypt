#include "CoreServiceHandler.h"
#include "CoreResponse.h"
#include "CoreRequest.h"
#include <QProcess>
#include <QMetaEnum>
#include <QTimer>
#include <QDebug>
#include "CoreException.h"

//#define DEBUG_CORESERVICE_HANDLER

namespace GostCrypt {
    namespace NewCore {

		CoreServiceHandler::CoreServiceHandler()
		{
			connect(&workerProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(receive()));
			connect(&workerProcess, SIGNAL(started()), this, SLOT(workerProcessStarted()));
			connect(&workerProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(workerProcessExited(int,QProcess::ExitStatus)));
			#ifdef DEBUG_CORESERVICE_HANDLER
			connect(&workerProcess, SIGNAL(bytesWritten(qint64)), this, SLOT(dbg_bytesWritten(qint64)));
			#endif
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
			ExitRequest request;
			askedToQuit = true;
			#ifdef DEBUG_CORESERVICE_HANDLER
			qDebug() << "Sending exit request";
			#endif
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

		void CoreServiceHandler::receive()
		{
			QVariant v;

			workerProcessStream.startTransaction();
			workerProcessStream >> v;
			if(!workerProcessStream.commitTransaction())
				return;

			if(v.canConvert<InitResponse>()) {
					#ifdef DEBUG_CORESERVICE_HANDLER
					qDebug() << "Worker process initialized";
					#endif
					processInitialized = true;
					sendRequests();
					return;
			}

			if(v.canConvert<ExceptionResponse>()) {
					ExceptionResponse er = v.value<ExceptionResponse>();

					const GostCrypt::NewCore::CoreException *exceptionPtr = reinterpret_cast<const GostCrypt::NewCore::CoreException*>(er.exception.constData());
					#ifdef DEBUG_CORESERVICE_HANDLER
					qDebug() << "Exception occured";
					qDebug().noquote() << exceptionPtr->qwhat();
					#endif
					exceptionPtr->raise();
					return;
			}

			#ifdef DEBUG_CORESERVICE_HANDLER
			qDebug() << "Receiving response: " << v.typeName();
			#endif
			emit sendResponse(v);
		}

		void CoreServiceHandler::sendRequests()
		{
			if(workerProcess.state() != QProcess::Running)
				return startWorkerProcess();

			while(!waitingRequests.isEmpty())
				workerProcessStream << waitingRequests.dequeue();
			#ifdef DEBUG_CORESERVICE_HANDLER
			qDebug() << "Requests sent";
			#endif
		}

		void CoreServiceHandler::workerProcessExited(int exitCode, QProcess::ExitStatus exitStatus)
		{
			(void)exitCode;
			#ifdef DEBUG_CORESERVICE_HANDLER
			QMetaEnum metaEnum = QMetaEnum::fromType<QProcess::ExitStatus>();
			qDebug() << "Worker process finished:" << metaEnum.valueToKey(exitStatus);
			#else
			(void)exitStatus;
			#endif
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
			#ifdef DEBUG_CORESERVICE_HANDLER
			qDebug() << "Worker process (" << workerProcess.processId() << ") started.";
			#endif
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

			#ifdef DEBUG_CORESERVICE_HANDLER
			qDebug() << "Start worker process";
			#endif
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
			#ifdef DEBUG_CORESERVICE_HANDLER
			workerProcess.setProcessChannelMode(QProcess::ForwardedErrorChannel);
			#else
			workerProcess.setProcessChannelMode(QProcess::SeparateChannels);
			workerProcess.setReadChannel(QProcess::StandardOutput);
			#endif
			workerProcess.start();
		}
	}
}
