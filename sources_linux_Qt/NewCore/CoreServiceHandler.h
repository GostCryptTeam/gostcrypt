#ifndef CORESERVICEHANDLER_H
#define CORESERVICEHANDLER_H

#include <QObject>
#include <QCoreApplication>
#include <QDataStream>
#include <QProcess>
#include <QVariant>
#include <QQueue>

//#define DEBUG_CORESERVICE_HANDLER

namespace GostCrypt {
    namespace NewCore {
		class CoreServiceHandler : public QObject
		{
			Q_OBJECT
		public:
			CoreServiceHandler();
			void sendToCoreService(QVariant request);
			bool isRunning();
			void exit();
		public slots:
			void receiveSudoPassword(QSharedPointer<QByteArray> password);
		private:
			void sendRequests();
			void startWorkerProcess();

			QDataStream workerProcessStream;
			QProcess workerProcess;
			QQueue<QVariant> waitingRequests;
			bool processInitialized;
			bool askedToQuit;
			bool exceptionToRead;
		private slots: // private for direct call but still connectable => Need for communicating class
			void receive();
			void workerProcessExited(int exitCode, QProcess::ExitStatus exitStatus);
			void dbg_bytesWritten(qint64 bytes);
			void workerProcessStarted();
			void checkInitReceived();
		signals:
			void sendResponse(QVariant &response);
			void exited();
			void askSudoPassword();
		};
	}
}

#endif // CORESERVICEHANDLER_H
