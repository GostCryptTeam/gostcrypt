#include "parentprocess.h"
#include "serializableclasses.h"
#include <QProcess>

ParentProcess::ParentProcess()
{

}

int ParentProcess::start(int argc, char **argv)
{
	QCoreApplication app(argc, argv);

	initSerializables();

	a = &app;
	QCoreApplication::setApplicationName("parent");
	a->connect(&workerProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(receiveResponse()));
	a->connect(&workerProcess, SIGNAL(started()), this, SLOT(workerProcessStarted()));
	a->connect(&workerProcess, SIGNAL(finished(int)), this, SLOT(finish()));
	a->connect(&workerProcess, SIGNAL(bytesWritten(qint64)), this, SLOT(dbg_bytesWritten(qint64)));

	min(3, 4);
	exit();
	return a->exec();
}

void ParentProcess::min(quint32 a, quint32 b)
{
	MinRequest request;

	qDebug() << "Sending min request";
	request.a = a;
	request.b = b;
	this->waitingRequests.enqueue(QVariant::fromValue(request));
	sendRequests();
}

void ParentProcess::max(quint32 a, quint32 b)
{
	MaxRequest request;

	qDebug() << "Sending max request";
	request.a = a;
	request.b = b;
	this->waitingRequests.enqueue(QVariant::fromValue(request));
	sendRequests();
}

void ParentProcess::exit()
{
	ExitRequest request;

	qDebug() << "Sending exit request";
	this->waitingRequests.enqueue(QVariant::fromValue(request));
	sendRequests();
}

void ParentProcess::receiveResponse()
{
	QVariant response;

	qDebug() << "Receiving response(s)";
	workerProcessStream.startTransaction();
	workerProcessStream >> response;
	if(!workerProcessStream.commitTransaction())
		return;

	if(response.canConvert<MinResponse>()) {
		response.value<MinResponse>().print();
	} else if(response.canConvert<MaxResponse>()) {
		response.value<MaxResponse>().print();
	} else {
		qDebug() << "Unknow object : " << response.typeName();
	}
}

void ParentProcess::sendRequests()
{
	if(workerProcess.state() != QProcess::QProcess::Running)
		return startWorkerProcess();

	while(!waitingRequests.isEmpty())
		workerProcessStream << waitingRequests.dequeue();
	qDebug() << "Requests sent";
}

void ParentProcess::finish()
{
	qDebug() << "Worker process finished, exiting.";
	a->quit();
}

void ParentProcess::dbg_bytesWritten(qint64 bytes)
{
	qDebug() << bytes << " bytes written";
}

void ParentProcess::workerProcessStarted()
{
	qDebug() << "Worker process (" << workerProcess.processId() << ") started.";
	sendRequests();
}

void ParentProcess::startWorkerProcess()
{
	if(workerProcess.state() == QProcess::Starting)
		return;

	qDebug() << "Start worker process";
	workerProcessStream.setDevice(&workerProcess);
	workerProcess.setProgram(QCoreApplication::applicationFilePath());
	workerProcess.setArguments({"slave"});
	workerProcess.setProcessChannelMode(QProcess::ForwardedErrorChannel);
	workerProcess.start();
}
