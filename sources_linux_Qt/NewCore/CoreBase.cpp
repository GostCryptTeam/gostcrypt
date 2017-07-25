#include <unistd.h>
#include "CoreBase.h"
#include "CoreUser.h"
#include "CoreRoot.h"

namespace GostCrypt {
	namespace NewCore {
		QSharedPointer<CoreBase> getCore()
		{
			if(getuid()) {
				return QSharedPointer<CoreBase>(new CoreRoot());
			} else {
				return QSharedPointer<CoreBase>(new CoreUser());
			}
		}

		CoreBase::CoreBase()
		{

		}

		QSharedPointer<GetHostDevicesResponse> CoreBase::getHostDevices(QSharedPointer<GetHostDevicesParams> params)
		{

		}

		QSharedPointer<GetMountedVolumesResponse> CoreBase::getMountedVolumes(QSharedPointer<GetMountedVolumesParams> params)
		{
			QSharedPointer<GetMountedVolumesResponse> response;

			return response;
		}
	}
}
