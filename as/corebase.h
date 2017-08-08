#ifndef COREBASE_H
#define COREBASE_H

#include <QObject>

class CoreBase : QObject
{
	Q_OBJECT
public:
	CoreBase(QCoreApplication &app);
public slots:
	void min(quint32 a, quint32 b);
	void max(quint32 a, quint32 b);
	void exit();
private:
	WorkerProcessHandler wph;
	QCoreApplication *app;
};

#endif // COREBASE_H
