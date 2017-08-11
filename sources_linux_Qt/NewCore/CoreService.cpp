#include "CoreService.h"
#include "CoreResponse.h"
#include "CoreParams.h"
#include <QFile>

namespace GostCrypt {
    namespace NewCore {
		int CoreService::start(int argc, char **argv)
		{
			QCoreApplication app(argc, argv);

			initCoreResponse();
			initCoreParams();
			initCoreException();

			a = &app; //should be removed
			QCoreApplication::setApplicationName("coreservice");
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
			app.connect(this, SIGNAL(request(QVariant)), &core, SLOT(request(QVariant)));

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
	}
}

