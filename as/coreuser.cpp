#include "coreuser.h"
#include "serializableclasses.h"

CoreUser::CoreUser(QObject *parent) : CoreBase(parent)
{
	connect(&csh, SIGNAL(sendResponse(QVariant&)), this, SLOT(receiveResponse(QVariant&)));
}

void CoreUser::exit()
{
	ExitRequest request;

	if(csh.isRunning()) {
		qDebug() << "Sending exit request";
		connect(&csh, SIGNAL(workerProcessFinished()), this, SIGNAL(exited()));
		csh.sendToCoreService(QVariant::fromValue(request));
	} else {
		// The main loop was not started, so an imediate call to app.quit() would not be working.
		QMetaObject::invokeMethod(this, "exited", Qt::QueuedConnection);
	}
}

void CoreUser::receiveResponse(QVariant &response)
{
	if(response.canConvert<QSharedPointer<MinResponse>>()) {
		QSharedPointer<MinResponse> minResponse;
		minResponse = response.value<QSharedPointer<MinResponse>>();
		emit sendMin(minResponse);
	} else if(response.canConvert<QSharedPointer<MaxResponse>>()) {
		QSharedPointer<MaxResponse> maxResponse;
		maxResponse = response.value<QSharedPointer<MaxResponse>>();
		emit sendMax(maxResponse);
	} else {
		qDebug() << "Unknow object : " << response.typeName();
	}
}

void CoreUser::request(QVariant r)
{
	if(r.canConvert<QSharedPointer<MinRequest>>()) {
		QSharedPointer<MinRequest> request;
		QSharedPointer<MinResponse> response;
		request = r.value<QSharedPointer<MinRequest>>();
		request->print();
		response = min(request);
		emit sendMin(response);
	} else {
		csh.sendToCoreService(r);
	}

}
