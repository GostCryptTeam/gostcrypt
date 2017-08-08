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
