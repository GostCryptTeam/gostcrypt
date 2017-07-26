#include <unistd.h>
#include <mntent.h>
#include "CoreBase.h"
#include "CoreUser.h"
#include "CoreRoot.h"

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

            return response;
        }

        QSharedPointer<GetFileSystemTypesResponse> CoreBase::getFileSystemsSupported(QSharedPointer<GetFileSystemTypesParams> params)
        {
            QSharedPointer<GetFileSystemTypesResponse> response(new GetFileSystemTypesResponse);
            return response;
        }

		QList<QSharedPointer<MountedFilesystem> > CoreBase::getMountedFilesystems(const QFileInfo &devicePath, const QFileInfo &mountPoint)
		{
			QList<QSharedPointer<MountedFilesystem>> mountedFilesystems;
			/*
			FILE *mtab = fopen ("/etc/mtab", "r");

			if (!mtab)
				mtab = fopen ("/proc/mounts", "r");
			if(!mtab)
				twrow Q
			throw_sys_sub_if (!mtab, "/proc/mounts");
			finally_do_arg (FILE *, mtab, { fclose (finally_arg); });

			static Mutex mutex;
			ScopeLock sl (mutex);

			struct mntent *entry;
			while ((entry = getmntent (mtab)) != nullptr)
			{
				make_shared_auto (MountedFilesystem, mf);

				if (entry->mnt_fsname)
					mf->Device = DevicePath (entry->mnt_fsname);
				else
					continue;

				if (entry->mnt_dir)
					mf->MountPoint = DirectoryPath (entry->mnt_dir);

				if (entry->mnt_type)
					mf->Type = entry->mnt_type;

				if ((devicePath.IsEmpty() || devicePath == mf->Device || realDevicePath == mf->Device) && (mountPoint.IsEmpty() || mountPoint == mf->MountPoint))
					mountedFilesystems.push_back (mf);
			}//*/

			return mountedFilesystems;

		}

	}
}
