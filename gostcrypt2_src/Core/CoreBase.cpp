#include <unistd.h>
#include <mntent.h>
#include "CoreBase.h"
#include "CoreUser.h"
#include "CoreRoot.h"
#include "CoreResponse.h"
#include "CoreRequest.h"
#include "CoreException.h"
#include <QMutex>
#include <QTextStream>
#include <QDebug>
#include <QString>
#include <QStandardPaths>

#include <grp.h>

#include <sys/types.h>
#include <pwd.h>

namespace GostCrypt {
	namespace Core {

        QSharedPointer<CoreBase> getCore()
		{
			initCoreRequest();
			initCoreResponse();
            initGostCryptException();

			INIT_SERIALIZE(ProgressUpdateResponse);


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

		QSharedPointer<GetEncryptionAlgorithmsResponse> CoreBase::getEncryptionAlgorithms(QSharedPointer<GetEncryptionAlgorithmsRequest> params)
		{
            try {

            QSharedPointer<GetEncryptionAlgorithmsResponse> response(new GetEncryptionAlgorithmsResponse());
            			if(!params.isNull())
				response->passThrough = params->passThrough;
			GostCrypt::Volume::EncryptionAlgorithmList algorithms = GostCrypt::Volume::EncryptionAlgorithm::GetAvailableAlgorithms ();
			(void)params;
            for(GostCrypt::Volume::EncryptionAlgorithmList::iterator algorithm = algorithms.begin(); algorithm != algorithms.end(); ++algorithm)
			{
				if (!(*algorithm)->IsDeprecated()){ // we don't allow deprecated algorithms
                    response->algorithms.append(QString::fromStdWString((*algorithm)->GetName()));
                }
			}

			if(params->emitResponse)
				emit sendGetEncryptionAlgorithms(response);
			return response;

            } catch(GostCryptException &e) {
                e.setRequestId(params->id.requestId);
                throw;
            }
		}

        QSharedPointer<GetDerivationFunctionsResponse> CoreBase::getDerivationFunctions(QSharedPointer<GetDerivationFunctionsRequest> params)
        {
            try {

            QSharedPointer<GetDerivationFunctionsResponse> response(new GetDerivationFunctionsResponse);
            			if(!params.isNull())
				response->passThrough = params->passThrough;

            GostCrypt::Volume::VolumeHashList pkcss = GostCrypt::Volume::VolumeHash::GetAvailableAlgorithms();

            (void)params;

            for (GostCrypt::Volume::VolumeHashList::iterator pkcs = pkcss.begin(); pkcs != pkcss.end(); ++pkcs)
            {
                if (!(*pkcs)->IsDeprecated()){ // we don't allow deprecated algorithms
                    response->algorithms.append(QString::fromStdWString((*pkcs)->GetName()));
                }
            }

           	if(params->emitResponse)
	            emit sendGetDerivationFunctions(response);
	        return response;

            } catch(GostCryptException &e) {
                e.setRequestId(params->id.requestId);
                throw;
            }
        }

		QSharedPointer<GetHostDevicesResponse> CoreBase::getHostDevices(QSharedPointer<GetHostDevicesRequest> params)
		{
            try {
            QSharedPointer<GetHostDevicesResponse> response(new GetHostDevicesResponse);

			if(!params.isNull())
				response->passThrough = params->passThrough;
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
                    || fields.at(2) == "1"			// skip extended partitions
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
				if(!response->hostDevices.isEmpty()) {
                    if(hd->devicePath->absoluteFilePath().startsWith(response->hostDevices.last()->devicePath->absoluteFilePath())) {
						response->hostDevices.last()->partitions.append(hd);
						continue;
					}
				}

				response->hostDevices.append(hd);
			}
			file.close();

			if(params->emitResponse)
				emit sendGetHostDevices(response);

			return response;

            } catch(GostCryptException &e) {
                e.setRequestId(params->id.requestId);
                throw;
            }
		}

