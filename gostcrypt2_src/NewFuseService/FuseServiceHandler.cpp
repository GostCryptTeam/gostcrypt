#include "FuseServiceHandler.h"
#include "FuseService.h"
#include "NewCore/ServiceHandler.h"
#include "NewCore/CoreRequest.h"


namespace GostCrypt {
	namespace NewFuseDriver {
		FuseServiceHandler::FuseServiceHandler() : ServiceHandler(QCoreApplication::applicationFilePath(), {"fuseservice"})
		{
			connect(this, SIGNAL(sendResponse(QVariant&)), this, SLOT(receiveResponse(QVariant&)));
			connect(this, SIGNAL(exited()), this, SLOT(done()));
			initFuseException();
			this->succeed = false;
		}

		void FuseServiceHandler::mount(QSharedPointer<NewCore::MountVolumeRequest> params)
		{
			this->params = params;
			this->sendToService(QVariant::fromValue(params));
		}

		void FuseServiceHandler::receiveResponse(QVariant &r)
		{
			if(!r.canConvert<QSharedPointer<NewCore::MountVolumeResponse>>())
				throw UnknowResponseException(r.typeName());

			this->succeed = true;
			this->exit();
			this->response = r.value<QSharedPointer<NewCore::MountVolumeResponse>>();
			if(this->response.isNull())
				this->response.reset(new NewCore::MountVolumeResponse());
			if(!this->params.isNull())
				response->passThrough = params->passThrough;
		}

		void FuseServiceHandler::done()
		{
			if(this->succeed)
				emit continueMountVolume(this->params, this->response);
		}
	}
}
