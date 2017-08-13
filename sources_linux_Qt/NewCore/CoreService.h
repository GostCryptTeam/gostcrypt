#ifndef CORESERVICE_H
#define CORESERVICE_H

#include <QObject>
#include <QCoreApplication>
#include <QDataStream>
#include <QFile>
#include <QVariant>
#include "CoreResponse.h"
#include "CoreRoot.h"

//#define DEBUG_CORESERVICE

#define DEF_RESPONSE_SLOT(request) \
	void send ## request (QSharedPointer< request ## Response> response) { \
		sendResponse(QVariant::fromValue(response)); \
	}
#define CONNECT_RESPONSE_SLOT(request) \
	app.connect(&core, SIGNAL(send ## request (QSharedPointer< request ## Response>)), this, SLOT(send ## request (QSharedPointer< request ## Response>)))

namespace GostCrypt {
    namespace NewCore {
		class CoreService : QObject
		{
			Q_OBJECT
		public:
			CoreService() {}
			int start(int argc, char **argv);
		private:
			QFile outputFile;
			QFile inputFile;
			QDataStream inputStream;
			QDataStream outputStream;
			CoreRoot core;
			bool receiveRequest();
			void sendResponse(QVariant r);

		public slots:
			DEF_RESPONSE_SLOT(CreateVolume)
			DEF_RESPONSE_SLOT(MountVolume)
			DEF_RESPONSE_SLOT(DismountVolume)
			DEF_RESPONSE_SLOT(GetMountedVolumes)
		private slots:
			void sendException(CoreException &e);
		signals:
			void request(QVariant request);
			void exit();
		};

		// redefines the notify function of QCoreApplication to catch all exceptions at once
		class CoreServiceApplication : public QCoreApplication {
		Q_OBJECT
		public:
			CoreServiceApplication(int& argc, char** argv) : QCoreApplication(argc, argv) {}
			bool notify(QObject* receiver, QEvent* event);
		signals:
			void exit();
			void sendException(CoreException &e);
		};
	}
}

#endif // CORESERVICE_H
