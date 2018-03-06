#include <unistd.h>
#include <mntent.h>
#include "CoreBase.h"
#include "CoreUser.h"
#include "CoreRoot.h"
#include "CoreResponse.h"
#include "CoreRequest.h"
#include "CoreException.h"
#include "Volume/EncryptionThreadPool.h"
#include <QMutex>
#include <QTextStream>
#include <QDebug>
#include <QString>
#include <QStandardPaths>
#include <QElapsedTimer>

#include <grp.h>

#include <sys/types.h>
#include <pwd.h>

namespace GostCrypt
{
namespace Core
{

QSharedPointer<CoreBase> getCore()
{
    initCoreRequest();
    initCoreResponse();
    initGostCryptException();

    if (getuid())
    {
        return QSharedPointer<CoreBase>(new CoreUser());
    }
    else
    {
        return QSharedPointer<CoreBase>(new CoreRoot());
    }
}

CoreBase::CoreBase(QObject* parent) : QObject(parent)
{
    RandomGenerator::Start();
}

CoreBase::~CoreBase()
{
    RandomGenerator::Stop();
}

QSharedPointer<GetEncryptionAlgorithmsResponse> CoreBase::getEncryptionAlgorithms(
    QSharedPointer<GetEncryptionAlgorithmsRequest> params)
{
    QSharedPointer<GetEncryptionAlgorithmsResponse> response(new GetEncryptionAlgorithmsResponse());
    try
    {
        if (!params.isNull())
        {
            response->passThrough = params->passThrough;
        }
        GostCrypt::Volume::EncryptionAlgorithmList algorithms =
            GostCrypt::Volume::EncryptionAlgorithm::GetAvailableAlgorithms();

        for (GostCrypt::Volume::EncryptionAlgorithmList::iterator algorithm = algorithms.begin();
                algorithm != algorithms.end(); ++algorithm)
        {
            if (!(*algorithm)->IsDeprecated())  // we don't allow deprecated algorithms
            {
                response->algorithms.append((*algorithm)->GetName());
            }
        }
    }
    catch (GostCryptException& e)
    {
        e.clone(params->id.requestId)->raise();
    }

    if (params->emitResponse)
    {
        emit sendGetEncryptionAlgorithms(response);
    }
    return response;
}

QSharedPointer<GetDerivationFunctionsResponse> CoreBase::getDerivationFunctions(
    QSharedPointer<GetDerivationFunctionsRequest> params)
{
    QSharedPointer<GetDerivationFunctionsResponse> response(new GetDerivationFunctionsResponse);
    try
    {

        if (!params.isNull())
        {
            response->passThrough = params->passThrough;
        }

        GostCrypt::Volume::VolumeHashList pkcss = GostCrypt::Volume::VolumeHash::GetAvailableAlgorithms();


        for (GostCrypt::Volume::VolumeHashList::iterator pkcs = pkcss.begin(); pkcs != pkcss.end(); ++pkcs)
        {
            if (!(*pkcs)->IsDeprecated())  // we don't allow deprecated algorithms
            {
                response->algorithms.append((*pkcs)->GetName());
            }
        }
    }
    catch (GostCryptException& e)
    {
        e.clone(params->id.requestId)->raise();
    }
    if (params.isNull() || params->emitResponse)
    {
        emit sendGetDerivationFunctions(response);
    }
    return response;
}

QSharedPointer<GetHostDevicesResponse> CoreBase::getHostDevices(
    QSharedPointer<GetHostDevicesRequest> params)
{
    QSharedPointer<GetHostDevicesResponse> response(new GetHostDevicesResponse);
    try
    {

        if (!params.isNull())
        {
            response->passThrough = params->passThrough;
        }
        QFile file("/proc/partitions");

        if (!file.open(QFile::ReadOnly))
        {
            throw FailedOpenFileException(QFileInfo("/proc/partitions"));
        }
        QByteArray fileContent = file.readAll();
        QTextStream ts(&fileContent);
        while (!ts.atEnd())
        {
            QStringList fields = ts.readLine().trimmed().split(" ", QString::SkipEmptyParts);

            if (fields.count() != 4
                    || fields.at(3).startsWith("loop")  // skip loop devices
                    || fields.at(3).startsWith("cloop")
                    || fields.at(3).startsWith("ram")   // skip RAM devices
                    || fields.at(3).startsWith("dm-")   // skip device mapper devices
                    || fields.at(2) == "1"          // skip extended partitions
               )
            {
                continue;
            }

            bool isNumber;
            fields.at(0).toInt(&isNumber);
            if (!isNumber)
            {
                continue;
            }
            QSharedPointer<HostDevice> hd(new HostDevice());
            hd->devicePath.setFile((fields.at(3).startsWith("/dev/") ? "" : "/dev/") + fields.at(3));
            hd->size = fields.at(2).toULongLong(&isNumber) * 1024;
            if (!isNumber)
            {
                qDebug() << "Fail to read device size for device " << hd->devicePath.absoluteFilePath();
            }
            try
            {
                hd->mountPoint = getDeviceMountPoint(hd->devicePath);
            }
            catch (DeviceNotMounted& e) {}

            /* Check if device is partition */
            if (!response->hostDevices.isEmpty())
            {
                if (hd->devicePath.absoluteFilePath().startsWith(
                            response->hostDevices.last()->devicePath.absoluteFilePath()))
                {
                    response->hostDevices.last()->partitions.append(hd);
                    continue;
                }
            }

            response->hostDevices.append(hd);
        }
        file.close();



    }
    catch (GostCryptException& e)
    {
        e.clone(params->id.requestId)->raise();
    }

    if (params.isNull() || params->emitResponse)
    {
        emit sendGetHostDevices(response);
    }

    return response;
}

QSharedPointer<GetMountedVolumesResponse> CoreBase::getMountedVolumes(
    QSharedPointer<GetMountedVolumesRequest> params)
{
    QSharedPointer<GetMountedVolumesResponse> response(new GetMountedVolumesResponse);
    try
    {
        if (!params.isNull())
        {
            response->passThrough = params->passThrough;
        }
        for (QSharedPointer<MountedFilesystem> mf : getMountedFilesystems())
        {
            /* Filter only Fuse FileSystems*/
            if (!mf->MountPoint.absoluteFilePath().startsWith(QStandardPaths::writableLocation(
                        QStandardPaths::TempLocation) + QStringLiteral("/" GOSTCRYPT_FUSE_MOUNT_DIR_PREFIX)))
            {
                continue;
            }

            QSharedPointer<Volume::VolumeInformation> mountedVol(new Volume::VolumeInformation);

            try
            {
                QFile controlFile(mf->MountPoint.absoluteFilePath() + FuseDriver::getControlPath());
                if (!controlFile.open(QIODevice::ReadOnly))
                {
                    continue;
                }
                QDataStream stream(&controlFile);
                stream.startTransaction();

                stream >> mountedVol;
                if (!stream.commitTransaction())
                {
                    continue;
                }
                if (mountedVol.isNull())
                {
                    continue;
                }
                if (!mountedVol->volumePath.exists())
                {
                    continue;
                }
            }
            catch (...)
            {
                continue;
            }

            /* If specific volume asked, check if this is the one */
            if (params && !params->all &&
                    mountedVol->volumePath.absoluteFilePath() != params->volumePath.absoluteFilePath())
            {
                continue;
            }

            /* Adding Fuse mount point information thanks to previous found mounted filesystem */
            mountedVol->fuseMountPoint = mf->MountPoint;

            /* Add final mount point information if possible */
            try
            {
                if (!mountedVol->virtualDevice.filePath().isEmpty())
                {
                    mountedVol->mountPoint = getDeviceMountPoint(mountedVol->virtualDevice);
                }
            }
            catch (DeviceNotMounted) {}  //There is no mountpoint since the virtual device is not mounted

            response->volumeInfoList.append(mountedVol);

            /* If volume path specified no need to stay in the loop */
            if (params && !params->volumePath.filePath().isEmpty())
            {
                break;
            }
        }
    }
    catch (GostCryptException& e)
    {
        e.clone(params->id.requestId)->raise();
    }
    if (params.isNull() || params->emitResponse)
    {
        emit sendGetMountedVolumes(response);
    }
    return response;
}

QList<QSharedPointer<MountedFilesystem>> CoreBase::getMountedFilesystems(
        const QFileInfo& devicePath, const QFileInfo& mountPoint)
{
    QList<QSharedPointer<MountedFilesystem>> mountedFilesystems;
    //*
    FILE* mtab = setmntent("/etc/mtab", "r");

    if (!mtab)
    {
        mtab = setmntent("/proc/mounts", "r");
    }
    if (!mtab)
    {
        throw FailedOpenFileException(QFileInfo("/proc/mounts"));
    }

    static QMutex mutex;
    mutex.lock();

    struct mntent* entry;
    while ((entry = getmntent(mtab)) != nullptr)
    {
        QSharedPointer<MountedFilesystem> mf(new MountedFilesystem);

        if (entry->mnt_fsname)
        {
            mf->Device.setFile(QString(entry->mnt_fsname));
        }
        else
        {
            continue;
        }

        if (entry->mnt_dir)
        {
            mf->MountPoint.setFile(QString(entry->mnt_dir));
        }

        if (entry->mnt_type)
        {
            mf->Type = QString(entry->mnt_type);
        }

        if ((devicePath.filePath().isEmpty() || devicePath == mf->Device) && \
                (mountPoint.filePath().isEmpty() || mountPoint == mf->MountPoint))
        {
            mountedFilesystems.append(mf);
        }
    }

    endmntent(mtab);
    mutex.unlock();

    return mountedFilesystems;
}

QSharedPointer<GostCrypt::Volume::EncryptionAlgorithm> CoreBase::getEncryptionAlgorithm(
    QString algorithm)
{
    GostCrypt::Volume::EncryptionAlgorithmList eas =
        GostCrypt::Volume::EncryptionAlgorithm::GetAvailableAlgorithms();
    for (GostCrypt::Volume::EncryptionAlgorithmList::iterator ea = eas.begin(); ea != eas.end(); ++ea)
    {
        if (!(*ea)->IsDeprecated())  // we don't allow deprecated algorithms
        {
            if (algorithm.compare((*ea)->GetName(), Qt::CaseInsensitive))
            {
                return *ea;
            }
        }
    }
    throw AlgorithmNotFoundException(algorithm);
}

QSharedPointer<Volume::VolumeHash> CoreBase::getDerivationKeyFunction(QString function)
{

    GostCrypt::Volume::VolumeHashList pkcss = GostCrypt::Volume::VolumeHash::GetAvailableAlgorithms();
    for (GostCrypt::Volume::VolumeHashList::iterator pkcs = pkcss.begin(); pkcs != pkcss.end(); ++pkcs)
    {
        if (!(*pkcs)->IsDeprecated())  // we don't allow deprecated algorithms
        {
            if (function.compare((*pkcs)->GetName(), Qt::CaseInsensitive))
            {
                return *pkcs;
            }
        }
    }
    throw AlgorithmNotFoundException(function);
}

QFileInfo CoreBase::getDeviceMountPoint(const QFileInfo devicePath)
{
    QList<QSharedPointer<MountedFilesystem> > mpl = getMountedFilesystems(devicePath);
    if (mpl.isEmpty())
    {
        throw DeviceNotMountedException(devicePath);
    }
    return mpl.first()->MountPoint;
}

bool CoreBase::isDevice(QString path)
{
    QSharedPointer<GetHostDevicesResponse> response;
    QSharedPointer<GetHostDevicesRequest> params(new GetHostDevicesRequest());
    params->emitResponse = false;
    response = getHostDevices(params);
    for (QSharedPointer<GostCrypt::Core::HostDevice> d : response->hostDevices)
    {
        if (d->devicePath.canonicalFilePath() == path)
        {
            return true;
        }
    }
    return false;
}

void CoreBase::randomizeEncryptionAlgorithmKey(QSharedPointer <Volume::EncryptionAlgorithm>
        encryptionAlgorithm) const
{
    SecureBuffer eaKey(encryptionAlgorithm->GetKeySize());
    RandomGenerator::GetData(eaKey);
    encryptionAlgorithm->SetKey(eaKey);

    SecureBuffer modeKey(encryptionAlgorithm->GetMode()->GetKeySize());
    RandomGenerator::GetData(modeKey);
    encryptionAlgorithm->GetMode()->SetKey(modeKey);
}

void CoreBase::createRandomFile(QFileInfo path, quint64 size, ProgressTrackingParameters id,
                                QString algorithm, bool random)
{
    std::fstream file;

    updateProgress(0.0, id);
    file.open(path.absoluteFilePath().toStdString(), std::ios::out | std::ios::binary);
    if (!file.is_open())
    {
        throw FailedOpenFileException(path);
    }

    QSharedPointer<GostCrypt::Volume::EncryptionAlgorithm> ea;
    if (!algorithm.isEmpty())
    {
        ea = getEncryptionAlgorithm(algorithm);
        if (!ea)
        {
            throw /* TODO */;
        }

        // Empty sectors are encrypted with different key to randomize plaintext
        randomizeEncryptionAlgorithmKey(ea);
    }

    quint64 dataFragmentLength = FILE_OPTIMAL_WRITE_SIZE;
    // we can't get more than the pool size a each run. Very slow.
    if (random &&
            dataFragmentLength > RandomGenerator::PoolSize) // TODO maybe find a better way ?
    {
        dataFragmentLength = RandomGenerator::PoolSize;
    }

    SecureBuffer outputBuffer(dataFragmentLength);
    quint64 offset = 0; // offset where the data starts
    quint64 sizetodo = size; // size of the data to override

    while (sizetodo > 0)
    {
        if (sizetodo < dataFragmentLength)
        {
            dataFragmentLength = sizetodo;
        }

        if (random)
        {
            RandomGenerator::GetData(outputBuffer);    // getting random data
        }
        else
        {
            outputBuffer.erase();
        }

        if (!algorithm.isEmpty())
        {
            ea->EncryptSectors(outputBuffer.get(), offset / ENCRYPTION_DATA_UNIT_SIZE,
                               dataFragmentLength / ENCRYPTION_DATA_UNIT_SIZE,
                               ENCRYPTION_DATA_UNIT_SIZE);    // TODO : check that if dataFragmentLength is set to minimum (RandomGenerator::PoolSize) there no issue
        }
        file.write((char*)outputBuffer.get(), (size_t) dataFragmentLength);  // writing it

        offset += dataFragmentLength;
        sizetodo -= dataFragmentLength;

        updateProgress(((float)offset / (float)size), id);
    }

}


QSharedPointer<ChangeVolumePasswordResponse> CoreBase::changeVolumePassword(
    QSharedPointer<ChangeVolumePasswordRequest> params)
{
    QSharedPointer<ChangeVolumePasswordResponse> response(new ChangeVolumePasswordResponse());


    try
    {
        if (!params.isNull())
        {
            response->passThrough = params->passThrough;
        }
        if (params->changeMasterKey)
        {
            throw InvalidParameterException("params->changeMasterKey", "Not implemented yet")
        }

        //Open Volume
        QSharedPointer<Volume::VolumePassword> password;
        QSharedPointer <Volume::KeyfileList> keyfiles;
        QSharedPointer<Volume::Volume> volume;


        // Conversions :(
        if (params->password.isNull())
        {
            throw InvalidParameterException("params->password", "Password is null.");
        }

        password.reset(new Volume::VolumePassword(params->password->constData(), params->password->size()));


        for (QFileInfo keyfile : params->keyfiles)
        {
            keyfiles->append(QSharedPointer<Volume::Keyfile>(new Volume::Keyfile(keyfile)));
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

        QSharedPointer<Volume::VolumeHash> newVolumeHeaderKdf = getDerivationKeyFunction(
                    params->newVolumeHeaderKdf);
        Buffer newSalt(volume->GetSaltSize());
        SecureBuffer newHeaderKey(Volume::VolumeHeader::GetLargestSerializedKeySize());

        RandomGenerator::SetHash(newVolumeHeaderKdf);

        /* Conversions */
        QSharedPointer<Volume::VolumePassword> newPassword(new Volume::VolumePassword(
                    params->newPassword->data(), params->newPassword->size()));
        QSharedPointer <Volume::KeyfileList> newKeyfiles;
        for (QFileInfo keyfile : params->newKeyfiles)
        {
            newKeyfiles->append(QSharedPointer<Volume::Keyfile>(new Volume::Keyfile(keyfile)));
        }

        QSharedPointer <Volume::VolumePassword> newPasswordKey = Volume::Keyfile::ApplyListToPassword(
                    newKeyfiles, newPassword);

        bool backupHeader = false;
        while (true)
        {
            for (int i = 1; i <= PRAND_DISK_WIPE_PASSES ; i++)
            {
                //TODO AskFiliol, Why several dummy header write (SecureWipePassCount)
                if (i == PRAND_DISK_WIPE_PASSES)
                {
                    RandomGenerator::GetData(newSalt);
                }
                else
                {
                    RandomGenerator::GetDataFast(newSalt);
                }

                newVolumeHeaderKdf->HMAC_DeriveKey(newHeaderKey, *newPasswordKey, newSalt);
                volume->ReEncryptHeader(backupHeader, newSalt, newHeaderKey, newVolumeHeaderKdf);

                volume->GetFile()->Flush();
            }

            if (!volume->GetLayout()->HasBackupHeader() || backupHeader)
            {
                break;
            }

            backupHeader = true;
        }

        volume->Close();

    }
    catch (GostCryptException& e)
    {
        e.clone(params->id.requestId)->raise();
    }

    if (params.isNull() || params->emitResponse)
    {
        emit sendChangeVolumePassword(response);
    }

    return response;
}

bool CoreBase::isVolumeMounted(QFileInfo volumeFile)
{
    QSharedPointer<GetMountedVolumesRequest> params(new GetMountedVolumesRequest);
    params->volumePath = volumeFile;
    params->all = false;
    params->emitResponse = false;
    return !getMountedVolumes(params)->volumeInfoList.isEmpty();
}

QFileInfo CoreBase::getFreeFuseMountPoint()
{
    QList<QSharedPointer<MountedFilesystem>> mountedFilesystems = getMountedFilesystems();

    for (quint32 i = 1; true; i++)
    {
        try
        {
            QString path(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QStringLiteral("/"
                         GOSTCRYPT_FUSE_MOUNT_DIR_PREFIX) + QString::number(i));

            for (QSharedPointer<MountedFilesystem> mountedFilesystem : mountedFilesystems)
            {
                if (mountedFilesystem->MountPoint.absoluteFilePath() == path)
                {
                    throw MountPointUsedException(mountedFilesystem->MountPoint);
                }
            }

            return QFileInfo(path);

        }
        catch (MountPointUsed& e)
        {
            if (i < 100)
            {
                continue;
            }
            throw FailedCreateFuseMountPointException(e.getMountpoint());
        }
    }
}

QFileInfo CoreBase::getFreeDefaultMountPoint(uid_t userId)
{
    passwd* userinfo = getpwuid(userId);
    QString mountPointbase = QStringLiteral("/media/") + QString(userinfo->pw_name) +
                             QStringLiteral("/gostcrypt");
    QList<QSharedPointer<MountedFilesystem>> mountedFilesystems = getMountedFilesystems();

    for (quint32 i = 1; true; i++)
    {
        try
        {
            QString path = mountPointbase + QString::number(i);

            for (QSharedPointer<MountedFilesystem> mountedFilesystem : mountedFilesystems)
            {
                if (mountedFilesystem->MountPoint.absoluteFilePath() == path)
                {
                    throw MountPointUsedException(mountedFilesystem->MountPoint);
                }
            }


            return QFileInfo(path);

        }
        catch (MountPointUsed& e)
        {
            if (i < 100)
            {
                continue;
            }
            throw;//TODO
        }
    }
}

void CoreBase::ReEncryptVolumeHeaderWithNewSalt(BufferPtr& newHeaderBuffer,
        QSharedPointer<Volume::VolumeHeader> header, QSharedPointer<Volume::VolumePassword> password,
        QSharedPointer<Volume::KeyfileList> keyfiles) const
{
    QSharedPointer <Volume::VolumeHash> hash = header->GetVolumeHash();

    RandomGenerator::SetHash(hash);

    SecureBuffer newSalt(header->GetSaltSize());
    SecureBuffer newHeaderKey(Volume::VolumeHeader::GetLargestSerializedKeySize());

    QSharedPointer <Volume::VolumePassword> passwordKey(Volume::Keyfile::ApplyListToPassword(keyfiles,
            password));

    RandomGenerator::GetData(newSalt);
    hash->HMAC_DeriveKey(newHeaderKey, *passwordKey, newSalt);

    header->EncryptNew(newHeaderBuffer, newSalt, newHeaderKey, hash);
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
                                        else HANDLE_REQUEST(BenchmarkAlgorithms, benchmarkAlgorithms)
                                            else
                                            {
                                                return false;
                                            }
    return true;
}

void CoreBase::updateProgress(qreal progress, ProgressTrackingParameters id)
{
    emit sendProgressUpdate(id.requestId, id.end * progress + id.start * (1 - progress));
}

QSharedPointer<CreateKeyFileResponse> CoreBase::createKeyFile(QSharedPointer<CreateKeyFileRequest>
        params)
{
    QSharedPointer<CreateKeyFileResponse> response(new CreateKeyFileResponse());

    try
    {
        if (params.isNull())
        {
            throw InvalidParameterException("params", "params is null.");
        }
        response->passThrough = params->passThrough;

        CoreBase::createRandomFile(params->file, Volume::VolumePassword::MaxSize, params->id,
                                   "Gost Grasshopper",
                                   true); // TODO certain values of MaxSize may no work with encryption AND random
    }
    catch (GostCryptException& e)
    {
        e.clone(params->id.requestId)->raise();
    }

    if (params->emitResponse)
    {
        emit sendCreateKeyFile(QSharedPointer<CreateKeyFileResponse>(nullptr));
    }

    return response;
}

QSharedPointer<BackupHeaderResponse> CoreBase::backupHeader(QSharedPointer<BackupHeaderRequest>
        params)
{
    QSharedPointer<BackupHeaderResponse> response(new BackupHeaderResponse);
    try
    {

        QSharedPointer<Volume::Volume> normalVolume;
        QSharedPointer<Volume::Volume> hiddenVolume;

        QSharedPointer<Volume::VolumePassword> password;
        QSharedPointer <Volume::KeyfileList> keyfiles;
        QSharedPointer<Volume::VolumePassword> hiddenVolumePassord;
        QSharedPointer <Volume::KeyfileList> hiddenVolumeKeyfiles;


        // Conversions :(
        if (params->password.isNull())
        {
            throw InvalidParameterException("params->password", "Password is null.");
        }


        password.reset(new Volume::VolumePassword(params->password->constData(), params->password->size()));

        for (QFileInfo keyfile : params->keyfiles)
        {
            keyfiles->append(QSharedPointer<Volume::Keyfile>(new Volume::Keyfile(keyfile)));
        }

        try
        {
            normalVolume->Open(params->volumePath, false, password, keyfiles,
                               Volume::VolumeProtection::ReadOnly, QSharedPointer<Volume::VolumePassword>(),
                               QSharedPointer<Volume::KeyfileList>(), Volume::VolumeType::Enum::Normal);
        }
        catch (...)
        {
            //TODO or maybe not  necessary actually since the exception thrown should make sense
        }

        if (params->hiddenVolume)
        {


            // Conversion :(
            if (params->hiddenVolumePassword.isNull())
            {
                throw InvalidParameterException("params->hiddenVolumePassword", "hiddenVolumePassword is null.");
            }

            hiddenVolumePassord.reset(new Volume::VolumePassword(params->hiddenVolumePassword->constData(),
                                      params->hiddenVolumePassword->size()));
            for (QFileInfo keyfile : params->hiddenVolumeKeyfiles)
            {
                hiddenVolumeKeyfiles->append(QSharedPointer<Volume::Keyfile>(new Volume::Keyfile(keyfile)));
            }

            try
            {
                hiddenVolume->Open(params->volumePath, false, hiddenVolumePassord, hiddenVolumeKeyfiles,
                                   Volume::VolumeProtection::ReadOnly, QSharedPointer<Volume::VolumePassword>(),
                                   QSharedPointer<Volume::KeyfileList>(), Volume::VolumeType::Enum::Hidden);
            }
            catch (...)
            {
                //TODO
            }
        }

        QFile backupHeaderFile;
        backupHeaderFile.setFileName(params->backupHeaderFile.absoluteFilePath());
        if (!backupHeaderFile.open(QIODevice::WriteOnly))
        {
            throw FailedOpenFileException(QFileInfo(params->backupHeaderFile));
        }

        SecureBuffer newHeaderBuffer(normalVolume->GetLayout()->GetHeaderSize());

        //Rencryt volume header with new salt
        ReEncryptVolumeHeaderWithNewSalt(newHeaderBuffer, normalVolume->GetHeader(), password, keyfiles);
        normalVolume->Close();
        backupHeaderFile.write(reinterpret_cast<char*>(newHeaderBuffer.get()), newHeaderBuffer.size());

        if (params->hiddenVolume)
        {
            ReEncryptVolumeHeaderWithNewSalt(newHeaderBuffer, hiddenVolume->GetHeader(), hiddenVolumePassord,
                                             hiddenVolumeKeyfiles);
            hiddenVolume->Close();
        }
        else
        {
            // Store random data in place of hidden volume (normal volume backuped header reencrypted with a random key)
            QSharedPointer<Volume::EncryptionAlgorithm> ea = normalVolume->GetEncryptionAlgorithm();
            randomizeEncryptionAlgorithmKey(ea);
            ea->Encrypt(newHeaderBuffer);
        }

        backupHeaderFile.write(reinterpret_cast<char*>(newHeaderBuffer.get()), newHeaderBuffer.size());

        backupHeaderFile.close();

    }
    catch (GostCryptException& e)
    {
        e.clone(params->id.requestId)->raise();
    }
    if (params->emitResponse)
    {
        emit sendBackupHeader(response);
    }
    return response;
}

QSharedPointer<RestoreHeaderResponse> CoreBase::restoreHeader(QSharedPointer<RestoreHeaderRequest>
        params)
{
    QSharedPointer<RestoreHeaderResponse> response(new RestoreHeaderResponse);

    try
    {
        QSharedPointer<Volume::VolumePassword> password;
        QSharedPointer <Volume::KeyfileList> keyfiles;
        QSharedPointer<Volume::Volume> volume;
        QSharedPointer<Volume::VolumeLayout> decryptedLayout;
        QSharedPointer<Volume::VolumeHeader> backupHeader;

        // Conversions :(
        if (!params->password.isNull())
        {
            throw InvalidParameterException("params->password", "Password is null.");
        }

        password.reset(new Volume::VolumePassword(params->password->constData(), params->password->size()));

        for (QFileInfo keyfile : params->keyfiles)
        {
            keyfiles->append(QSharedPointer<Volume::Keyfile>(new Volume::Keyfile(keyfile)));
        }

        // Open Volume
        volume->Open(params->volumePath, false, password, keyfiles, Volume::VolumeProtection::None,
                     QSharedPointer<Volume::VolumePassword>(), QSharedPointer<Volume::KeyfileList>(),
                     Volume::VolumeType::Unknown, params->useInternalBackup);


        if (params->useInternalBackup)
        {
            decryptedLayout = volume->GetLayout();
            if (!decryptedLayout->HasBackupHeader())
            {
                throw;    //TODO
            }
            backupHeader = volume->GetHeader();
        }
        else
        {
            QFile backupFile(params->backupHeaderFile.absoluteFilePath());
            if (!backupFile.open(QIODevice::ReadOnly))
            {
                throw FailedOpenFileException(QFileInfo(params->backupHeaderFile));
            }


            for (QSharedPointer<Volume::VolumeLayout> layout : Volume::VolumeLayout::GetAvailableLayouts())
            {
                SecureBuffer headerBuffer(layout->GetHeaderSize());

                if (!backupFile.seek(layout->GetType() == Volume::VolumeType::Hidden ? layout->GetHeaderSize() : 0))
                {
                    throw FailedLseekFileException(params->backupHeaderFile);
                }
                backupFile.read(reinterpret_cast<char*>(headerBuffer.get()), headerBuffer.size());

                QSharedPointer<Volume::VolumePassword> passwordKey = Volume::Keyfile::ApplyListToPassword(keyfiles,
                        password);
                if (layout->GetHeader()->Decrypt(headerBuffer, *passwordKey,
                                                 layout->GetSupportedKeyDerivationFunctions(), layout->GetSupportedEncryptionAlgorithms()))
                {
                    decryptedLayout = layout;
                    break;
                }
            }
            if (decryptedLayout.isNull())
            {
                throw PasswordOrKeyfilesIncorrectException(params->volumePath);
            }
            backupHeader = decryptedLayout->GetHeader();
        }

        SecureBuffer newHeaderBuffer(decryptedLayout->GetHeaderSize());
        ReEncryptVolumeHeaderWithNewSalt(newHeaderBuffer, backupHeader, password, keyfiles);
        int headerOffset = decryptedLayout->GetHeaderOffset();
        if (headerOffset >= 0)
        {
            volume->GetFile()->SeekAt(headerOffset);
        }
        else
        {
            volume->GetFile()->SeekEnd(headerOffset);
        }
        volume->GetFile()->Write(newHeaderBuffer);


    }
    catch (GostCryptException& e)
    {
        e.clone(params->id.requestId)->raise();
    }
    if (params->emitResponse)
    {
        emit sendRestoreHeader(response);
    }
    return response;
}

QSharedPointer<BenchmarkAlgorithmsResponse> CoreBase::benchmarkAlgorithms(
    QSharedPointer<BenchmarkAlgorithmsRequest> params)
{
    QSharedPointer<BenchmarkAlgorithmsResponse> response(new BenchmarkAlgorithmsResponse);

    try
    {
        GostCrypt::Volume::EncryptionAlgorithmList algorithms =
            GostCrypt::Volume::EncryptionAlgorithm::GetAvailableAlgorithms();

        if (params->bufferSize < ENCRYPTION_DATA_UNIT_SIZE)
        {
            throw InvalidParameterException("params->bufferSize",
                                            "The buffer size need to be at least " + QString::number(ENCRYPTION_DATA_UNIT_SIZE) + " bytes")
        }

            Buffer dataBuffer(params->bufferSize);

        Volume::EncryptionThreadPool::Start();

        for (QSharedPointer<GostCrypt::Volume::EncryptionAlgorithm> ea : algorithms)
        {
            if (ea->IsDeprecated())  // we don't allow deprecated algorithms
            {
                continue;
            }

            Buffer key(ea->GetKeySize());
            QElapsedTimer timer;
            quint64 processedDataSize;

            response->algorithmsNames.append(ea->GetName());
            ea->SetKey(key);
            ea->GetMode()->SetKey(key);

            // CPU "warm up" (an attempt to prevent skewed results on systems where CPU frequency gradually changes depending on CPU load).
            timer.start();
            do
            {
                // no need to cipher the whole buffer
                ea->EncryptSectors(dataBuffer, 0, 1, ENCRYPTION_DATA_UNIT_SIZE);
            }
            while (timer.elapsed() < 20);


            timer.restart();
            processedDataSize = 0;
            do
            {
                ea->EncryptSectors(dataBuffer, 0, dataBuffer.size() / ENCRYPTION_DATA_UNIT_SIZE,
                                   ENCRYPTION_DATA_UNIT_SIZE);
                processedDataSize += dataBuffer.size();
            }
            while (timer.elapsed() < 100);

            //computer spent time to cipher and store the derived encryption speed in response->encryptionSpeed (byte/s)
            response->encryptionSpeed.append((processedDataSize * 1000) / timer.elapsed());

            timer.restart();
            processedDataSize = 0;
            do
            {
                ea->DecryptSectors(dataBuffer, 0, dataBuffer.size() / ENCRYPTION_DATA_UNIT_SIZE,
                                   ENCRYPTION_DATA_UNIT_SIZE);
                processedDataSize += dataBuffer.size();
            }
            while (timer.elapsed() < 100);

            //computer spent time to cipher and store the derived encryption speed in response->encryptionSpeed (byte/s)
            response->decryptionSpeed.append((processedDataSize * 1000) / timer.elapsed());


            //compute average time (cipher + uncipher times / 2)
            response->meanSpeed.append((response->encryptionSpeed.last() + response->decryptionSpeed.last()) /
                                       2);
        }

        Volume::EncryptionThreadPool::Stop();
    }
    catch (GostCryptException& e)
    {
        e.clone(params->id.requestId)->raise();
    }

    if (params->emitResponse)
    {
        emit sendBenchmarkAlgorithms(response);
    }
    return response;


}

uid_t getUserId(QString username)
{
    struct passwd* passwdPtr;
    passwdPtr = getpwnam(username.toLocal8Bit().data());
    if (!passwdPtr)
    {
        throw InvalidParameterException("mountForUser", "The given user can not be found on the system.");
    }
    return passwdPtr->pw_uid;
}

gid_t getGroupId(QString groupname)
{
    struct group* groupPtr;
    groupPtr = getgrnam(groupname.toLocal8Bit().data());
    if (!groupPtr)
    {
        throw InvalidParameterException("mountForGroup", "The given group can not be found on the system.");
    }
    return groupPtr->gr_gid;
}
}
}
