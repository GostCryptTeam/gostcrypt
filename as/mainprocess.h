#ifndef PARENTPROCESS_H
#define PARENTPROCESS_H

#include <QObject>
#include <QVariant>
#include <QCoreApplication>
#include "serializableclasses.h"
#include "corebase.h"

class MainProcess : public QObject
{
	Q_OBJECT
public:
	explicit MainProcess(QObject *parent = nullptr);
	int start(int argc, char **argv);
signals:
	void request(QVariant request);
	void exit();
public slots:
	void printMin(QSharedPointer<MinResponse> res);
	void printMax(QSharedPointer<MaxResponse> res);
private:
	QSharedPointer<CoreBase> core;
};

#endif // PARENTPROCESS_H
