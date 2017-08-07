#ifndef COREROOT_H
#define COREROOT_H

#include <QSharedPointer>
#include <fstream>
#include <QProcess>
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
            virtual QSharedPointer<CreateVolumeResponse> createVolume(QSharedPointer<CreateVolumeParams> params);
			virtual QSharedPointer<CreateKeyFileResponse> createKeyFile(QSharedPointer<CreateKeyFileParams> params);
			virtual QSharedPointer<ChangeVolumePasswordResponse> changeVolumePassword(QSharedPointer<ChangeVolumePasswordParams> params);
        private:
            void writeHeaderToFile(fstream &file, QSharedPointer<CreateVolumeParams::VolumeParams> params, QSharedPointer<VolumeLayout> layout, quint64 containersize);
            void formatVolume(QSharedPointer<QFileInfo> volume, QSharedPointer<VolumePassword> password, QSharedPointer<KeyfileList> keyfiles, QString filesystem);
            uid_t realUserId;
            gid_t realGroupId;
		};
	}
}

#endif // COREROOT_H
