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
            virtual void receiveSudoPassword(QString password); //TODO
		private:
			QSharedPointer<MountVolumeResponse> mountVolume(QSharedPointer<MountVolumeRequest> params, bool emitResponse = true);
            QSharedPointer<DismountVolumeResponse> dismountVolume(QSharedPointer<DismountVolumeRequest> params = QSharedPointer<DismountVolumeRequest>(), bool emitResponse = true);
            QSharedPointer<CreateVolumeResponse> createVolume(QSharedPointer<CreateVolumeRequest> params, bool emitResponse = true);
			QSharedPointer<ChangeVolumePasswordResponse> changeVolumePassword(QSharedPointer<ChangeVolumePasswordRequest> params, bool emitResponse = true);

            void writeHeaderToFile(fstream &file, QSharedPointer<CreateVolumeRequest::VolumeParams> params, QSharedPointer<VolumeLayout> layout, quint64 containersize);
            void formatVolume(QSharedPointer<QFileInfo> volume, QSharedPointer<QByteArray> password, QSharedPointer<QList<QSharedPointer<QFileInfo>>> keyfiles, QString filesystem);
            uid_t realUserId;
            gid_t realGroupId;
		};
	}
}

#endif // COREROOT_H
