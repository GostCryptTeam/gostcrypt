#include <unistd.h>
#include <mntent.h>
#include "CoreBase.h"
#include "CoreUser.h"
#include "CoreRoot.h"
#include "CoreResponse.h"
#include "CoreParams.h"
#include "CoreException.h"
#include <QMutex>

#include "Platform/FileStream.h"
#include "Platform/SharedPtr.h"
#include "FuseDriver/FuseService.h"

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
			for(QSharedPointer<MountedFilesystem> mf : getMountedFilesystems()) {
				/* Filter only Fuse FileSystems*/
				if(!mf->MountPoint.canonicalFilePath().startsWith(GOSTCRYPT_FUSE_MOUNTPOINT_PREFIX)) {
					continue;
				}

				QSharedPointer<VolumeInfo> mountedVol;

				/* TODO : Replace by Qt serialization in the future */
				try
				{
					shared_ptr <File> controlFile (new File);
					controlFile->Open (mf->MountPoint.canonicalFilePath().toStdString() + FuseService::GetControlPath());

					shared_ptr <Stream> controlFileStream (new FileStream (controlFile));
					mountedVol.reset(new VolumeInfo(*Serializable::DeserializeNew <VolumeInfo> (controlFileStream)));
				}
				catch (...)
				{
					continue;
				}

				/* If specific volume asked, check if this is the one */
				if(params && !params->volumePath.IsEmpty() && mountedVol->Path != params->volumePath)
					continue;

				/* Adding Fuse mount point information thanks to previous found mounted filesystem */
				mountedVol->AuxMountPoint = DirectoryPath(mf->MountPoint.canonicalFilePath().toStdString());

				/* Add final mount point information if possible */
				if(!mountedVol->VirtualDevice.IsEmpty())
				{
					QList<QSharedPointer<MountedFilesystem> > mpl = getMountedFilesystems(QFileInfo(QString::fromStdString(string(mountedVol->VirtualDevice))));
					if(!mpl.isEmpty()) {
						mountedVol->MountPoint = DirectoryPath(mpl.first()->MountPoint.canonicalFilePath().toStdString());
					}
				}

				response->volumeInfoList.append(mountedVol);

				/* If volume path specified no need to stay in the loop */
				if(params && !params->volumePath.IsEmpty())
					break;
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

        QSharedPointer<GetFileSystemTypesResponse> CoreBase::getFileSystemsSupported(QSharedPointer<GetFileSystemTypesParams> params)
        {
            QSharedPointer<GetFileSystemTypesResponse> response(new GetFileSystemTypesResponse);
            return response;
        }

	}
}
