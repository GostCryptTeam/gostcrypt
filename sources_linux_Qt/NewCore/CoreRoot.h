#ifndef COREROOT_H
#define COREROOT_H

#include <QSharedPointer>
#include <fstream>
#include "CoreBase.h"
#include "CoreParams.h"
#include "CoreResponse.h"
#include "RandomNumberGenerator.h"

namespace GostCrypt {
	namespace NewCore {
		class CoreRoot : public CoreBase
		{
		public:
			CoreRoot();
			virtual QSharedPointer<MountVolumeResponse> mountVolume(QSharedPointer<MountVolumeParams> params);
			virtual QSharedPointer<DismountVolumeResponse> dismountVolume(QSharedPointer<DismountVolumeParams> params);
			virtual QSharedPointer<CreateVolumeResponse> createVolume(QSharedPointer<DismountVolumeParams> params);
			virtual QSharedPointer<CreateKeyFileResponse> createKeyFile(QSharedPointer<CreateKeyFileParams> params);
			virtual QSharedPointer<ChangeVolumePasswordResponse> changeVolumePassword(QSharedPointer<ChangeVolumePasswordParams> params);
        private:
            virtual void writeHeaderToFile(fstream &file, QSharedPointer<CreateVolumeParams::VolumeParams> params, QSharedPointer<VolumeLayout> layout, quint64 containersize);
		};
	}
}

#endif // COREROOT_H
