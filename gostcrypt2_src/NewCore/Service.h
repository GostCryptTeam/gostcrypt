#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <QCoreApplication>
#include <QDataStream>
#include <QFile>
#include <QVariant>
#include <QSocketNotifier>
#include "CoreException.h"
#include "NewFuseService/FuseException.h"
#include "CoreResponse.h"

//#define DEBUG_SERVICE_HANDLER

namespace GostCrypt {
    namespace NewCore {
		class Service : public QObject
		{
			Q_OBJECT
		public:
			Service(QString serviceName);
			int start(int argc, char **argv);
		private:
			QFile outputFile;
			QFile inputFile;
			QDataStream inputStream;
			QDataStream outputStream;
			QSharedPointer<QSocketNotifier> inputFileMonitor;
			QString serviceName;
		protected:
			void sendResponse(QVariant r);
			virtual void connectRequestHandlingSignals() = 0;
			virtual void initSerializables() = 0;
		private slots:
			void sendException(GostCryptException &e);
			bool receiveRequest();
		signals:
			void request(QVariant request);
			void askExit();
			void exit();
		};

		// redefines the notify function of QCoreApplication to catch all exceptions at once
		class ServiceApplication : public QCoreApplication {
			Q_OBJECT
			public:
				ServiceApplication(int& argc, char** argv) : QCoreApplication(argc, argv) {}
				bool notify(QObject* receiver, QEvent* event);
			signals:
				void sendException(GostCryptException &e);
		};

		struct InitResponse {
			DEC_SERIALIZABLE(InitResponse);
		};

		struct ExceptionResponse {
			QVariant exception;
			DEC_SERIALIZABLE(ExceptionResponse);
		};

		struct ExitRequest {
            DEC_SERIALIZABLE(ExitRequest);
        }; // no parameters

        #define UnknowRequestException(requestTypeName) GostCrypt::NewCore::UnknowRequest(__PRETTY_FUNCTION__, __FILE__, __LINE__, requestTypeName);
        class UnknowRequest : public GostCryptException {
            public:
                UnknowRequest() {}
                UnknowRequest(QString fonction, QString filename, quint32 line, const char *requestTypeName) : GostCryptException(fonction, filename, line), requestTypeName(requestTypeName) {}
                DEF_EXCEPTION_WHAT(UnknowRequest, GostCryptException, "Child process received an unknown request (" + requestTypeName + ")")
            protected:
            QString requestTypeName;
            DEC_SERIALIZABLE(UnknowRequest);
        };

        #define UnknowResponseException(requestTypeName) GostCrypt::NewCore::UnknowResponse(__PRETTY_FUNCTION__, __FILE__, __LINE__, requestTypeName);
        class UnknowResponse : public GostCryptException {
            public:
                UnknowResponse() {}
                UnknowResponse(QString fonction, QString filename, quint32 line, const char *responseTypeName) : GostCryptException(fonction, filename, line), responseTypeName(responseTypeName) {}
                DEF_EXCEPTION_WHAT(UnknowResponse, GostCryptException, "Unknow reponse received from child process (" + responseTypeName + ")")
            protected:
            QString responseTypeName;
            DEC_SERIALIZABLE(UnknowResponse);
        };
	}
}

SERIALIZABLE(GostCrypt::NewCore::InitResponse)
SERIALIZABLE(GostCrypt::NewCore::ExceptionResponse)
SERIALIZABLE(GostCrypt::NewCore::ExitRequest)
SERIALIZABLE(GostCrypt::NewCore::UnknowRequest)
SERIALIZABLE(GostCrypt::NewCore::UnknowResponse)


#endif // SERVICE_H
