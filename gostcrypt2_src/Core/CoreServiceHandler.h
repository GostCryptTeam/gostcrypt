#ifndef CORESERVICEHANDLER_H
#define CORESERVICEHANDLER_H

#include <QObject>
#include <QCoreApplication>
#include <QDataStream>
#include <QProcess>
#include <QVariant>
#include <QQueue>
#include "ServiceHandler.h"

namespace GostCrypt {
    namespace Core {
		class CoreServiceHandler : public ServiceHandler
		{
			Q_OBJECT
		public:
			CoreServiceHandler();
		public slots:
			void receiveSudoPassword(QSharedPointer<QByteArray> password);
			void checkInitReceived();
		private:
			virtual void processCrashedBeforeInitialization();
		signals:
			void askSudoPassword();
		};
	}
}

#endif // CORESERVICEHANDLER_H
