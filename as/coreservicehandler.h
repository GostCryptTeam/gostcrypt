#ifndef CORESERVICEHANDLER_H
#define CORESERVICEHANDLER_H

#include <QObject>
#include <QCoreApplication>
#include <QDataStream>
#include <QProcess>
#include <QVariant>
#include <QQueue>

class CoreServiceHandler : public QObject
{
	Q_OBJECT
public:
	CoreServiceHandler();
	void sendToCoreService(QVariant request);
	bool isRunning();
private:
	void sendRequests();
	void startWorkerProcess();

	QDataStream workerProcessStream;
	QProcess workerProcess;
	QQueue<QVariant> waitingRequests;
private slots: // private for direct call but still connectable => Need for communicating class
	void receiveResponse();
	void workerProcessExited(int exitCode, QProcess::ExitStatus exitStatus);
	void dbg_bytesWritten(qint64 bytes);
	void workerProcessStarted();
signals:
	void sendResponse(QVariant &response);
	void workerProcessFinished();
};

#endif // CORESERVICEHANDLER_H
