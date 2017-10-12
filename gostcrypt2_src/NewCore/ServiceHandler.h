#ifndef SERVICEHANDLER_H
#define SERVICEHANDLER_H

#include <QObject>
#include <QCoreApplication>
#include <QDataStream>
#include <QProcess>
#include <QVariant>
#include <QQueue>
#include <QFile>

//#define DEBUG_SERVICE_HANDLER

namespace GostCrypt {
    namespace NewCore {
		class ServiceHandler : public QObject
		{
			Q_OBJECT
		public:
			ServiceHandler(QString programName, QStringList args);
			void sendToService(QVariant request);
			bool isRunning();
			void exit();
		protected:
			bool isInitialized();
			virtual void processCrashedBeforeInitialization();
			void writeToStdin(QSharedPointer<QByteArray> data, bool newline=false);
		private:
			void sendRequests();
			void startProcess();
			QString programName;
			QStringList args;
			QDataStream processStream;
			QProcess process;
			QQueue<QVariant> waitingRequests;
			bool processInitialized;
			bool askedToQuit;

		private slots:
			void receive();
			void processExited(int exitCode);
			void dbg_bytesWritten(qint64 bytes);
			void processStarted();
		signals:
			void sendResponse(QVariant &response);
			void exited();
			void started();
			void initialized();
		};
	}
}

#endif // SERVICEHANDLER_H
