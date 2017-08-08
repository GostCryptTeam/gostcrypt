#ifndef CHILDPROCESS_H
#define CHILDPROCESS_H

#include <QObject>
#include <QCoreApplication>
#include <QDataStream>
#include <QFile>

class ChildProcess : QObject
{
	Q_OBJECT
public:
	ChildProcess();
	int start(int argc, char **argv);
private:
	QCoreApplication *a;
	QFile outputFile;
	QDataStream inputStream;
	QDataStream outputStream;
	bool handleRequest(QVariant &v);
	bool receiveRequest();
};

#endif // CHILDPROCESS_H
