#ifndef PARENTPROCESS_H
#define PARENTPROCESS_H

#include <QObject>
#include <QCoreApplication>
#include <QDataStream>
#include <QProcess>
#include <QVariant>
#include <QQueue>

class ParentProcess : QObject
{
	Q_OBJECT
public:
	ParentProcess();
	int start(int argc, char **argv);
private:
	void sendRequests();
	void startWorkerProcess();

	QCoreApplication *a;
	QDataStream workerProcessStream;
	QProcess workerProcess;
	QQueue<QVariant> waitingRequests;
public slots:
	void min(quint32 a, quint32 b);
	void max(quint32 a, quint32 b);
	void exit();
private slots: // private for direct call but still connectable => Need for communicating class
	void receiveResponse();
	void finish();
	void dbg_bytesWritten(qint64 bytes);
	void workerProcessStarted();


};

#endif // PARENTPROCESS_H
