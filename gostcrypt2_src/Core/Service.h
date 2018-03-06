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
        /**
         * @brief Abstract base class for classes corresponding to Service process. A Service process is a process receiving request (as serialized objects) on its standard input and writing responses to these requests on its standard output.
         *
         */
        class Service : public QObject
		{
			Q_OBJECT
		public:
            /**
             * @brief Constructor setting the Qt application name to serviceName
             *
             * @param serviceName Name of the Qt application for this process
             */
            explicit Service(QString serviceName);
            /**
             * @brief Start the service. Equivalent to the main function of a program.
             *
             * @param argc Number of strings in argv
             * @param argv Command line arguments with which the program was launched
             * @return int Exit code of the program
             */
            int start(int argc, char **argv);
		private:
            QFile outputFile; /**< QFile in which the responses are serialized. Correspond to the standard output. */
            QFile inputFile; /**< QFile from which the requests are deserialized. Corresponds to the standard input except if a file name is provided in command line parameters. */
            QDataStream inputStream; /**< QDataStream corresponding to the inputFile used to deserialize requests */
            QDataStream outputStream; /**< QDataStream corresponding to the outputFile used t serialize responses */
            QSharedPointer<QSocketNotifier> inputFileMonitor; /**< Pointer to QSocketNotifier used in async mode to detect when there is new data to read in the inputFile */
            QString serviceName; /**< Name of this service which is also the name of the Qt application */
		protected:
            /**
             * @brief Send the given response to the parent process by serializing it in outputFile
             *
             * @param r A QVariant storing the response to send
             */
            void sendResponse(QVariant r);
            /**
             * @brief Connect the necessarry the service signals to the slots in charge
             * The signals request and askExit need to be connected.
             */
            virtual void connectRequestHandlingSignals() = 0;
            /**
             * @brief Initialize all the objects that need to be received or transmitted to the parent process
             *
             */
            virtual void initSerializables() = 0;
		private slots:
            /**
             * @brief Send the given exception to the parent process. This exception will be rethrown in the prent process.
             *
             * @param e Exception to send
             */
            void sendException(GostCryptException &e);
            /**
             * @brief Check if new requests have been sent and process them
             *
             * @return False if the ExitRequest is received and the service should exit
             */
            bool receiveRequest();
            /**
             * @brief Send the current progress in the processing of a request to the parent process
             *
             * @param requestId Request Identifier given by the User Interface module
             * @param progress Progress of the request processing time between 0 and 1
             */
            void sendProgressUpdate(quint32 requestId, qreal progress);
        signals:
            /**
             * @brief Signal emitted when a request is received
             *
             * @param request QVariant containing the request
             */
            void request(QVariant request);
            /**
             * @brief Signal emitted when the parent process asked this service process to exit
             *
             */
            void askExit();
            /**
             * @brief Signal that should be emitted when this process is ready to exit
             *
             */
            void exit();
		};

		// redefines the notify function of QCoreApplication to catch all exceptions at once
        /**
         * @brief Class used instead of QCoreApplication for this process in order to intercept GostCrypt exceptions thrown from slots
         *
         */
        class ServiceApplication : public QCoreApplication {
			Q_OBJECT
			public:
				ServiceApplication(int& argc, char** argv) : QCoreApplication(argc, argv) {}
                /**
                 * @brief Wrapper intercepting all exceptions thrown inside QCoreApplication::notify
                 */
                bool notify(QObject* receiver, QEvent* event);
			signals:
                /**
                 * @brief Signal emitted when an exception is caught
                 *
                 * @param e GostCrypt exception caught
                 */
                void exceptionCaught(GostCryptException &e);
		};

        /**
         * @brief Empty Response sent to the parent process as soon as the service process is ready to accept requests
         *
         */
        struct InitResponse {
			DEC_SERIALIZABLE(InitResponse);
		};

        /**
         * @brief Response sent to the parent process when an exception occur while processing a request in order to inform it about the exception
         *
         */
        struct ExceptionResponse {
            QVariant exception; /**< QVariant containing the exception that occurred */
			DEC_SERIALIZABLE(ExceptionResponse);
		};

        /**
         * @brief Request sent to the service process by the parent to ask the service process to exit
         *
         */
        struct ExitRequest {
            DEC_SERIALIZABLE(ExitRequest);
        };

        #define UnknowRequestException(requestTypeName) GostCrypt::Core::UnknowRequest(__PRETTY_FUNCTION__, __FILE__, __LINE__, requestTypeName);
        /**
         * @brief Exception thrown when the request can not be processed because the sevice process can not recognise its type
         *
         */
        class UnknowRequest : public GostCryptException {
            public:
                /**
                 * @brief Default constructor used when deserializing
                 *
                 */
                UnknowRequest() {}
                /**
                * @brief Constructor used when throwing the exception
                *
                * @param fonction Name of the function in which the exception is thrown
                * @param filename Name of the file where the exception is thrown
                * @param line Line where the exception is thrown
                * @param requestTypeName Name of the unrecognised request
                */
                UnknowRequest(QString fonction, QString filename, quint32 line, const char *requestTypeName) : GostCryptException(fonction, filename, line), requestTypeName(requestTypeName) {}
                DEF_EXCEPTION_WHAT(UnknowRequest, GostCryptException, "Child process received an unknown request (" + requestTypeName + ")")
            protected:
            QString requestTypeName; /**< Name of the unrecognised request */
            DEC_SERIALIZABLE(UnknowRequest);
        };

        #define UnknowResponseException(requestTypeName) GostCrypt::Core::UnknowResponse(__PRETTY_FUNCTION__, __FILE__, __LINE__, requestTypeName);
         /**
         * @brief Exception thrown when the response can not be understood because the parent process can not recognise its type
         *
         */
        class UnknowResponse : public GostCryptException {
            public:
                /**
                 * @brief Default constructor used when deserializing
                 *
                 */
                UnknowResponse() {}
                /**
                * @brief Constructor used when throwing the exception
                *
                * @param fonction Name of the function in which the exception is thrown
                * @param filename Name of the file where the exception is thrown
                * @param line Line where the exception is thrown
                * @param responseTypeName Name of the unrecognised response
                */
                UnknowResponse(QString fonction, QString filename, quint32 line, const char *responseTypeName) : GostCryptException(fonction, filename, line), responseTypeName(responseTypeName) {}
                DEF_EXCEPTION_WHAT(UnknowResponse, GostCryptException, "Unknow reponse received from child process (" + responseTypeName + ")")
            protected:
            QString responseTypeName; /**< Name of the unrecognised response */
            DEC_SERIALIZABLE(UnknowResponse);
        };

        /**
         * @brief Response sent to the parent process to inform it about the progress of a request processing
         *
         */
        struct ProgressUpdateResponse : CoreResponse {
            /**
             * @brief Default constructor used when deserializing
             *
             */
            ProgressUpdateResponse() {}
            /**
             * @brief Constructor
             *
             * @param requestId Identifier of the request for which we are reporting the progress
             * @param progress Current progress of the request processing
             */
            ProgressUpdateResponse(quint32 requestId, qreal progress) : requestId(requestId), progress(progress) {}
            qint32 requestId; /**< Identifier of the request for which we are reporting the progress */
            qreal progress; /**< Progress of the request processing */
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
