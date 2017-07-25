#ifndef COREBASE_H
#define COREBASE_H

#include <QObject>
#include "CoreParams.h"
#include "CoreResponse.h"

namespace GostCrypt {
	namespace NewCore {
		class CoreBase : public QObject
		{
			Q_OBJECT
		public:
			CoreBase();

			virtual QSharedPointer<MountVolumeResponse> mountVolume(QSharedPointer<MountVolumeParams> params) = 0;
			virtual QSharedPointer<DismountVolumeResponse> dismountVolume(QSharedPointer<DismountVolumeParams> params) = 0;
			virtual QSharedPointer<CreateVolumeResponse> createVolume(QSharedPointer<DismountVolumeParams> params) = 0;
			virtual QSharedPointer<CreateKeyFileResponse> createKeyFile(QSharedPointer<CreateKeyFileParams> params) = 0;
			virtual QSharedPointer<ChangeVolumePasswordResponse> changeVolumePassword(QSharedPointer<ChangeVolumePasswordParams> params) = 0;
			QSharedPointer<GetHostDevicesResponse> getHostDevices(QSharedPointer<GetHostDevicesParams> params);
			QSharedPointer<GetMountedVolumesResponse> getMountedVolumes(QSharedPointer<GetMountedVolumesParams> params);
		private:

		};
		QSharedPointer<CoreBase> getCore();
	}
}

#endif // COREBASE_H
