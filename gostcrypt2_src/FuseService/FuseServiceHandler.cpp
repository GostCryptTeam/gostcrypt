#include "FuseServiceHandler.h"
#include "FuseService.h"
#include "Core/ServiceHandler.h"
#include "Core/CoreRequest.h"


namespace GostCrypt {
	namespace FuseDriver {
		FuseServiceHandler::FuseServiceHandler() : ServiceHandler(QCoreApplication::applicationFilePath(), {"fuseservice"})
		{
			connect(this, SIGNAL(sendResponse(QVariant&)), this, SLOT(receiveResponse(QVariant&)));
			connect(this, SIGNAL(exited()), this, SLOT(done()));
			initFuseException();
			this->succeed = false;
		}

		void FuseServiceHandler::mount(QSharedPointer<Core::MountVolumeRequest> request)
		{
			this->params = request;
			this->sendToService(QVariant::fromValue(request));
		}

		void FuseServiceHandler::receiveResponse(QVariant &r)
		{
			if(!r.canConvert<QSharedPointer<Core::MountVolumeResponse>>())
				throw UnknowResponseException(r.typeName());

			this->succeed = true;
			this->exit();
			this->response = r.value<QSharedPointer<Core::MountVolumeResponse>>();
			if(this->response.isNull())
				this->response.reset(new Core::MountVolumeResponse());
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
