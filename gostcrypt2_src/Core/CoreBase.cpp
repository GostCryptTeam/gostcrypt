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

        CoreBase::~CoreBase()
        {
            RandomNumberGenerator::Stop();
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

            QSharedPointer<GostCrypt::Volume::EncryptionAlgorithm> ea;
            if(!algorithm.isEmpty()) {
                ea = getEncryptionAlgorithm(algorithm);
                if(!ea)
                    throw /* TODO */;

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
                    outputBuffer.Erase();

                if(ea)
                    ea->EncryptSectors (outputBuffer.Get(), offset / ENCRYPTION_DATA_UNIT_SIZE, dataFragmentLength / ENCRYPTION_DATA_UNIT_SIZE, ENCRYPTION_DATA_UNIT_SIZE); // encrypting it
                file.write((char *)outputBuffer.Get(), (size_t) dataFragmentLength); // writing it

                offset += dataFragmentLength;
                sizetodo -= dataFragmentLength;
            }

        }

        QSharedPointer<ChangeVolumePasswordResponse> CoreBase::changeVolumePassword(QSharedPointer<ChangeVolumePasswordRequest> params)
        {
            try {

            QSharedPointer<ChangeVolumePasswordResponse> response(new ChangeVolumePasswordResponse());
            if(!params.isNull())
                response->passThrough = params->passThrough;

            if(params->changeMasterKey)
                throw IncorrectParameterException("Not implemented yet"); //TODO

            //Open Volume
            QSharedPointer<Volume::VolumePassword> password;
            QSharedPointer <Volume::KeyfileList> keyfiles;
            QSharedPointer<Volume::Volume> volume;

            // Conversions :(
            if(!params->password.isNull())
                password.reset(new Volume::VolumePassword(params->password->constData(), params->password->size()));
            else
                throw MissingParamException("password");
            if(!params->keyfiles.isNull()) {
                for(QSharedPointer<QFileInfo> keyfile : *params->keyfiles) {
                    keyfiles->append(QSharedPointer<Volume::Keyfile>(new Volume::Keyfile(*keyfile)));
                }
            }

            volume->Open(
                params->path,
                false,
                password,
                keyfiles,
                Volume::VolumeProtection::Enum::None,
                QSharedPointer <Volume::VolumePassword> (), QSharedPointer <Volume::KeyfileList> ()
            );

            params->password->fill('\0');

            QSharedPointer<Volume::VolumeHash> newVolumeHeaderKdf = getDerivationKeyFunction(params->newVolumeHeaderKdf);
            Buffer newSalt(volume->GetSaltSize());
            SecureBuffer newHeaderKey(Volume::VolumeHeader::GetLargestSerializedKeySize());

            RandomNumberGenerator::SetHash(newVolumeHeaderKdf);

            // Conversions :(
            Volume::VolumePassword newPassword(params->newPassword->data(), params->newPassword->size());

            bool backupHeader = false;
            while(true) {
                for (int i = 1; i <= PRAND_DISK_WIPE_PASSES ; i++) {
                //TODO AskFiliol, Why several dummy header write (SecureWipePassCount)
                    if(i == PRAND_DISK_WIPE_PASSES)
                        RandomNumberGenerator::GetData(newSalt);
                    else
                        RandomNumberGenerator::GetDataFast(newSalt);

                    newVolumeHeaderKdf->HMAC_DeriveKey(newHeaderKey, newPassword, newSalt);
                    volume->ReEncryptHeader(backupHeader, newSalt, newHeaderKey, newVolumeHeaderKdf);

                    volume->GetFile()->Flush();
                }

                if(!volume->GetLayout()->HasBackupHeader() || backupHeader)
                    break;

                backupHeader = true;
            }

            volume->Close();

            if(params->emitResponse)
                emit sendChangeVolumePassword(response);

            return response;

            } catch(GostCryptException &e) {
                e.setRequestId(params->id.requestId);
                throw;
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

        void CoreBase::ReEncryptVolumeHeaderWithNewSalt(BufferPtr &newHeaderBuffer, QSharedPointer<Volume::VolumeHeader> header, QSharedPointer<Volume::VolumePassword> password, QSharedPointer<Volume::KeyfileList> keyfiles) const
        {
            QSharedPointer <Volume::VolumeHash> hash = header->GetVolumeHash();

            RandomNumberGenerator::SetHash (hash);

            SecureBuffer newSalt (header->GetSaltSize());
            SecureBuffer newHeaderKey (Volume::VolumeHeader::GetLargestSerializedKeySize());

            QSharedPointer <Volume::VolumePassword> passwordKey (Volume::Keyfile::ApplyListToPassword (keyfiles, password));

            RandomNumberGenerator::GetData (newSalt);
            hash->HMAC_DeriveKey(newHeaderKey, *passwordKey, newSalt);

            header->EncryptNew (newHeaderBuffer, newSalt, newHeaderKey, hash);
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
            else HANDLE_REQUEST(ChangeVolumePassword, changeVolumePassword)
            else HANDLE_REQUEST(BackupHeader, backupHeader)
            else HANDLE_REQUEST(RestoreHeader, restoreHeader)
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

        QSharedPointer<BackupHeaderResponse> CoreBase::backupHeader(QSharedPointer<BackupHeaderRequest> params)
        {
            try {
                QSharedPointer<BackupHeaderResponse> response(new BackupHeaderResponse);

                QSharedPointer<Volume::Volume> normalVolume;
                QSharedPointer<Volume::Volume> hiddenVolume;

                QSharedPointer<Volume::VolumePassword> password;
                QSharedPointer <Volume::KeyfileList> keyfiles;
                QSharedPointer<Volume::VolumePassword> hiddenVolumePassord;
                QSharedPointer <Volume::KeyfileList> hiddenVolumeKeyfiles;


                // Conversions :(
                if(!params->password.isNull())
                    password.reset(new Volume::VolumePassword(params->password->constData(), params->password->size()));
                else
                    throw MissingParamException("password");
                if(!params->hiddenVolumeKeyfiles.isNull()) {
                    for(QSharedPointer<QFileInfo> keyfile : *params->hiddenVolumeKeyfiles) {
                        keyfiles->append(QSharedPointer<Volume::Keyfile>(new Volume::Keyfile(*keyfile)));
                    }
                }

                try {
                    normalVolume->Open(params->volumePath, false, password, keyfiles, Volume::VolumeProtection::ReadOnly,QSharedPointer<Volume::VolumePassword>(), QSharedPointer<Volume::KeyfileList>(), Volume::VolumeType::Enum::Normal);
                } catch(...) {
                    //TODO or maybe not  necessary actually since the exception thrown should make sense
                }

                if(params->hiddenVolume) {


                    // Conversion :(
                    if(!params->hiddenVolumePassword.isNull())
                        hiddenVolumePassord.reset(new Volume::VolumePassword(params->hiddenVolumePassword->constData(), params->hiddenVolumePassword->size()));
                    else
                        throw MissingParamException("hiddenVolumePassword");
                    if(!params->hiddenVolumeKeyfiles.isNull()) {
                        for(QSharedPointer<QFileInfo> keyfile : *params->hiddenVolumeKeyfiles) {
                            hiddenVolumeKeyfiles->append(QSharedPointer<Volume::Keyfile>(new Volume::Keyfile(*keyfile)));
                        }
                    }

                    try {
                        hiddenVolume->Open(params->volumePath, false, hiddenVolumePassord, hiddenVolumeKeyfiles, Volume::VolumeProtection::ReadOnly,QSharedPointer<Volume::VolumePassword>(), QSharedPointer<Volume::KeyfileList>(), Volume::VolumeType::Enum::Hidden);
                    } catch(...) {
                        //TODO
                    }
                }

                QFile backupHeaderFile;
                backupHeaderFile.setFileName(params->backupHeaderFile.absoluteFilePath());
                if(!backupHeaderFile.open(QIODevice::WriteOnly))
                    throw FailedOpenFileException(QFileInfo(params->backupHeaderFile));

                SecureBuffer newHeaderBuffer(normalVolume->GetLayout()->GetHeaderSize());

                //Rencryt volume header with new salt
                ReEncryptVolumeHeaderWithNewSalt(newHeaderBuffer, normalVolume->GetHeader(), password, keyfiles);
                normalVolume->Close();
                backupHeaderFile.write(reinterpret_cast<char *> (newHeaderBuffer.Get()), newHeaderBuffer.Size());

                if(params->hiddenVolume) {
                    ReEncryptVolumeHeaderWithNewSalt(newHeaderBuffer, hiddenVolume->GetHeader(), hiddenVolumePassord, hiddenVolumeKeyfiles);
                    hiddenVolume->Close();
                } else {
                    // Store random data in place of hidden volume (normal volume backuped header reencrypted with a random key)
                    QSharedPointer<Volume::EncryptionAlgorithm> ea = normalVolume->GetEncryptionAlgorithm();
                    randomizeEncryptionAlgorithmKey(ea);
                    ea->Encrypt(newHeaderBuffer);
                }

                backupHeaderFile.write(reinterpret_cast<char *> (newHeaderBuffer.Get()), newHeaderBuffer.Size());

                backupHeaderFile.close();

                if(params->emitResponse)
                    emit sendBackupHeader(response);
                return response;
            } catch(GostCryptException &e) {
                e.setRequestId(params->id.requestId);
                throw;
            }
        }

        QSharedPointer<RestoreHeaderResponse> CoreBase::restoreHeader(QSharedPointer<RestoreHeaderRequest> params)
        {
            try {
                QSharedPointer<RestoreHeaderResponse> response(new RestoreHeaderResponse);

                //TODO cf TextUserInterface.cpp:1258

                if(params->emitResponse)
                    emit sendRestoreHeader(response);
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
