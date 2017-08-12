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
#include <QStandardPaths>

#include "Platform/FileStream.h"
#include "Platform/SharedPtr.h"
#include "FuseDriver/FuseService.h"

#include <sys/types.h>
#include <pwd.h>

namespace GostCrypt {
	namespace NewCore {

        QSharedPointer<CoreBase> getCore()
		{
			initCoreParams();
			initCoreResponse();
			initCoreException();

            //for debug purpose
            //return QSharedPointer<CoreBase>(new CoreUser());

            if(getuid()) {
                return QSharedPointer<CoreBase>(new CoreUser());
			} else {
                return QSharedPointer<CoreBase>(new CoreRoot());
			}
		}

		CoreBase::CoreBase(QObject *parent) : QObject(parent)
		{
			RandomNumberGenerator::Start();
		}



        QSharedPointer<GetDerivationFunctionsResponse> CoreBase::getDerivationFunctions(QSharedPointer<GetDerivationFunctionsParams> params)
        {
            QSharedPointer<GetDerivationFunctionsResponse> response(new GetDerivationFunctionsResponse);
            GostCrypt::Pkcs5KdfList pkcss = GostCrypt::Pkcs5Kdf::GetAvailableAlgorithms();

            (void)params;
            for (GostCrypt::Pkcs5KdfList::iterator pkcs = pkcss.begin(); pkcs != pkcss.end(); pkcs++)
            {
                if (!(*pkcs)->IsDeprecated()){ // we don't allow deprecated algorithms
                    response->algorithms.append(QString::fromStdWString((*pkcs)->GetName()));
                }
            }
            return response;
        }

		QSharedPointer<GetHostDevicesResponse> CoreBase::getHostDevices(QSharedPointer<GetHostDevicesParams> params)
		{
			(void)params;
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
                hd->devicePath.reset(new QFileInfo((fields.at(3).startsWith("/dev/") ? "" : "/dev/") + fields.at(3)));
				hd->size = fields.at(2).toULongLong(&isNumber)*1024;
				if(!isNumber)
                    qDebug() << "Fail to read device size for device " << hd->devicePath->absoluteFilePath();
				try {
					hd->mountPoint = getDeviceMountPoint(hd->devicePath);
				} catch(DeviceNotMounted &e) {}

				/* Check if device is partition */
				if(!res->hostDevices.isEmpty()) {
                    if(hd->devicePath->absoluteFilePath().startsWith(res->hostDevices.last()->devicePath->absoluteFilePath())) {
						res->hostDevices.last()->partitions.append(hd);
						continue;
					}
				}

				res->hostDevices.append(hd);
			}
			file.close();

			return res;
		}

		QSharedPointer<GetMountedVolumesResponse> CoreBase::getMountedVolumes(QSharedPointer<GetMountedVolumesParams> params)
		{
			QSharedPointer<GetMountedVolumesResponse> response(new GetMountedVolumesResponse);
			for(QSharedPointer<MountedFilesystem> mf : getMountedFilesystems()) {
				/* Filter only Fuse FileSystems*/
                if(!mf->MountPoint->absoluteFilePath().startsWith(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QStringLiteral("/" GOSTCRYPT_FUSE_MOUNT_DIR_PREFIX))) {
					continue;
				}

				QSharedPointer<VolumeInfo> mountedVol;

				/* TODO : Replace by Qt serialization in the future */
				try
				{
					shared_ptr <File> controlFile (new File);
                    controlFile->Open (mf->MountPoint->absoluteFilePath().toStdString() + FuseService::GetControlPath());

					shared_ptr <Stream> controlFileStream (new FileStream (controlFile));
					mountedVol.reset(new VolumeInfo(*Serializable::DeserializeNew <VolumeInfo> (controlFileStream)));
				}
				catch (...)
				{
					continue;
				}

				/* If specific volume asked, check if this is the one */
                if(params && params->volumePath && !params->volumePath->absoluteFilePath().isEmpty() && mountedVol->Path != VolumePath(params->volumePath->absoluteFilePath().toStdWString()))
					continue;

				/* Adding Fuse mount point information thanks to previous found mounted filesystem */
                mountedVol->AuxMountPoint = DirectoryPath(mf->MountPoint->absoluteFilePath().toStdString());

				/* Add final mount point information if possible */
                try {
                    if(!mountedVol->VirtualDevice.IsEmpty())
                        mountedVol->MountPoint = DirectoryPath(
                                    getDeviceMountPoint(
                                        QSharedPointer<QFileInfo>(
                                            new QFileInfo(
                                                QString::fromStdString(
                                                    string(
                                                        mountedVol->VirtualDevice
                                                        )
                                                    )
                                                )
                                            )
                                        )->absoluteFilePath().toStdString()
                                    );
                } catch(DeviceNotMounted) {} //There is no mountpoint, the virtual device is not mounted

				response->volumeInfoList.append(mountedVol);

				/* If volume path specified no need to stay in the loop */
                if(params && params->volumePath && !params->volumePath->absoluteFilePath().isEmpty())
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
                    mf->Device.reset(new QFileInfo (QString(entry->mnt_fsname)));
				else
					continue;

				if (entry->mnt_dir)
                    mf->MountPoint.reset(new QFileInfo(QString(entry->mnt_dir)));

				if (entry->mnt_type) {
                    mf->Type.reset(new QString(entry->mnt_type));
				}

                if ((devicePath.absoluteFilePath().isEmpty() || devicePath == *mf->Device) && \
                        (mountPoint.absoluteFilePath().isEmpty() || mountPoint == *mf->MountPoint))
					mountedFilesystems.append(mf);
			}

			endmntent(mtab);
			mutex.unlock();
            return mountedFilesystems;
        }

