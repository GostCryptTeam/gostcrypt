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
             * @brief
             *
             * @param request QVariant
             */
            void request(QVariant request);
            /**
             * @brief
             *
             */
            void askExit();
            /**
             * @brief
             *
             */
            void exit();
		};

		// redefines the notify function of QCoreApplication to catch all exceptions at once
        /**
         * @brief
         *
         */
        class ServiceApplication : public QCoreApplication {
			Q_OBJECT
			public:
				ServiceApplication(int& argc, char** argv) : QCoreApplication(argc, argv) {}
                /**
                 * @brief
                 *
                 * @param receiver
                 * @param event
                 * @return bool
                 */
                bool notify(QObject* receiver, QEvent* event);
			signals:
                /**
                 * @brief
                 *
                 * @param e
                 */
                void sendException(GostCryptException &e);
		};

        /**
         * @brief
         *
         */
        struct InitResponse {
			DEC_SERIALIZABLE(InitResponse);
		};

        /**
         * @brief
         *
         */
        struct ExceptionResponse {
            QVariant exception; /**< TODO: describe */
			DEC_SERIALIZABLE(ExceptionResponse);
		};

        /**
         * @brief
         *
         */
        struct ExitRequest {
            DEC_SERIALIZABLE(ExitRequest);
        }; // no parameters

        #define UnknowRequestException(requestTypeName) GostCrypt::Core::UnknowRequest(__PRETTY_FUNCTION__, __FILE__, __LINE__, requestTypeName);
        /**
         * @brief
         *
         */
        class UnknowRequest : public GostCryptException {
            public:
                /**
                 * @brief
                 *
                 */
                UnknowRequest() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param requestTypeName
                 */
                UnknowRequest(QString fonction, QString filename, quint32 line, const char *requestTypeName) : GostCryptException(fonction, filename, line), requestTypeName(requestTypeName) {}
                DEF_EXCEPTION_WHAT(UnknowRequest, GostCryptException, "Child process received an unknown request (" + requestTypeName + ")")
            protected:
            QString requestTypeName; /**< TODO: describe */
            DEC_SERIALIZABLE(UnknowRequest);
        };

        #define UnknowResponseException(requestTypeName) GostCrypt::Core::UnknowResponse(__PRETTY_FUNCTION__, __FILE__, __LINE__, requestTypeName);
         /**
         * @brief
         *
         */
        class UnknowResponse : public GostCryptException {
            public:
                /**
                 * @brief
                 *
                 */
                UnknowResponse() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param responseTypeName
                 */
                UnknowResponse(QString fonction, QString filename, quint32 line, const char *responseTypeName) : GostCryptException(fonction, filename, line), responseTypeName(responseTypeName) {}
                DEF_EXCEPTION_WHAT(UnknowResponse, GostCryptException, "Unknow reponse received from child process (" + responseTypeName + ")")
            protected:
            QString responseTypeName; /**< TODO: describe */
            DEC_SERIALIZABLE(UnknowResponse);
        };

        /**
         * @brief
         *
         */
        struct ProgressUpdateResponse : CoreResponse {
            /**
             * @brief
             *
             */
            ProgressUpdateResponse() {}
            /**
             * @brief
             *
             * @param requestId
             * @param progress
             */
            ProgressUpdateResponse(quint32 requestId, qreal progress) : requestId(requestId), progress(progress) {}
            qint32 requestId; /**< TODO: describe */
            qreal progress; /**< TODO: describe */
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
