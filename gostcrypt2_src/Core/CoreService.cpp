#include "CoreService.h"
#include "CoreResponse.h"
#include "CoreRequest.h"
#include "Service.h"
#include <QFile>
#include <QThread>

namespace GostCrypt {
    namespace Core {
		void CoreService::connectRequestHandlingSignals()
		{
			core.reset(new CoreRoot);
			connect(this, SIGNAL(request(QVariant)), core.data(), SLOT(request(QVariant)));
			connect(this, SIGNAL(askExit()), core.data(), SLOT(exit()));
            connect(core.data(), SIGNAL(exited()), this, SIGNAL(exit()));

			/* For each core request */
			CONNECT_RESPONSE_SLOT(CreateVolume);
			CONNECT_RESPONSE_SLOT(MountVolume);
			CONNECT_RESPONSE_SLOT(DismountVolume);
			CONNECT_RESPONSE_SLOT(ChangeVolumePassword);
			CONNECT_RESPONSE_SLOT(GetEncryptionAlgorithms);
			CONNECT_RESPONSE_SLOT(GetDerivationFunctions);
			CONNECT_RESPONSE_SLOT(GetHostDevices);
			CONNECT_RESPONSE_SLOT(GetMountedVolumes);
			CONNECT_RESPONSE_SLOT(CreateKeyFile);
			CONNECT_RESPONSE_SLOT(ProgressUpdate);
		}

		void CoreService::initSerializables()
		{
			initCoreRequest();
			initCoreResponse();
            initGostCryptException();
			INIT_SERIALIZE(ProgressUpdateResponse);

		}

	}
}

