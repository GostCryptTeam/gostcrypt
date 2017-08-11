#include "mainprocess.h"
#include "corebase.h"
#include "coreuser.h"
#include "serializableclasses.h"
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <QFile>
#include <QException>
//#define MIN_REQUEST

MainProcess::MainProcess(QObject *parent) : QObject(parent)
{

}

QTextStream &qStdOut() {
    static QTextStream ts( stdout );
    return ts;
}

int MainProcess::start(int argc, char **argv)
{
	MyApplication app(argc, argv);
	core.reset(new CoreUser);

	MyApplication::setApplicationName("main");
	initSerializables(); //TODO change syntax
	app.connect(core.data(), SIGNAL(sendMax(QSharedPointer<MaxResponse>)), this, SLOT(printMax(QSharedPointer<MaxResponse>)));
	app.connect(core.data(), SIGNAL(sendMin(QSharedPointer<MinResponse>)), this, SLOT(printMin(QSharedPointer<MinResponse>)));

	app.connect(this, SIGNAL(request(QVariant)), core.data(), SLOT(request(QVariant)));
	app.connect(this, SIGNAL(exit()), core.data(), SLOT(exit()));
	app.connect(&app, SIGNAL(exit()), core.data(), SLOT(exit()));
	app.connect(core.data(), SIGNAL(exited()), &app, SLOT(quit()));

	app.connect(core.data(), SIGNAL(askSudoPassword()), this, SLOT(askSudoPassword()));
	app.connect(this, SIGNAL(sendSudoPassword(QSharedPointer<QByteArray>)), core.data(), SLOT(receiveSudoPassword(QSharedPointer<QByteArray>)));

#ifndef MIN_REQUEST
	QSharedPointer<MaxRequest> rmax(new MaxRequest);
	rmax->a = 6;
	rmax->b = 8;
	emit request(QVariant::fromValue(rmax));
#else
	QSharedPointer<MinRequest> rmin(new MinRequest);
	rmin->a = 3;
	rmin->b = 8;
	emit request(QVariant::fromValue(rmin));
#endif

	return app.exec();
}

void MainProcess::printMin(QSharedPointer<MinResponse> r)
{
	qDebug() << "Min: " << r->res;
	emit exit();
}

void MainProcess::printMax(QSharedPointer<MaxResponse> r)
{
	qDebug() << "Max: " << r->res;
	emit exit();
}

void MainProcess::askSudoPassword()
{
	QSharedPointer<QByteArray> password(new QByteArray("a", 1));

	termios oldt;
	tcgetattr(STDIN_FILENO, &oldt);
	termios newt = oldt;
	newt.c_lflag &= ~ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &newt); // hide input

	qStdOut() << "Please enter your sudo password: " << endl;

	// Temporaire mot de passe toujours en mémoire
	QFile inputFile;
	QTextStream in(&inputFile);
	inputFile.open(stdin, QFile::ReadOnly);
	QString line = in.readLine();
	inputFile.close();
	*password = line.toUtf8();
	line.fill('\0');

	qDebug() << QString(*password);
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // reset back the previous configuration
	emit sendSudoPassword(password);
}

bool MyApplication::notify(QObject *receiver, QEvent *event)
{
	bool done = true;
	try {
		done = QCoreApplication::notify(receiver, event);
	} catch (QException ) {
		qDebug() << "Exception catched !";
		emit exit();
	}
	return done;
}
