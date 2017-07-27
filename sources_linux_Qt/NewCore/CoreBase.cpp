#include <unistd.h>
#include <mntent.h>
#include "CoreBase.h"
#include "CoreUser.h"
#include "CoreRoot.h"
#include "CoreResponse.h"
#include "CoreParams.h"
#include "CoreException.h"
#include <QMutex>
#include <QTextStream>
#include <QDebug>
#include <QString>

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
			QSharedPointer<GetHostDevicesResponse> res(new GetHostDevicesResponse);
			QFile file("/proc/partitions");

			if(!file.open(QFile::ReadOnly))
				throw FailedOpenFileException(QFileInfo("/proc/partitions"));
			QByteArray fileContent = file.readAll();
			QTextStream ts(&fileContent);
			while(!ts.atEnd()) {
				QStringList fields = ts.readLine().trimmed().split(" ", QString::SkipEmptyParts);

				if(fields.count() != 4
					|| fields.at(3).startsWith("loop")	// skip loop devices
					|| fields.at(3).startsWith("cloop")
					|| fields.at(3).startsWith("ram")	// skip RAM devices
					|| fields.at(3).startsWith("dm-")	// skip device mapper devices
					|| fields.at(2) == 1				// skip extended partitions
					)
					continue;

				bool isNumber;
				fields.at(0).toInt(&isNumber);
				if(!isNumber)
					continue;
				QSharedPointer<HostDevice> hd(new HostDevice());
				hd->devicePath = QFileInfo((fields.at(3).startsWith("/dev/") ? "" : "/dev/") + fields.at(3));
				hd->size = fields.at(2).toULongLong(&isNumber)*1024;
				if(!isNumber)
					qDebug() << "Fail to read device size for device " << hd->devicePath.canonicalFilePath();
				try {
					hd->mountPoint = getDeviceMountPoint(hd->devicePath);
				} catch(DeviceNotMounted &e) {}

				/* Check if device is partition */
				if(!res->hostDevices.isEmpty()) {
					if(hd->devicePath.canonicalFilePath().startsWith(res->hostDevices.last()->devicePath.canonicalFilePath())) {
						res->hostDevices.last()->partitions.append(hd);
						continue;
					}
				}

				res->hostDevices.append(hd);
			}


			return res;
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
					QFileInfo device (QString::fromStdString(string(mountedVol->VirtualDevice)));
					mountedVol->MountPoint = DirectoryPath(getDeviceMountPoint(device).canonicalFilePath().toStdString());
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
				throw FailedOpenFileException(QFileInfo("/proc/mounts"));

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

				if (entry->mnt_type) {
					mf->Type = QString(entry->mnt_type);
				}

				if ((devicePath.canonicalFilePath().isEmpty() || devicePath == mf->Device) && \
						(mountPoint.canonicalFilePath().isEmpty() || mountPoint == mf->MountPoint))
					mountedFilesystems.append(mf);
			}

			endmntent(mtab);
			mutex.unlock();
			return mountedFilesystems;
		}

		QFileInfo CoreBase::getDeviceMountPoint(QFileInfo &devicePath)
		{
			QList<QSharedPointer<MountedFilesystem> > mpl = getMountedFilesystems(devicePath);
			if(mpl.isEmpty())
				 throw DeviceNotMountedException(devicePath);
			return mpl.first()->MountPoint;
		}
        QSharedPointer<GetFileSystemTypesResponse> CoreBase::getFileSystemsSupported(QSharedPointer<GetFileSystemTypesParams> params)
        {
            QSharedPointer<GetFileSystemTypesResponse> response(new GetFileSystemTypesResponse);
            return response;
        }
	}
}
