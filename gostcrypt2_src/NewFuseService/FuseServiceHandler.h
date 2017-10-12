#ifndef FUSESERVICEHANDLER_H
#define FUSESERVICEHANDLER_H
//#define FUSE_SERVICE_DEBUG
#include "NewCore/ServiceHandler.h"
#include "NewCore/CoreRequest.h"
#include "NewCore/CoreRequest.h"
#include "NewFuseService/FuseService.h"
#include <QSharedPointer>

namespace GostCrypt {
	namespace NewFuseDriver {
		class FuseServiceHandler : public GostCrypt::NewCore::ServiceHandler
		{
			Q_OBJECT
		public:
			FuseServiceHandler();
			void mount(QSharedPointer<NewCore::MountVolumeRequest> params);
		private slots:
			void receiveResponse(QVariant &response);
			void done();
		private:
			QSharedPointer<NewCore::MountVolumeResponse> response;
			QSharedPointer<NewCore::MountVolumeRequest> params;
			bool succeed;
		signals:
			void continueMountVolume(QSharedPointer<NewCore::MountVolumeRequest> params, QSharedPointer<NewCore::MountVolumeResponse> response);
		};
	}
}

#endif // FUSESERVICEHANDLER_H
