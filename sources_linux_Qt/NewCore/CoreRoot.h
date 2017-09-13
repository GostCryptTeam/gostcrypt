#ifndef COREROOT_H
#define COREROOT_H

#include <QSharedPointer>
#include <fstream>
#include <QProcess>
#include <QByteArray>
#include "CoreBase.h"
#include "CoreRequest.h"
#include "CoreResponse.h"
#include "RandomNumberGenerator.h"

namespace GostCrypt {
	namespace NewCore {
		class CoreRoot : public CoreBase
		{
		Q_OBJECT
		public:
			explicit CoreRoot(QObject *parent = nullptr);
		public slots:
			virtual void exit();
			virtual void request(QVariant r);
			virtual void receiveSudoPassword(QSharedPointer<QByteArray> password);
		public: //make protected
			virtual QSharedPointer<MountVolumeResponse> mountVolume(QSharedPointer<MountVolumeRequest> params);
            virtual QSharedPointer<DismountVolumeResponse> dismountVolume(QSharedPointer<DismountVolumeRequest> params = QSharedPointer<DismountVolumeRequest>());
            virtual QSharedPointer<CreateVolumeResponse> createVolume(QSharedPointer<CreateVolumeRequest> params);
			virtual QSharedPointer<ChangeVolumePasswordResponse> changeVolumePassword(QSharedPointer<ChangeVolumePasswordRequest> params);
        private:
            void writeHeaderToFile(fstream &file, QSharedPointer<CreateVolumeRequest::VolumeParams> params, QSharedPointer<VolumeLayout> layout, quint64 containersize);
            void formatVolume(QSharedPointer<QFileInfo> volume, QSharedPointer<QByteArray> password, QSharedPointer<QList<QSharedPointer<QFileInfo>>> keyfiles, QString filesystem);
            uid_t realUserId;
            gid_t realGroupId;
		};
	}
}

#endif // COREROOT_H
