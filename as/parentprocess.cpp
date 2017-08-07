#include "parentprocess.h"
#include "serializableclasses.h"
#include <QProcess>

ParentProcess::ParentProcess()
{

}

int ParentProcess::start(int argc, char **argv)
{
	QCoreApplication app(argc, argv);

	INIT_SERIALIZE(BaseClass);
	INIT_SERIALIZE(ChildCLass1);
	INIT_SERIALIZE(ChildCLass2);

	a = &app;
	QCoreApplication::setApplicationName("parent");
	a->connect(&workerProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(receiveResponse()));
	a->connect(&workerProcess, SIGNAL(started()), this, SLOT(workerProcessStarted()));
	a->connect(&workerProcess, SIGNAL(finished(int)), this, SLOT(finish()));
	a->connect(&workerProcess, SIGNAL(bytesWritten(qint64)), this, SLOT(dbg_bytesWritten(qint64)));
	ChildCLass1 c1;
	c1.a = 3;
	c1.b = 4;
	this->currentRequest = QVariant::fromValue(c1);
	workerProcessStream.setDevice(&workerProcess);

	workerProcess.setProgram(QCoreApplication::applicationFilePath());
	workerProcess.setArguments({"slave"});
	workerProcess.setProcessChannelMode(QProcess::ForwardedErrorChannel);
	workerProcess.start();

	return a->exec();
}

void ParentProcess::receiveResponse()
{
	QVariant response;

	qDebug() << "something received";
	workerProcessStream.startTransaction();
	workerProcessStream >> response;
	if(!workerProcessStream.commitTransaction())
		return;

	if(response.canConvert<ChildCLass1>()) {
		response.value<ChildCLass1>().print();
	} else if(response.canConvert<ChildCLass2>()) {
		ChildCLass2 c2;
		c2 = response.value<ChildCLass2>();
		c2.print();
	} else {
		qDebug() << "Unknow object : " << response.typeName();
	}

	/* Send finish instruction */
	ChildCLass1 c1;
	c1.a = c1.b = 0;
	currentRequest = QVariant::fromValue(c1);
	sendRequest();

}

void ParentProcess::sendRequest()
{
	this->workerProcessStream << currentRequest;
	currentRequest = QVariant();
	qDebug() << "Request sent";
}

void ParentProcess::finish()
{
	a->quit();
}

void ParentProcess::dbg_bytesWritten(qint64 bytes)
{
	qDebug() << bytes << " bytes written";
}

void ParentProcess::workerProcessStarted()
{
	qDebug() << "Worker process (" << workerProcess.processId() << ") started.";
	sendRequest();
}
