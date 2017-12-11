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
         * @brief Class defining the process which are requested in standart entry
         *
         */
        class Service : public QObject
		{
			Q_OBJECT
		public:
            explicit Service(QString serviceName);
            /**
             * @brief start the service
             *
             * @param argc
             * @param argv
             * @return int
             */
            int start(int argc, char **argv);
		private:
            QFile outputFile; /**< TODO: describe */
            QFile inputFile; /**< TODO: describe */
            QString inputFilePath;
            QDataStream inputStream; /**< TODO: describe */
            QDataStream outputStream; /**< TODO: describe */
            QSharedPointer<QSocketNotifier> inputFileMonitor; /**< TODO: describe */
            QString serviceName; /**< TODO: describe */
		protected:
            /**
             * @brief send a response to it Parent process (with requested informations ) when it finished
             *
             * @param r QVariant the response given to the parent
             */
            void sendResponse(QVariant r);
            /**
             * @brief
             *
             */
            virtual void connectRequestHandlingSignals() = 0;
            /**
             * @brief
             *
             */
            virtual void initSerializables() = 0;
		private slots:
            /**
             * @brief send a exception to it Parent process when it finished
             *
             * @param e GostCryptException the exception given to the parent
             */
            void sendException(GostCryptException &e);
            /**
             * @brief check if a request is received
             *
             * @return bool
             */
            bool receiveRequest();
            /**
             * @brief send the progression to it Parent
             *
             * @param response
             */
            void sendProgressUpdate(QSharedPointer<ProgressUpdateResponse> response);
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
