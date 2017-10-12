#ifndef COREROOT_H
#define COREROOT_H

#include <QSharedPointer>
#include <fstream>
#include <QProcess>
#include <QByteArray>
#include "CoreBase.h"
#include "CoreRequest.h"
#include "CoreResponse.h"
#include "NewFuseService/FuseServiceHandler.h"
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
        private slots:
        	void continueMountVolume(QSharedPointer<NewCore::MountVolumeRequest> params, QSharedPointer<NewCore::MountVolumeResponse> response);
        	void continueFormatHidden(QSharedPointer<CreateVolumeRequest> params, QSharedPointer<CreateVolumeResponse> response);
        	void finishCreateVolume(QSharedPointer<CreateVolumeRequest> params, QSharedPointer<CreateVolumeResponse> response);
        	void continueMountFormat(QSharedPointer<MountVolumeResponse> mountResponse);
        	void continueDismountFormat(QSharedPointer<DismountVolumeResponse> dismountResponse);
		private:
			void mountVolume(QSharedPointer<MountVolumeRequest> params);
            QSharedPointer<DismountVolumeResponse> dismountVolume(QSharedPointer<DismountVolumeRequest> params = QSharedPointer<DismountVolumeRequest>());
            void createVolume(QSharedPointer<CreateVolumeRequest> params);
			QSharedPointer<ChangeVolumePasswordResponse> changeVolumePassword(QSharedPointer<ChangeVolumePasswordRequest> params);

            void writeHeaderToFile(fstream &file, QSharedPointer<CreateVolumeRequest::VolumeParams> params, QSharedPointer<VolumeLayout> layout, quint64 containersize);
            void mountFormatVolume(QSharedPointer<QFileInfo> volume, QSharedPointer<QByteArray> password, QSharedPointer<QList<QSharedPointer<QFileInfo>>> keyfiles, QString filesystem, QSharedPointer<CreateVolumeRequest> parentParams, QSharedPointer<CreateVolumeResponse> parentResponse);
            uid_t realUserId;
            gid_t realGroupId;
            GostCrypt::NewFuseDriver::FuseServiceHandler fuseServiceHandler;
        signals:
			void formatVolumeDone(QSharedPointer<CreateVolumeRequest> params, QSharedPointer<CreateVolumeResponse> response);

		};
	}
}

#endif // COREROOT_H
