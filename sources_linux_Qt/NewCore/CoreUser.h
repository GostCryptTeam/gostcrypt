#ifndef COREUSER_H
#define COREUSER_H

#include <QSharedPointer>
#include "CoreResponse.h"
#include "CoreParams.h"
#include "CoreBase.h"
#include "CoreServiceHandler.h"

namespace GostCrypt {
	namespace NewCore {
		class CoreUser : public CoreBase
		{
		Q_OBJECT
		public:
            explicit CoreUser(QObject *parent = nullptr);
        public slots:
			virtual void exit();
			virtual void request(QVariant r);
			virtual void receiveSudoPassword(QSharedPointer<QByteArray> password);
        private slots:
			void receiveResponse(QVariant &response);
        private:
			CoreServiceHandler csh;
		signals:
			void sendSudoPassword(QSharedPointer<QByteArray> password);
		};

	}
}

#endif // COREUSER_H
