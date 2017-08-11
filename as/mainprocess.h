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
	void sendSudoPassword(QSharedPointer<QByteArray> password);
private slots:
	void printMin(QSharedPointer<MinResponse> res);
	void printMax(QSharedPointer<MaxResponse> res);
	void askSudoPassword();

private:
	QSharedPointer<CoreBase> core;
};

class MyApplication : public QCoreApplication {
Q_OBJECT
public:
	MyApplication(int& argc, char** argv) : QCoreApplication(argc, argv) {}
	bool notify(QObject* receiver, QEvent* event);
signals:
	void exit();
};
QTextStream &qStdOut();

#endif // PARENTPROCESS_H