        QSharedPointer<GostCrypt::EncryptionAlgorithm> CoreBase::getEncryptionAlgorithm(QString algorithm)
        {
            GostCrypt::EncryptionAlgorithmList eas = GostCrypt::EncryptionAlgorithm::GetAvailableAlgorithms();
            for (GostCrypt::EncryptionAlgorithmList::iterator ea = eas.begin(); ea != eas.end(); ea++)
            {
                if (!(*ea)->IsDeprecated()){ // we don't allow deprecated algorithms
                    if(algorithm.compare(QString::fromStdWString((*ea)->GetName()), Qt::CaseInsensitive))
                        return *ea;
                }
            }
            throw AlgorithmNotFoundException(algorithm);
        }

        QSharedPointer<Pkcs5Kdf> CoreBase::getDerivationKeyFunction(QString function)
        {
            GostCrypt::Pkcs5KdfList pkcss = GostCrypt::Pkcs5Kdf::GetAvailableAlgorithms();
            for (GostCrypt::Pkcs5KdfList::iterator pkcs = pkcss.begin(); pkcs != pkcss.end(); pkcs++)
            {
                if (!(*pkcs)->IsDeprecated()){ // we don't allow deprecated algorithms
                    if(function.compare(QString::fromStdWString((*pkcs)->GetName()), Qt::CaseInsensitive))
                        return *pkcs;
                }
            }
            throw AlgorithmNotFoundException(function);
        }

        QSharedPointer<QFileInfo> CoreBase::getDeviceMountPoint(const QSharedPointer<QFileInfo> &devicePath)
		{
            QList<QSharedPointer<MountedFilesystem> > mpl = getMountedFilesystems(*devicePath);
			if(mpl.isEmpty())
                 throw DeviceNotMountedException(devicePath);
            return mpl.first()->MountPoint;
        }

        bool CoreBase::isDevice(QString path)
        {
            QSharedPointer<GetHostDevicesResponse> response;
            response = getHostDevices();
            for(QSharedPointer<GostCrypt::NewCore::HostDevice> d : response->hostDevices) {
                if(d->devicePath->canonicalFilePath() == path)
                    return true;
            }
            return false;
        }

        void CoreBase::randomizeEncryptionAlgorithmKey (QSharedPointer <EncryptionAlgorithm> encryptionAlgorithm) const
        {
            SecureBuffer eaKey (encryptionAlgorithm->GetKeySize());
            RandomNumberGenerator::GetData (eaKey);
            encryptionAlgorithm->SetKey (eaKey);

            SecureBuffer modeKey (encryptionAlgorithm->GetMode()->GetKeySize());
            RandomNumberGenerator::GetData (modeKey);
            encryptionAlgorithm->GetMode()->SetKey (modeKey);
        }

        void CoreBase::createRandomFile(QSharedPointer<QFileInfo> path, quint64 size, QString algorithm, bool random)
        {
            fstream file;

            if(!path)
                 throw MissingParamException("path");

            file.open(path->absoluteFilePath().toStdString(), ios::out | ios::binary);
            if(!file.is_open())
                throw /* TODO add exception here */;

            QSharedPointer<GostCrypt::EncryptionAlgorithm> ea (nullptr);
            if(!algorithm.isEmpty()) {
                ea = getEncryptionAlgorithm(algorithm);
                if(!ea)
                    throw /* TODO */;
                ea->SetMode(shared_ptr<EncryptionMode>(new EncryptionModeXTS()));

                // Empty sectors are encrypted with different key to randomize plaintext
                randomizeEncryptionAlgorithmKey (ea);
            }

            quint64 dataFragmentLength = File::GetOptimalWriteSize(); // TODO define

            // we can't get more than the pool size a each run. Very slow.
            if(random && dataFragmentLength > RandomNumberGenerator::PoolSize) // TODO maybe find a better way ?
                dataFragmentLength = RandomNumberGenerator::PoolSize;

            SecureBuffer outputBuffer (dataFragmentLength);
            quint64 offset = 0; // offset where the data starts
            quint64 sizetodo = size; // size of the data to override

            while (sizetodo > 0)
            {
                if (sizetodo < dataFragmentLength)
                    dataFragmentLength = sizetodo;//RandomNumberGenerator::PoolSize

                if(random)
                    RandomNumberGenerator::GetData(outputBuffer); // getting random data
                else
                    outputBuffer.Zero();

                if(ea)
                    ea->EncryptSectors (outputBuffer.Ptr(), offset / ENCRYPTION_DATA_UNIT_SIZE, dataFragmentLength / ENCRYPTION_DATA_UNIT_SIZE, ENCRYPTION_DATA_UNIT_SIZE); // encrypting it
                file.write((char *)outputBuffer.Ptr(), (size_t) dataFragmentLength); // writing it

                offset += dataFragmentLength;
                sizetodo -= dataFragmentLength;
            }

        }

