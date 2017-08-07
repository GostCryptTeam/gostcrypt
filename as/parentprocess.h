#ifndef PARENTPROCESS_H
#define PARENTPROCESS_H

#include <QObject>
#include <QCoreApplication>
#include <QDataStream>
#include <QProcess>
#include <QVariant>

class ParentProcess : QObject
{
	Q_OBJECT
public:
	ParentProcess();
	int start(int argc, char **argv);
private:
	QCoreApplication *a;
	QDataStream workerProcessStream;
	QProcess workerProcess;
	QVariant currentRequest;
private slots:
	void receiveResponse();
	void sendRequest();
	void finish();
	void dbg_bytesWritten(qint64 bytes);
	void workerProcessStarted();

};

#endif // PARENTPROCESS_H