		QSharedPointer<GetMountedVolumesResponse> CoreBase::getMountedVolumes(QSharedPointer<GetMountedVolumesRequest> params)
		{
            try {

            QSharedPointer<GetMountedVolumesResponse> response(new GetMountedVolumesResponse);

			if(!params.isNull())
				response->passThrough = params->passThrough;
			for(QSharedPointer<MountedFilesystem> mf : getMountedFilesystems()) {
				/* Filter only Fuse FileSystems*/
                if(!mf->MountPoint->absoluteFilePath().startsWith(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QStringLiteral("/" GOSTCRYPT_FUSE_MOUNT_DIR_PREFIX))) {
					continue;
				}

				QSharedPointer<Volume::VolumeInformation> mountedVol(new Volume::VolumeInformation);

				try
				{
					QFile controlFile(mf->MountPoint->absoluteFilePath() + FuseDriver::getControlPath());
					if(!controlFile.open(QIODevice::ReadOnly)) {
						continue;
					}
					QDataStream stream(&controlFile);
					stream.startTransaction();

					stream >> mountedVol;
					if(!stream.commitTransaction())
						continue;
					if(mountedVol.isNull())
						continue;
                                        if(!mountedVol->volumePath.exists())
						continue;
				}
				catch (...)
				{
					continue;
				}

				/* If specific volume asked, check if this is the one */
                if(params && !params->all && mountedVol->volumePath.absoluteFilePath() != params->volumePath.absoluteFilePath())
					continue;

				/* Adding Fuse mount point information thanks to previous found mounted filesystem */
                mountedVol->fuseMountPoint = mf->MountPoint;

				/* Add final mount point information if possible */
                try {
                    if(!mountedVol->virtualDevice.isNull())
						mountedVol->mountPoint = getDeviceMountPoint(mountedVol->virtualDevice);
                } catch(DeviceNotMounted) {} //There is no mountpoint since the virtual device is not mounted

				response->volumeInfoList.append(mountedVol);

				/* If volume path specified no need to stay in the loop */
                if(params && !params->volumePath.absoluteFilePath().isEmpty())
					break;
			}

			if(params->emitResponse)
				emit sendGetMountedVolumes(response);
			return response;

            } catch(GostCryptException &e) {
                e.setRequestId(params->id.requestId);
                throw;
            }
		}

		QList<QSharedPointer<MountedFilesystem>> CoreBase::getMountedFilesystems(const QFileInfo &devicePath, const QFileInfo &mountPoint)
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
                    mf->Type = QString(entry->mnt_type);
				}

