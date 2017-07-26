#include <unistd.h>
#include <mntent.h>
#include "CoreBase.h"
#include "CoreUser.h"
#include "CoreRoot.h"
#include "CoreException.h"

namespace GostCrypt {
	namespace NewCore {

		QSharedPointer<CoreBase> getCore()
		{
			initCoreParams();
			initCoreResponse();
			initCoreException();
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
			QSharedPointer<GetMountedVolumesResponse> response(new GetMountedVolumesResponse);

			foreach(QSharedPointer<MountedFilesystem> mf, getMountedFilesystems()) {
				//if(mf->MountPoint.)
			}

			return response;
		}

		QList<QSharedPointer<MountedFilesystem> > CoreBase::getMountedFilesystems(const QFileInfo &devicePath, const QFileInfo &mountPoint)
		{
			QList<QSharedPointer<MountedFilesystem>> mountedFilesystems;
			//*
			FILE *mtab = setmntent ("/etc/mtab", "r");

			if (!mtab)
				mtab = setmntent ("/proc/mounts", "r");
			if(!mtab)
				throw FailedOpenFile(QFileInfo("/proc/mounts"));

			static QMutex mutex;
			mutex.lock();

			struct mntent *entry;
			while ((entry = getmntent (mtab)) != nullptr)
			{
				QSharedPointer<MountedFilesystem> mf(new MountedFilesystem);

				if (entry->mnt_fsname)
					mf->Device = QFileInfo (QString(entry->mnt_fsname));
				else
					continue;

				if (entry->mnt_dir)
					mf->MountPoint = QFileInfo(QString(entry->mnt_dir));

				//if (entry->mnt_type)
				//	mf->Type = entry->mnt_type;//TODO


				if ((devicePath.canonicalFilePath().isEmpty() || devicePath == mf->Device) && \
						(mountPoint.canonicalFilePath().isEmpty() || mountPoint == mf->MountPoint))
					mountedFilesystems.append(mf);
			}//*/

			endmntent(mtab);
			mutex.unlock();
			return mountedFilesystems;
		}

	}
}
