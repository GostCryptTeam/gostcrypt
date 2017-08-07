#include <QCoreApplication>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QProcess>
#include <QThread>
#include <QBuffer>
#include "parentprocess.h"
#include "childprocess.h"
#include "serializableclasses.h"

int parentProcess(int argc, char *argv[]);
int childProcess(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	qSetMessagePattern("%{appname}: %{message}");
	if(argc <= 1) {
		ParentProcess p;
		return p.start(argc, argv);
	} else {
		ChildProcess p;
		return p.start(argc, argv);
	}
}

void send(QCoreApplication *a, QDataStream &stream, QVariant v) {
	stream << v;
	a->processEvents();
	qDebug() << "sent";
}

void printRead(QCoreApplication *a, QDataStream &stream) {
	qDebug() << "reading";
	QVariant v;
	while(v.isNull()) {
		stream >> v;
		QThread::msleep(100);
		a->processEvents();
	}
	if(v.canConvert<ChildCLass1>()) {
		ChildCLass1 c1Out;
		c1Out = v.value<ChildCLass1>();
		qDebug() << c1Out.a << c1Out.b;
	} else if(v.canConvert<ChildCLass2>()) {
		ChildCLass2 c2Out;
		c2Out = v.value<ChildCLass2>();
		qDebug() << c2Out.a << c2Out.c;
	} else if(v.canConvert<QSharedPointer<ChildCLass1>>()) {
		QSharedPointer<ChildCLass1> p;
		p = v.value<QSharedPointer<ChildCLass1>>();
		qDebug() << p->a << p->b;
	}
	qDebug()<< "done";
}
