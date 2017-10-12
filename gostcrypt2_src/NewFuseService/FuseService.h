#ifndef FUSESERVICE_H
#define FUSESERVICE_H

#include <QMutex>
#include <QString>
#include "NewCore/CoreRequest.h"
#include "NewCore/CoreResponse.h" //TODO move definition of VolumeInformation to Volume subproject
#include "NewCore/Service.h"
#include "NewFuseService/FuseException.h"
#include "Volume/Volume.h"


namespace GostCrypt {
	namespace NewFuseDriver {
		const char *getVolumeImagePath();
		const char *getControlPath();
	    void sendAuxDeviceInfo (QSharedPointer<QFileInfo> fuseMountPoint, QSharedPointer<QFileInfo> virtualDevice);



		class NewFuseService : public GostCrypt::NewCore::Service
		{
			Q_OBJECT
		public:
			NewFuseService();
			static bool checkAccessRights();
			static int exceptionToErrorCode();
			static void dismount();
			static uint64 getVolumeSize();
			static uint64 getVolumeSectorSize();
			static void readVolumeSectors(const BufferPtr &buffer, uint64 byteOffset);
			static void writeVolumeSectors (const ConstBufferPtr &buffer, uint64 byteOffset);
			static bool auxDeviceInfoReceived();
			static void receiveAuxDeviceInfo (QByteArray &buffer);
			static QSharedPointer<QByteArray> getVolumeInfo();
			static uid_t getUserId() { return NewFuseService::userId; }
			static gid_t getGroupId() { return NewFuseService::groupId; }
		private:
			static QSharedPointer<Volume> mountedVolume;
			static QSharedPointer<NewCore::VolumeInformation> volumeInfo;
			static uid_t userId;
			static gid_t groupId;
			static QMutex volumeInfoMutex;
			static QSharedPointer<NewCore::MountVolumeRequest> params;
			virtual void connectRequestHandlingSignals();
			virtual void initSerializables();
			void sendResponseWhenReady(QVariant response);
			void launchFuse();

		public slots:			 //TODO : make private
			void mountRequestHandler(QVariant r);
		};
	}
}

#endif // FUSESERVICE_H
