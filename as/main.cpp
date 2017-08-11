#include <QCoreApplication>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QProcess>
#include <QThread>
#include <QBuffer>
//#include "parentprocessold.h"
#include "coreservice.h"
#include "serializableclasses.h"
#include "mainprocess.h"

int main(int argc, char *argv[])
{
	qSetMessagePattern("%{appname}: %{message}");
	if(argc <= 1) {
		MainProcess p;
		return p.start(argc, argv);
	} else {
		CoreService p;
		return p.start(argc, argv);
	}
}
