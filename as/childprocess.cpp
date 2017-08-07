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

	INIT_SERIALIZE(BaseClass);
	INIT_SERIALIZE(ChildCLass1);
	INIT_SERIALIZE(ChildCLass2);

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

	if(v.canConvert<ChildCLass1>()) {
		ChildCLass1 c1;
		c1 = v.value<ChildCLass1>();
		c1.print();
		if(c1.a == 0 && c1.b == 0)
			return false;
		c1.a *= 2;
		c1.b *= 2;
		outputStream << QVariant::fromValue(c1);
		outputFile.flush();
	} else if(v.canConvert<ChildCLass2>()) {
		ChildCLass2 c2;
		c2 = v.value<ChildCLass2>();
		c2.print();
		if(c2.a == 0 && c2.c == 0)
			return false;
		c2.a *= 2;
		c2.c *= 2;
		outputStream << QVariant::fromValue(c2);
		outputFile.flush();
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