                if ((devicePath.absoluteFilePath().isEmpty() || devicePath == *mf->Device) && \
                        (mountPoint.absoluteFilePath().isEmpty() || mountPoint == *mf->MountPoint))
					mountedFilesystems.append(mf);
			}

			endmntent(mtab);
			mutex.unlock();

            return mountedFilesystems;
        }

        QSharedPointer<GostCrypt::Volume::EncryptionAlgorithm> CoreBase::getEncryptionAlgorithm(QString algorithm)
        {
            GostCrypt::Volume::EncryptionAlgorithmList eas = GostCrypt::Volume::EncryptionAlgorithm::GetAvailableAlgorithms();
            for (GostCrypt::Volume::EncryptionAlgorithmList::iterator ea = eas.begin(); ea != eas.end(); ++ea)
            {
                if (!(*ea)->IsDeprecated()){ // we don't allow deprecated algorithms
                    if(algorithm.compare(QString::fromStdWString((*ea)->GetName()), Qt::CaseInsensitive))
                        return *ea;
                }
            }
            throw AlgorithmNotFoundException(algorithm);
        }

        QSharedPointer<Volume::VolumeHash> CoreBase::getDerivationKeyFunction(QString function)
        {

            GostCrypt::Volume::VolumeHashList pkcss = GostCrypt::Volume::VolumeHash::GetAvailableAlgorithms();
            for (GostCrypt::Volume::VolumeHashList::iterator pkcs = pkcss.begin(); pkcs != pkcss.end(); ++pkcs)
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
            QSharedPointer<GetHostDevicesRequest> params(new GetHostDevicesRequest());
            params->emitResponse = false;
            response = getHostDevices(params);
            for(QSharedPointer<GostCrypt::Core::HostDevice> d : response->hostDevices) {
                if(d->devicePath->canonicalFilePath() == path)
                    return true;
            }
            return false;
        }

        void CoreBase::randomizeEncryptionAlgorithmKey (QSharedPointer <Volume::EncryptionAlgorithm> encryptionAlgorithm) const
        {
            SecureBuffer eaKey (encryptionAlgorithm->GetKeySize());
            RandomNumberGenerator::GetData (eaKey);
            encryptionAlgorithm->SetKey (eaKey);

            SecureBuffer modeKey (encryptionAlgorithm->GetMode()->GetKeySize());
            RandomNumberGenerator::GetData (modeKey);
            encryptionAlgorithm->GetMode()->SetKey (modeKey);
        }

        void CoreBase::createRandomFile(QFileInfo path, quint64 size, QString algorithm, bool random)
        {
            std::fstream file;

            file.open(path.absoluteFilePath().toStdString(), std::ios::out | std::ios::binary);
            if(!file.is_open())
                throw /* TODO add exception here */;

            QSharedPointer<GostCrypt::Volume::EncryptionAlgorithm> ea (nullptr);
            if(!algorithm.isEmpty()) {
                ea = getEncryptionAlgorithm(algorithm);
                if(!ea)
                    throw /* TODO */;
                ea->SetMode(QSharedPointer<Volume::EncryptionMode>(new Volume::EncryptionModeXTS()));

                // Empty sectors are encrypted with different key to randomize plaintext
                randomizeEncryptionAlgorithmKey (ea);
            }

            quint64 dataFragmentLength = FILE_OPTIMAL_WRITE_SIZE;
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

                bool CoreBase::isVolumeMounted(QFileInfo volumeFile)
		{
			QSharedPointer<GetMountedVolumesRequest> params(new GetMountedVolumesRequest);
            params->volumePath = volumeFile;
            params->all = false;
            params->emitResponse = false;
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

		bool CoreBase::processNonRootRequest(QVariant r)
		{
			HANDLE_REQUEST(GetMountedVolumes, getMountedVolumes)
			else HANDLE_REQUEST(GetEncryptionAlgorithms, getEncryptionAlgorithms)
			else HANDLE_REQUEST(GetHostDevices, getHostDevices)
			else HANDLE_REQUEST(CreateKeyFile, createKeyFile)
            else HANDLE_REQUEST(GetEncryptionAlgorithms, getEncryptionAlgorithms)
            else HANDLE_REQUEST(GetDerivationFunctions, getDerivationFunctions)
            else HANDLE_REQUEST(GetHostDevices, getHostDevices)
			else {
				return false;
			}
			return true;
		}

        QSharedPointer<CreateKeyFileResponse> CoreBase::createKeyFile(QSharedPointer<CreateKeyFileRequest> params) {
            try {

            if(!params)
                throw MissingParamException("params");
            QSharedPointer<CreateKeyFileResponse> response(new CreateKeyFileResponse());
            if(!params.isNull())
				response->passThrough = params->passThrough;

            CoreBase::createRandomFile(params->file, Volume::VolumePassword::MaxSize, "Gost Grasshopper", true); // certain values of MaxSize may no work with encryption AND random

            if(params->emitResponse)
				emit sendCreateKeyFile(QSharedPointer<CreateKeyFileResponse>(nullptr));

			return response;

            } catch(GostCryptException &e) {
                e.setRequestId(params->id.requestId);
                throw;
            }
		}

		uid_t getUserId(QString username)
		{
			struct passwd *passwdPtr;
			passwdPtr = getpwnam(username.toLocal8Bit().data());
			if(!passwdPtr)
				throw InvalidParamException("mountForUser");
			return passwdPtr->pw_uid;
		}

		gid_t getGroupId(QString groupname)
		{
			struct group *groupPtr;
			groupPtr = getgrnam(groupname.toLocal8Bit().data());
			if(!groupPtr)
				throw InvalidParamException("mountForGroup");
			return groupPtr->gr_gid;
		}
	}
}
