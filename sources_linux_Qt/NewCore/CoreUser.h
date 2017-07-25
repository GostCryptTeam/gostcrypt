#ifndef COREUSER_H
#define COREUSER_H

#include <QSharedPointer>
#include "CoreResponse.h"
#include "CoreParams.h"
#include "CoreBase.h"

namespace GostCrypt {
	namespace NewCore {
		class CoreUser : public CoreBase
		{
		public:
			CoreUser();
			virtual QSharedPointer<MountVolumeResponse> mountVolume(QSharedPointer<MountVolumeParams> params);
			virtual QSharedPointer<DismountVolumeResponse> dismountVolume(QSharedPointer<DismountVolumeParams> params);
			virtual QSharedPointer<CreateVolumeResponse> createVolume(QSharedPointer<DismountVolumeParams> params);
			virtual QSharedPointer<CreateKeyFileResponse> createKeyFile(QSharedPointer<CreateKeyFileParams> params);
			virtual QSharedPointer<ChangeVolumePasswordResponse> changeVolumePassword(QSharedPointer<ChangeVolumePasswordParams> params);
		protected:
			QSharedPointer<CoreResponse> forwardRoot(QSharedPointer<CoreParams> params);
		signals:
			void askAdminPassword();
		public slots:
			void sendAdminPassword();
		};

	}
}

#endif // COREUSER_H
