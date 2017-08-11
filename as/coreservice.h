#ifndef CORESERVICE_H
#define CORESERVICE_H

#include <QObject>
#include <QCoreApplication>
#include <QDataStream>
#include <QFile>
#include <QVariant>
#include "coreroot.h"

#define DEF_RESPONSE_SLOT(request) \
	void send ## request (QSharedPointer< request ## Response> response) { \
		sendResponse(QVariant::fromValue(response)); \
	}
#define CONNNECT_RESPONSE_SLOT(request) \
	app.connect(&core, SIGNAL(send ## request (QSharedPointer< request ## Response>)), this, SLOT(send ## request (QSharedPointer< request ## Response>)))

class CoreService : QObject
{
	Q_OBJECT
public:
	CoreService();
	int start(int argc, char **argv);
private:
	QCoreApplication *a;
	QFile outputFile;
	QFile inputFile;
	QDataStream inputStream;
	QDataStream outputStream;
	CoreRoot core;
	bool receiveRequest();
	void sendResponse(QVariant r);

public slots:
	DEF_RESPONSE_SLOT(Max)
	DEF_RESPONSE_SLOT(Min)
signals:
	void request(QVariant request);
	void exit();
};

#endif // CORESERVICE_H
