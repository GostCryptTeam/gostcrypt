#include "childprocess.h"
#include "serializableclasses.h"
#include <QFile>

ChildProcess::ChildProcess()
{

}

int ChildProcess::start(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	QFile inputFile;

	initSerializables();

	a = &app;
	QCoreApplication::setApplicationName("child");

	inputStream.setDevice(&inputFile);
	outputStream.setDevice(&outputFile);
	inputFile.open(stdin, QFile::ReadOnly);
	outputFile.open(stdout, QFile::WriteOnly);

	while(receiveRequest());

	return 0;
}

bool ChildProcess::handleRequest(QVariant &v)
{
	qDebug() << "Handling request";

	if(v.canConvert<MaxRequest>()) {
		MaxRequest request;
		MaxResponse response;
		request = v.value<MaxRequest>();
		request.print();
		response.res = (request.a > request.b) ? request.a : request.b;
		outputStream << QVariant::fromValue(response);
		outputFile.flush();
	} else if(v.canConvert<MinRequest>()) {
		MinRequest request;
		MinResponse response;
		request = v.value<MinRequest>();
		request.print();
		response.res = (request.a < request.b) ? request.a : request.b;
		outputStream << QVariant::fromValue(response);
		outputFile.flush();
	} else if(v.canConvert<ExitRequest>()) {
		qDebug() << "Received Exit Request";
		return false;
	} else {
		qDebug() << "Unknow object : " << v.typeName();
	}

	return true;
}

bool ChildProcess::receiveRequest()
{
	QVariant request;

	qDebug() << "Reading...";
	inputStream.startTransaction();
	inputStream >> request;
	if(!inputStream.commitTransaction()) {
		qDebug() << "Not enough data";
		return true;
	}

	return handleRequest(request);
}

