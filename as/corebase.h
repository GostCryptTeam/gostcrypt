#ifndef COREBASE_H
#define COREBASE_H

#include <QObject>
#include <QCoreApplication>
#include "serializableclasses.h"

class CoreBase : public QObject
{
	Q_OBJECT
public:
	explicit CoreBase(QObject *parent = nullptr) : QObject(parent) {}
public slots:
	virtual void request(QVariant request) = 0;
	virtual void exit() = 0;

protected:
	virtual QSharedPointer<MinResponse> min(QSharedPointer<MinRequest> r);

signals:
	void sendMin(QSharedPointer<MinResponse> r);
	void sendMax(QSharedPointer<MaxResponse> r);
	void exited();
};

#endif // COREBASE_H
