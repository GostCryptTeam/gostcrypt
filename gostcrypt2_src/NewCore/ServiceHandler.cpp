#include "ServiceHandler.h"
#include <QProcess>
#include <QMetaEnum>
#include <QTimer>
#include <QDebug>
#include "CoreException.h"
#include "NewFuseService/FuseException.h"
#include "Service.h"

namespace GostCrypt {
    namespace NewCore {

		ServiceHandler::ServiceHandler(QString programName, QStringList args) : programName(programName), args(args)
		{

			INIT_SERIALIZE(InitResponse);
			INIT_SERIALIZE(ExceptionResponse);
			INIT_SERIALIZE(ExitRequest);
			INIT_SERIALIZE(UnknowRequest);
			INIT_SERIALIZE(UnknowResponse);

			connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(receive()));
			connect(this, SIGNAL(responseReaded()), this, SLOT(receive()));
			connect(&process, SIGNAL(started()), this, SLOT(processStarted()));
			connect(&process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processExited(int)));
			#ifdef DEBUG_SERVICE_HANDLER
			connect(&process, SIGNAL(bytesWritten(qint64)), this, SLOT(dbg_bytesWritten(qint64)));
#endif
		}

		void ServiceHandler::sendToService(QVariant request)
		{
			this->waitingRequests.enqueue(QVariant::fromValue(request));
			sendRequests();
		}

		bool ServiceHandler::isRunning()
		{
			return (process.state() != QProcess::NotRunning);
		}

		void ServiceHandler::exit()
		{
			ExitRequest request;

			askedToQuit = true;
			#ifdef DEBUG_SERVICE_HANDLER
			qDebug() << "Sending exit request";
			#endif
			sendToService(QVariant::fromValue(request));
		}

		void ServiceHandler::writeToStdin(QSharedPointer<QByteArray> data, bool newline)
		{
			process.write(*data);
			if(newline) {
				process.write("\n");
			}
		}

		void ServiceHandler::receive()
		{
			QVariant v;

			processStream.startTransaction();
			processStream >> v;
			if(!processStream.commitTransaction())
				return;

			if(v.canConvert<InitResponse>()) {
					#ifdef DEBUG_SERVICE_HANDLER
					qDebug() << "Service process initialized";
					#endif
					processInitialized = true;
					emit initialized();
					sendRequests();
					return;
			}

			if(v.canConvert<ExceptionResponse>()) {
					ExceptionResponse er = v.value<ExceptionResponse>();
					const GostCrypt::NewCore::GostCryptException *exceptionPtr = reinterpret_cast<const GostCrypt::NewCore::GostCryptException*>(er.exception.constData());

					#ifdef DEBUG_SERVICE_HANDLER
					qDebug() << "Exception occured in child service:";
					qDebug().noquote() << exceptionPtr->displayedMessage();
					#endif
					exceptionPtr->raise();
					return;
			}

			#ifdef DEBUG_SERVICE_HANDLER
			qDebug() << "Receiving response: " << v.typeName();
			#endif
			emit sendResponse(v);
			emit responseReaded();
		}

		void ServiceHandler::sendRequests()
		{
			if(process.state() != QProcess::Running)
				return startProcess();

			while(!waitingRequests.isEmpty())
				processStream << waitingRequests.dequeue();
			#ifdef DEBUG_SERVICE_HANDLER
			qDebug() << "Requests sent";
			#endif
		}

		void ServiceHandler::processExited(int exitCode)
		{
			#ifdef DEBUG_SERVICE_HANDLER
			qDebug() << "Service process exited (" + QString::number(exitCode) + ")";
			#else
			Q_UNUSED(exitCode)
			#endif
			if(askedToQuit) {
				emit exited();
			} else if (!processInitialized) {
				processCrashedBeforeInitialization();
			} else {
				throw WorkerProcessCrashedException();
			}
		}

		void ServiceHandler::dbg_bytesWritten(qint64 bytes)
		{
			qDebug() << bytes << " bytes written";
		}

		void ServiceHandler::processStarted()
		{
			#ifdef DEBUG_SERVICE_HANDLER
			qDebug() << "Service process (" << process.processId() << ") started.";
			#endif
			processInitialized = false;
			emit started();
		}

		bool ServiceHandler::isInitialized()
		{
			return (processInitialized || (process.state() == QProcess::NotRunning));
		}

		void ServiceHandler::processCrashedBeforeInitialization()
		{
			throw WorkerProcessCrashedException();
		}

		void ServiceHandler::startProcess()
		{
			askedToQuit = false;
			if(process.state() == QProcess::Starting)
				return;

			#ifdef DEBUG_SERVICE_HANDLER
			qDebug() << "Start service process";
			#endif

			processStream.setDevice(&process);
			process.setProgram(programName);
			process.setArguments(args);
			#ifdef DEBUG_SERVICE_HANDLER
			process.setProcessChannelMode(QProcess::ForwardedErrorChannel);
			#else
			process.setProcessChannelMode(QProcess::SeparateChannels);
			process.setReadChannel(QProcess::StandardOutput);
			//TODO tmp
			process.setProcessChannelMode(QProcess::ForwardedErrorChannel);
			#endif
			process.start();
		}

	}
}
