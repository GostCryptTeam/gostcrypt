#include "mainprocess.h"
#include "corebase.h"
#include "coreuser.h"
#include "serializableclasses.h"
#define MIN_REQUEST

MainProcess::MainProcess(QObject *parent) : QObject(parent)
{

}

int MainProcess::start(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	core.reset(new CoreUser);

	QCoreApplication::setApplicationName("main");
	initSerializables(); //TODO change syntax
	app.connect(core.data(), SIGNAL(sendMax(QSharedPointer<MaxResponse>)), this, SLOT(printMax(QSharedPointer<MaxResponse>)));
	app.connect(core.data(), SIGNAL(sendMin(QSharedPointer<MinResponse>)), this, SLOT(printMin(QSharedPointer<MinResponse>)));

	app.connect(this, SIGNAL(request(QVariant)), core.data(), SLOT(request(QVariant)));
	app.connect(this, SIGNAL(exit()), core.data(), SLOT(exit()));
	app.connect(core.data(), SIGNAL(exited()), &app, SLOT(quit()));

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
