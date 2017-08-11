#include "corebase.h"
#include "serializableclasses.h"

QSharedPointer<MinResponse> CoreBase::min(QSharedPointer<MinRequest> r)
{
	QSharedPointer<MinResponse> res(new MinResponse);
	res->res = (r->a<r->b)?r->a:r->b;

	throw std::exception();

	return res;
}
