#include "Service.h"
#include <QFile>
#include <QThread>

namespace GostCrypt {
	namespace Core {

	Service::Service(QString serviceName) : serviceName(serviceName)
	{
		ServiceApplication::setApplicationName(serviceName);
	}

	int Service::start(int argc, char **argv)
		{
            QString inputFilePath;
            ServiceApplication app(argc, argv);

			INIT_SERIALIZE(InitResponse);
            INIT_SERIALIZE(ExceptionResponse);
            INIT_SERIALIZE(ExitRequest);
            INIT_SERIALIZE(UnknowRequest);
            INIT_SERIALIZE(UnknowResponse);
            INIT_SERIALIZE(ProgressUpdateResponse);


			initSerializables();

            if(argc > 2) {
                inputFilePath = QString(argv[2]);
            }

            // Creation of input and output streams for communication with parent process
			inputStream.setDevice(&inputFile);
			outputStream.setDevice(&outputFile);
            if(inputFilePath.isEmpty()) {
                inputFile.open(stdin, QFile::ReadOnly);
            } else {
                inputFile.setFileName(inputFilePath);
                inputFile.open(QIODevice::ReadOnly);
            }
            outputFile.open(stdout, QFile::WriteOnly);

			// connecting signals
            connect(&app, SIGNAL(exceptionCaught(GostCryptException&)), this, SLOT(sendException(GostCryptException&)));
			connect(this, SIGNAL(exit()), &app, SLOT(quit()));

			connectRequestHandlingSignals();

			// Service ready to handle requests, ending InitResponse
			#ifdef DEBUG_SERVICE_HANDLER
			qDebug() << "Sending Init Request";
			#endif


			InitResponse init;
			sendResponse(QVariant::fromValue(init));

			// Handling requests
			if(serviceName == "CoreService") {
				inputFileMonitor.reset(new QSocketNotifier(inputFile.handle(), QSocketNotifier::Read));
				connect(inputFileMonitor.data(), SIGNAL(activated(int)), this, SLOT(receiveRequest()));
                qDebug() <<"start async";
                return app.exec();
			}

			try {
				while(receiveRequest());
            } catch(GostCrypt::GostCryptException &e) {
				emit sendException(e);
			} catch (QException &e) {
				qDebug() << e.what();
			}

			return 0;
		}

		bool Service::receiveRequest()
		{
			QVariant request;

			/* Deserialize request */
			#ifdef DEBUG_SERVICE_HANDLER
			qDebug() << "Reading...";
			#endif
			inputStream.startTransaction();
			inputStream >> request;
			if(!inputStream.commitTransaction()) {
				#ifdef DEBUG_SERVICE_HANDLER
				qDebug() << "Not enough data";
				#endif
				QThread::msleep(100);
				return true;
			}

			#ifdef DEBUG_SERVICE_HANDLER
			qDebug() << "Handling request";
			#endif
			if(request.canConvert<ExitRequest>()) {
				#ifdef DEBUG_SERVICE_HANDLER
				qDebug() << "Received Exit Request";
				#endif
				emit askExit();
				return false;
			}

			emit this->request(request);

            return true;
		}

		void Service::sendResponse(QVariant r)
		{
				#ifdef DEBUG_SERVICE_HANDLER
				qDebug() << "Sending:" << r.typeName();
				#endif
				outputStream << r;
				outputFile.flush();
		}

		void Service::sendException(GostCryptException &e)
		{
			ExceptionResponse r;
			r.exception = e.toQVariant();
			sendResponse(QVariant::fromValue(r));
		}

        void Service::sendProgressUpdate(quint32 requestId, qreal progress)
		{
            QSharedPointer<ProgressUpdateResponse> response(new ProgressUpdateResponse(requestId, progress));
            sendResponse(QVariant::fromValue(response));
		}

		bool ServiceApplication::notify(QObject *receiver, QEvent *event)
		{
			bool done = true;
			try {
				done = QCoreApplication::notify(receiver, event);
            } catch(GostCrypt::GostCryptException &e) {
				#ifdef DEBUG_SERVICE_HANDLER
				qDebug() << "Exception catched, forwarding to parent process";
				#endif
                emit exceptionCaught(e);
			} catch (QException &e) {
				qDebug() << e.what();
			}
			return done;
		}

		DEF_SERIALIZABLE(InitResponse)
        QDataStream & operator << (QDataStream & out, const InitResponse& Valeur) {
            (void)Valeur;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, InitResponse & Valeur) {
            (void)Valeur;
            return in;
        }

        DEF_SERIALIZABLE(ExceptionResponse)
        QDataStream & operator << (QDataStream & out, const ExceptionResponse& Valeur) {
            out << Valeur.exception;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, ExceptionResponse & Valeur) {
            in >> Valeur.exception;
            return in;
        }

        DEF_SERIALIZABLE(ExitRequest)
        QDataStream & operator << (QDataStream & out, const ExitRequest & Valeur) {
            (void)Valeur;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, ExitRequest & Valeur) {
            (void)Valeur;
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::UnknowRequest)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::UnknowRequest & Valeur) {
            out << static_cast<const GostCryptException&>(Valeur);
            out << Valeur.requestTypeName;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::UnknowRequest & Valeur) {
            in >> static_cast<GostCryptException&>(Valeur);
            in >> Valeur.requestTypeName;
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::UnknowResponse)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::UnknowResponse & Valeur) {
            out << static_cast<const GostCryptException&>(Valeur);
            out << Valeur.responseTypeName;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::UnknowResponse & Valeur) {
            in >> static_cast<GostCryptException&>(Valeur);
            in >> Valeur.responseTypeName;
            return in;
        }

        DEF_SERIALIZABLE(ProgressUpdateResponse)
        QDataStream & operator << (QDataStream & out, const ProgressUpdateResponse& Valeur) {
          out << static_cast<const CoreResponse&>(Valeur);
          out << Valeur.progress;
          out << Valeur.requestId;
          return out;
        }
        QDataStream & operator >> (QDataStream & in, ProgressUpdateResponse & Valeur) {
          in >> static_cast<CoreResponse&>(Valeur);
          in >> Valeur.progress;
          in >> Valeur.requestId;
          return in;
        }

	}
}

