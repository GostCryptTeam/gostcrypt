#include "coreservicehandler.h"
#include "serializableclasses.h"
#include <QProcess>
#include <QMetaEnum>


CoreServiceHandler::CoreServiceHandler()
{
	initSerializables();


	connect(&workerProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(receiveResponse()));
	connect(&workerProcess, SIGNAL(started()), this, SLOT(workerProcessStarted()));
	connect(&workerProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(workerProcessExited(int,QProcess::ExitStatus)));
	connect(&workerProcess, SIGNAL(bytesWritten(qint64)), this, SLOT(dbg_bytesWritten(qint64)));
}

void CoreServiceHandler::sendToCoreService(QVariant request)
{
	this->waitingRequests.enqueue(QVariant::fromValue(request));
	sendRequests();
}

bool CoreServiceHandler::isRunning()
{
	return (workerProcess.state() != QProcess::NotRunning);
}

void CoreServiceHandler::receiveResponse()
{
	QVariant response;

	qDebug() << "Receiving response(s)";
	workerProcessStream.startTransaction();
	workerProcessStream >> response;
	if(!workerProcessStream.commitTransaction())
		return;

	emit sendResponse(response);
}

void CoreServiceHandler::sendRequests()
{
	if(workerProcess.state() != QProcess::Running)
		return startWorkerProcess();

	while(!waitingRequests.isEmpty())
		workerProcessStream << waitingRequests.dequeue();
	qDebug() << "Requests sent";
}

void CoreServiceHandler::workerProcessExited(int exitCode, QProcess::ExitStatus exitStatus)
{
	(void)exitCode;
	QMetaEnum metaEnum = QMetaEnum::fromType<QProcess::ExitStatus>();
	qDebug() << "Worker process finished:" << metaEnum.valueToKey(exitStatus);
	emit workerProcessFinished();
}

void CoreServiceHandler::dbg_bytesWritten(qint64 bytes)
{
	qDebug() << bytes << " bytes written";
}

void CoreServiceHandler::workerProcessStarted()
{
	qDebug() << "Worker process (" << workerProcess.processId() << ") started.";
	sendRequests();
}

void CoreServiceHandler::startWorkerProcess()
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
