#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <QCoreApplication>
#include <QDataStream>
#include <QFile>
#include <QVariant>
#include <QSocketNotifier>
#include "CoreException.h"
#include "FuseService/FuseException.h"
#include "CoreResponse.h"

//#define DEBUG_SERVICE_HANDLER

namespace GostCrypt {
    namespace Core {

		struct ProgressUpdateResponse;
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
			void sendProgressUpdate(QSharedPointer<ProgressUpdateResponse> response);
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

        #define UnknowRequestException(requestTypeName) GostCrypt::Core::UnknowRequest(__PRETTY_FUNCTION__, __FILE__, __LINE__, requestTypeName);
        class UnknowRequest : public GostCryptException {
            public:
                UnknowRequest() {}
                UnknowRequest(QString fonction, QString filename, quint32 line, const char *requestTypeName) : GostCryptException(fonction, filename, line), requestTypeName(requestTypeName) {}
                DEF_EXCEPTION_WHAT(UnknowRequest, GostCryptException, "Child process received an unknown request (" + requestTypeName + ")")
            protected:
            QString requestTypeName;
            DEC_SERIALIZABLE(UnknowRequest);
        };

        #define UnknowResponseException(requestTypeName) GostCrypt::Core::UnknowResponse(__PRETTY_FUNCTION__, __FILE__, __LINE__, requestTypeName);
        class UnknowResponse : public GostCryptException {
            public:
                UnknowResponse() {}
                UnknowResponse(QString fonction, QString filename, quint32 line, const char *responseTypeName) : GostCryptException(fonction, filename, line), responseTypeName(responseTypeName) {}
                DEF_EXCEPTION_WHAT(UnknowResponse, GostCryptException, "Unknow reponse received from child process (" + responseTypeName + ")")
            protected:
            QString responseTypeName;
            DEC_SERIALIZABLE(UnknowResponse);
        };

        struct ProgressUpdateResponse : CoreResponse {
            ProgressUpdateResponse() {}
            ProgressUpdateResponse(quint32 requestId, qreal progress) : requestId(requestId), progress(progress) {}
            qint32 requestId;
            qreal progress;
            DEC_SERIALIZABLE(ProgressUpdateResponse);
        };
	}
}

SERIALIZABLE(GostCrypt::Core::InitResponse)
SERIALIZABLE(GostCrypt::Core::ExceptionResponse)
SERIALIZABLE(GostCrypt::Core::ExitRequest)
SERIALIZABLE(GostCrypt::Core::UnknowRequest)
SERIALIZABLE(GostCrypt::Core::UnknowResponse)
SERIALIZABLE(GostCrypt::Core::ProgressUpdateResponse)



#endif // SERVICE_H