		bool CoreBase::isVolumeMounted(QSharedPointer<QFileInfo> volumeFile)
		{
			QSharedPointer<GetMountedVolumesParams> params(new GetMountedVolumesParams);
            params->volumePath = volumeFile;
			return !getMountedVolumes(params)->volumeInfoList.isEmpty();
		}

		QSharedPointer<QFileInfo> CoreBase::getFreeFuseMountPoint()
		{
				QList<QSharedPointer<MountedFilesystem>> mountedFilesystems = getMountedFilesystems();

				for (quint32 i = 1; true; i++) {
					try {
                        QString path(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QStringLiteral("/" GOSTCRYPT_FUSE_MOUNT_DIR_PREFIX) + QString::number(i));

						for (QSharedPointer<MountedFilesystem> mountedFilesystem : mountedFilesystems) {
                            if(mountedFilesystem->MountPoint->absoluteFilePath() == path)
								throw MountPointUsedException(mountedFilesystem->MountPoint);
						}

						return QSharedPointer<QFileInfo>(new QFileInfo(path));

					} catch (MountPointUsed &e) {
						if(i < 100)
							continue;
						throw FailedCreateFuseMountPointException(e.getMountpoint());
					}
                }
        }

        QSharedPointer<QFileInfo> CoreBase::getFreeDefaultMountPoint(uid_t userId)
        {
            passwd* userinfo = getpwuid(userId);
            QString mountPointbase = QStringLiteral("/media/") + QString(userinfo->pw_name) + QStringLiteral("/gostcrypt");
            QList<QSharedPointer<MountedFilesystem>> mountedFilesystems = getMountedFilesystems();

            for (quint32 i = 1; true; i++) {
                try {
                    QString path = mountPointbase + QString::number(i);

                    for (QSharedPointer<MountedFilesystem> mountedFilesystem : mountedFilesystems) {
                        if(mountedFilesystem->MountPoint->absoluteFilePath() == path)
                            throw MountPointUsedException(mountedFilesystem->MountPoint);
                    }

                    return QSharedPointer<QFileInfo>(new QFileInfo(path));

                } catch (MountPointUsed &e) {
                    if(i < 100)
                        continue;
                    throw FailedCreateFuseMountPointException(e.getMountpoint());
                }
            }
        }

        QSharedPointer<GetFileSystemsTypesSupportedResponse> CoreBase::getFileSystemsTypesSupported(QSharedPointer<GetFileSystemsTypesSupportedParams> params)
        {
            QSharedPointer<GetFileSystemsTypesSupportedResponse> response(new GetFileSystemsTypesSupportedResponse);
			QFile file("/proc/filesystems");

			if(!file.open(QFile::ReadOnly))
				throw FailedOpenFileException(QFileInfo("/proc/filesystems"));
			QByteArray fileContent = file.readAll();
			QTextStream ts(&fileContent);
			while(!ts.atEnd()) {
				QStringList fields = ts.readLine().split("\t");

				if(fields.count() != 2
					|| fields.at(0) == "nodev") // We filter pseudo filesystems
					continue;

				if(params && !params->volumetypefilter.isEmpty() && fields[1] != params->volumetypefilter)
					continue;

				response->filesystems.append(fields[1]);
			}
			file.close();

            return response;
        }

        QSharedPointer<CreateKeyFileResponse> CoreBase::createKeyFile(QSharedPointer<CreateKeyFileParams> params) {
            if(!params)
                throw MissingParamException("params");
            CoreBase::createRandomFile(params->file, VolumePassword::MaxSize, "Gost Grasshopper", true); // certain values of MaxSize may no work with encryption AND random
            return QSharedPointer<CreateKeyFileResponse>(nullptr); // nothing to return...
        }

	}
}
