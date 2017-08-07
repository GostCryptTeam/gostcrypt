#ifndef COREUSER_H
#define COREUSER_H

#include <QSharedPointer>
#include <QProcess>
#include <QDataStream>
#include "CoreResponse.h"
#include "CoreParams.h"
#include "CoreBase.h"

namespace GostCrypt {
	namespace NewCore {
		class CoreUser : public CoreBase
		{
		public:
            CoreUser(QCoreApplication *a) : app(a) {}
			virtual QSharedPointer<MountVolumeResponse> mountVolume(QSharedPointer<MountVolumeParams> params);
			virtual QSharedPointer<DismountVolumeResponse> dismountVolume(QSharedPointer<DismountVolumeParams> params);
            virtual QSharedPointer<CreateVolumeResponse> createVolume(QSharedPointer<CreateVolumeParams> params);
			virtual QSharedPointer<CreateKeyFileResponse> createKeyFile(QSharedPointer<CreateKeyFileParams> params);
			virtual QSharedPointer<ChangeVolumePasswordResponse> changeVolumePassword(QSharedPointer<ChangeVolumePasswordParams> params);
		protected:
            QVariant forwardRoot(const QVariant params);
            void launchRootProcess();
		signals:
			void askAdminPassword();
		public slots:
			void sendAdminPassword();
        private:
            QProcess rootProcess;
            QDataStream rootProcessStream;
            QCoreApplication *app;
		};

	}
}

#endif // COREUSER_H
