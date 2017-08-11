#ifndef COREROOT_H
#define COREROOT_H

#include <QObject>
#include <corebase.h>
#include <QDebug>

class CoreRoot : public CoreBase
{
	Q_OBJECT
public:
	explicit CoreRoot(QObject *parent = nullptr) : CoreBase(parent) {}
public slots:
	virtual void exit() {
		qDebug() <<"CoreRoot exiting";
		emit exited();
	}
	virtual void request(QVariant request);


private:
	virtual QSharedPointer<MaxResponse> max(QSharedPointer<MaxRequest> r);

};

#endif // COREROOT_H
