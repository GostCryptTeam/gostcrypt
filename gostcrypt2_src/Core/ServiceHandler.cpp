#include "ServiceHandler.h"
#include <QProcess>
#include <QMetaEnum>
#include <QTimer>
#include <QDebug>
#include "CoreException.h"
#include "FuseService/FuseException.h"
#include "Service.h"

namespace GostCrypt {
    namespace Core {

        ServiceHandler::ServiceHandler(QString programName, QStringList args) : programPath(programName), args(args)
		{

			INIT_SERIALIZE(InitResponse);
			INIT_SERIALIZE(ExceptionResponse);
			INIT_SERIALIZE(ExitRequest);
			INIT_SERIALIZE(UnknowRequest);
			INIT_SERIALIZE(UnknowResponse);

			connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(receive()));
            connect(this, SIGNAL(sendResponse(QVariant&)), this, SLOT(receive()));
			connect(&process, SIGNAL(started()), this, SLOT(processStarted()));
            connect(&process, SIGNAL(started()), this, SIGNAL(started()));
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
#ifdef GENERATE_REQUESTS_DUMP
            requestDumpFile.close();
#endif
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
			if(!processStream.commitTransaction()) {
				processStream.resetStatus();
				return;
			}

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
                    const GostCrypt::GostCryptException *exceptionPtr = reinterpret_cast<const GostCrypt::GostCryptException*>(er.exception.constData());

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
		}

		void ServiceHandler::sendRequests()
		{
			if(process.state() != QProcess::Running)
				return startProcess();

            while(!waitingRequests.isEmpty()) {
#ifdef GENERATE_REQUESTS_DUMP
                requestsDumpStream << waitingRequests.head();
                requestDumpFile.flush();
#endif
                processStream << waitingRequests.dequeue();
            }
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
                throw ServiceProcessCrashedException();
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
		}

		bool ServiceHandler::isInitialized()
		{
            return processInitialized;
        }

		void ServiceHandler::processCrashedBeforeInitialization()
		{
            throw ServiceProcessCrashedException();
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
            process.setProgram(programPath);
			process.setArguments(args);
			#ifdef DEBUG_SERVICE_HANDLER
			process.setProcessChannelMode(QProcess::ForwardedErrorChannel);
			#else
			process.setProcessChannelMode(QProcess::SeparateChannels);
			process.setReadChannel(QProcess::StandardOutput);
			#endif
			process.start();

#ifdef GENERATE_REQUESTS_DUMP
            requestsDumpStream.setDevice(&requestDumpFile);
            requestDumpFile.setFileName("requestsDump_"+ args.last());
            requestDumpFile.open(QIODevice::WriteOnly);
#endif
		}

	}
}
