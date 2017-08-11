#ifndef COREUSER_H
#define COREUSER_H

#include <QObject>
#include "corebase.h"
#include "coreservicehandler.h"

class CoreUser : public CoreBase
{
	Q_OBJECT
public:
	explicit CoreUser(QObject *parent = nullptr);
public slots:
		virtual void exit();
		virtual void request(QVariant request);
		virtual void receiveSudoPassword(QSharedPointer<QByteArray> password);
private slots:
	void receiveResponse(QVariant &response);
private:
	CoreServiceHandler csh;
signals:
	void sendSudoPassword(QSharedPointer<QByteArray> password);
};

#endif // COREUSER_H
