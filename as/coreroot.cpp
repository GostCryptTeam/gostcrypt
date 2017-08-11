#include "coreroot.h"
#include "serializableclasses.h"

QSharedPointer<MaxResponse> CoreRoot::max(QSharedPointer<MaxRequest> r)
{
	QSharedPointer<MaxResponse> res(new MaxResponse);
	res->res = (r->a>r->b)?r->a:r->b;
	return res;
}

void CoreRoot::request(QVariant r)
{
	if(r.canConvert<QSharedPointer<MaxRequest>>()) {
		QSharedPointer<MaxRequest> request;
		QSharedPointer<MaxResponse> response;
		request = r.value<QSharedPointer<MaxRequest>>();
		request->print();
		response  = max(request);
		emit sendMax(response);
	} else if(r.canConvert<QSharedPointer<MinRequest>>()) {
		QSharedPointer<MinRequest> request;
		QSharedPointer<MinResponse> response;
		request = r.value<QSharedPointer<MinRequest>>();
		request->print();
		response = min(request);
		emit sendMin(response);
	} else {
		qDebug() << "Unknow request : " << r.typeName();
	}

}
