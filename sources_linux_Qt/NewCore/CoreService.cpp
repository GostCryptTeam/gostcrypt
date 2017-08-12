#include "CoreService.h"
#include "CoreResponse.h"
#include "CoreParams.h"
#include <QFile>

namespace GostCrypt {
    namespace NewCore {
		int CoreService::start(int argc, char **argv)
		{
			CoreServiceApplication app(argc, argv);

			initCoreParams();
			initCoreResponse();
			initCoreException();

			CoreServiceApplication::setApplicationName("coreservice");
			qDebug() << "CoreService started";

			inputStream.setDevice(&inputFile);
			outputStream.setDevice(&outputFile);
			inputFile.open(stdin, QFile::ReadOnly);
			outputFile.open(stdout, QFile::WriteOnly);

			/* For each core request */
			CONNECT_RESPONSE_SLOT(CreateVolume);
			CONNECT_RESPONSE_SLOT(MountVolume);
			CONNECT_RESPONSE_SLOT(DismountVolume);
			CONNECT_RESPONSE_SLOT(GetMountedVolumes);

			app.connect(this, SIGNAL(exit()), &core, SLOT(exit()));
			app.connect(&app, SIGNAL(exit()), &core, SLOT(exit()));
			app.connect(this, SIGNAL(request(QVariant)), &core, SLOT(request(QVariant)));

			qDebug() << "Sending Init Request";
			InitResponse init;
			sendResponse(QVariant::fromValue(init));

			try {
				while(receiveRequest());
			} catch(GostCrypt::NewCore::CoreException &e) {
				qDebug().noquote() << e.qwhat();
				return -1;
			} catch (QException &e) { // TODO : handle exceptions here
				qDebug() << e.what();
				return -1;
			}

			// No need for app.exec() since all signals use direct connection
			return 0;
		}

		bool CoreService::receiveRequest()
		{
			QVariant request;

			/* Deserialize request */
			qDebug() << "Reading...";
			inputStream.startTransaction();
			inputStream >> request;
			if(!inputStream.commitTransaction()) {
				qDebug() << "Not enough data";
				return true;
			}

			qDebug() << "Handling request";
			if(request.canConvert<ExitParams>()) {
				qDebug() << "Received Exit Request";
				emit exit();
				return false;
			}
			emit this->request(request);
			return true;
		}

		void CoreService::sendResponse(QVariant response)
		{
			outputStream << response;
			outputFile.flush();
		}

		bool CoreServiceApplication::notify(QObject *receiver, QEvent *event)
		{
			    bool done = true;
				try {
					done = QCoreApplication::notify(receiver, event);
				} catch(GostCrypt::NewCore::CoreException &e) {
					qDebug().noquote() << e.qwhat();
					emit exit();
				} catch (QException &e) { // TODO : handle exceptions here
					qDebug() << e.what();
					emit exit();
				}
				return done;
		}

	}
}

