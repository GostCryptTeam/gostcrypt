#include "CoreService.h"
#include "CoreResponse.h"
#include "CoreParams.h"
#include <QFile>
#include <QThread>

namespace GostCrypt {
    namespace NewCore {
		int CoreService::start(int argc, char **argv)
		{
			CoreServiceApplication app(argc, argv);

			initCoreParams();
			initCoreResponse();
			initCoreException();

			CoreServiceApplication::setApplicationName("coreservice");
			#ifdef DEBUG_CORESERVICE_HANDLER
			qDebug() << "CoreService started";
			#endif

			inputStream.setDevice(&inputFile);
			outputStream.setDevice(&outputFile);
			inputFile.open(stdin, QFile::ReadOnly);
			outputFile.open(stdout, QFile::WriteOnly);

			/* For each core request */
			CONNECT_RESPONSE_SLOT(CreateVolume);
			CONNECT_RESPONSE_SLOT(MountVolume);
			CONNECT_RESPONSE_SLOT(DismountVolume);
			CONNECT_RESPONSE_SLOT(ChangeVolumePassword);
			CONNECT_RESPONSE_SLOT(GetEncryptionAlgorithms);
			CONNECT_RESPONSE_SLOT(GetDerivationFunctions);
			CONNECT_RESPONSE_SLOT(GetHostDevices);
			CONNECT_RESPONSE_SLOT(GetMountedVolumes);
			CONNECT_RESPONSE_SLOT(CreateKeyFile);

			app.connect(this, SIGNAL(exit()), &core, SLOT(exit()));
			app.connect(&app, SIGNAL(exit()), &core, SLOT(exit()));
			app.connect(&app, SIGNAL(sendException(CoreException&)), this, SLOT(sendException(CoreException&)));
			app.connect(this, SIGNAL(request(QVariant)), &core, SLOT(request(QVariant)));

			#ifdef DEBUG_CORESERVICE_HANDLER
			qDebug() << "Sending Init Request";
			#endif
			InitResponse init;
			sendResponse(QVariant::fromValue(init));

			while(receiveRequest());

			// No need for app.exec() since all signals use direct connection
			return 0;
		}

		bool CoreService::receiveRequest()
		{
			QVariant request;

			/* Deserialize request */
			#ifdef DEBUG_CORESERVICE_HANDLER
			qDebug() << "Reading...";
			#endif
			inputStream.startTransaction();
			inputStream >> request;
			if(!inputStream.commitTransaction()) {
				#ifdef DEBUG_CORESERVICE_HANDLER
				qDebug() << "Not enough data";
				#endif
				QThread::msleep(100);
				return true;
			}

			#ifdef DEBUG_CORESERVICE_HANDLER
			qDebug() << "Handling request";
			#endif
			if(request.canConvert<ExitParams>()) {
				#ifdef DEBUG_CORESERVICE_HANDLER
				qDebug() << "Received Exit Request";
				#endif
				emit exit();
				return false;
			}

			try {
				emit this->request(request);
			} catch(GostCrypt::NewCore::CoreException &e) {
				sendException(e);
			}

			return true;
		}

		void CoreService::sendResponse(QVariant response)
		{
			#ifdef DEBUG_CORESERVICE_HANDLER
			qDebug() << "Sending:" << response.typeName();
			#endif
			outputStream << response;
			outputFile.flush();
		}

		void CoreService::sendException(CoreException &e)
		{
			ExceptionResponse r;
			sendResponse(QVariant::fromValue(r));
			sendResponse(e.toQVariant());
		}

		bool CoreServiceApplication::notify(QObject *receiver, QEvent *event)
		{
			    bool done = true;
				try {
					done = QCoreApplication::notify(receiver, event);
				} catch(GostCrypt::NewCore::CoreException &e) {
					emit sendException(e);
				} catch (QException &e) {
					qDebug() << e.what();
					emit exit();
				}
				return done;
		}

	}
}

