#include <QDir>
#include "CoreRoot.h"
#include "FuseDriver/FuseService.h"
#include "LoopDeviceManager.h"
#include "MountFilesystemManager.h"

namespace GostCrypt
{
namespace NewCore
{

CoreRoot::CoreRoot(QObject* parent): CoreBase(parent)
{
    realGroupId = static_cast <gid_t>(0);
    realUserId = static_cast <gid_t>(0);

    const char* envSudoGID = getenv("SUDO_GID");
    if (envSudoGID)
    {
        realGroupId =  static_cast <gid_t>(QString(envSudoGID).toLong());
    }
    const char* envSudoUID = getenv("SUDO_UID");
    if (envSudoUID)
    {
        realUserId =  static_cast <gid_t>(QString(envSudoUID).toLong());
    }
}

void CoreRoot::exit()
{
#ifdef DEBUG_CORESERVICE_HANDLER
    qDebug() << "CoreRoot exiting";
#endif
    // The main loop was not started, so an imediate call to app.quit() would not be working.
    QMetaObject::invokeMethod(this, "exited", Qt::QueuedConnection);
}

void CoreRoot::request(QVariant r)
{
    HANDLE_REQUEST(MountVolume, mountVolume)
    else HANDLE_REQUEST(DismountVolume, dismountVolume)
        else HANDLE_REQUEST(CreateVolume, createVolume)
            else HANDLE_REQUEST(ChangeVolumePassword, changeVolumePassword)
                else if (!processNonRootRequest(r))
                {
                    throw UnknowRequestException(r.typeName());
                }
}

//TODO
void CoreRoot::receiveSudoPassword(QString password)//QSharedPointer<QByteArray> password)
{
    password.fill('\0');
}

QSharedPointer<MountVolumeResponse> CoreRoot::mountVolume(QSharedPointer<MountVolumeRequest> params,
        bool emitResponse)
{
    QSharedPointer<MountVolumeResponse> response(new MountVolumeResponse);
    uid_t mountedForUserId;
    gid_t mountedForGroupId;

    UPDATE_PROGRESS(0);
    if (!params)
    {
        throw MissingParamException("params");
    }

    if (isVolumeMounted(params->path))
    {
        throw VolumeAlreadyMountedException(params->path);
    }

    if (params->mountedForUser.isEmpty())
    {
        mountedForUserId = realUserId;
    }
    else
    {
        mountedForUserId = getUserId(params->mountedForUser);
    }

    if (params->mountedForGroup.isEmpty())
    {
        mountedForGroupId = realGroupId;
    }
    else
    {
        mountedForGroupId = getGroupId(params->mountedForGroup);
    }

    QSharedPointer<Volume> volume(new Volume);
    QSharedPointer<QFileInfo> fuseMountPoint;

    do
    {
        try
        {
            VolumePath path(params->path->absoluteFilePath().toStdWString());
            shared_ptr<VolumePassword> password;
            shared_ptr<VolumePassword> protectionPassword;
            shared_ptr <KeyfileList> keyfiles;
            shared_ptr <KeyfileList> protectionKeyfiles;


            /* Conversions :( */
            if (!params->password.isNull())
            {
                password.reset(new VolumePassword(params->password->constData(), params->password->size()));
            }
            else
            {
                throw MissingParamException("password");
            }
            if (!params->protectionPassword.isNull())
            {
                protectionPassword.reset(new VolumePassword(params->protectionPassword->constData(),
                                         params->protectionPassword->size()));
            }
            if (!params->keyfiles.isNull())
            {
                for (QSharedPointer<QFileInfo> keyfile : *params->keyfiles)
                {
                    keyfiles->push_back(QSharedPointer<Keyfile>(new Keyfile(FilesystemPath(
                                            keyfile->absoluteFilePath().toStdWString()))));
                }
            }
            if (!params->protectionKeyfiles.isNull())
            {
                for (QSharedPointer<QFileInfo> keyfile : *params->protectionKeyfiles)
                {
                    protectionKeyfiles->push_back(QSharedPointer<Keyfile>(new Keyfile(FilesystemPath(
                                                      keyfile->absoluteFilePath().toStdWString()))));
                }
            }

            volume->Open(
                path,
                params->preserveTimestamps,
                password,
                keyfiles,
                params->protection,
                protectionPassword,
                protectionKeyfiles,
                params->useBackupHeaders
            );
        }
        catch (GostCrypt::PasswordException& e)
        {
            throw IncorrectVolumePasswordException(params->path)
        }
        catch (GostCrypt::SystemException& e)
        {
            // In case of permission issue try again in read-only
            if (params->protection != VolumeProtection::ReadOnly && (e.GetErrorCode() == EROFS ||
                    e.GetErrorCode() == EACCES || e.GetErrorCode() == EPERM))
            {
                params->protection = VolumeProtection::ReadOnly;
                response->writeDisabled = true;
                continue;
            }
            throw FailedOpenVolumeException(params->path);
        }
        catch (GostCrypt::Exception& e)
        {
            throw ExceptionFromVolumeException(e.what());
        }

        params->password->fill('\0');
        if (!params->protectionPassword.isNull())
        {
            params->protectionPassword->fill('\0');
        }
        break;
    }
    while (0);

    try
    {
        if (isDevice(params->path->canonicalFilePath()))
        {
            if (volume->GetFile()->GetDeviceSectorSize() != volume->GetSectorSize())
            {
                throw IncorrectSectorSizeException();
            }
            /* GostCrypt suport only 512 sector size, other sector sizes can be use only with kernel crypto */
            if (volume->GetSectorSize() != 512)
            {
                throw IncorrectSectorSizeException();
            }
        }

        fuseMountPoint = getFreeFuseMountPoint();
        QDir fuseMountPointDir(fuseMountPoint->absoluteFilePath());
        if (!fuseMountPointDir.exists() && !fuseMountPointDir.mkdir(fuseMountPoint->absoluteFilePath()))
        {
            throw FailedCreateFuseMountPointException(fuseMountPoint);
        }

        try
        {
            // TODO recode fuse
            shared_ptr<Volume> vol(new Volume(*volume));
            FuseService::Mount(vol, (VolumeSlotNumber)1, fuseMountPoint->absoluteFilePath().toStdString());
        }
        catch (...)
        {
            QDir(fuseMountPoint->absoluteFilePath()).rmdir(QStringLiteral("."));
            throw;
        }
    }
    catch (...)
    {
        volume->Close();
        throw;
    }

    bool mountDirCreated = false;
    try
    {

        QSharedPointer<QFileInfo> virtualDevice;
        QSharedPointer<QFileInfo> imageFile(new QFileInfo(fuseMountPoint->absoluteFilePath() +
                                            FuseService::GetVolumeImagePath()));
        virtualDevice = LoopDeviceManager::attachLoopDevice(imageFile,
                        params->protection == VolumeProtection::ReadOnly);

        try
        {
            DirectoryPath fmp(fuseMountPoint->absoluteFilePath().toStdWString());
            DirectoryPath vd(virtualDevice->absoluteFilePath().toStdWString());
            FuseService::SendAuxDeviceInfo(fmp, vd, vd);
        }
        catch (...)
        {
            LoopDeviceManager::detachLoopDevice(virtualDevice);
            throw;
        }

        if (params->doMount)
        {
            if (params->mountPoint.isNull() || params->mountPoint->absoluteFilePath().isEmpty())
            {
                params->mountPoint = getFreeDefaultMountPoint(mountedForUserId);
            }

            QDir mountpoint(params->mountPoint->absoluteFilePath());
            if (!mountpoint.exists())
            {
                if (!mountpoint.mkpath(params->mountPoint->absoluteFilePath()))
                {
                    throw FailedCreateDirectoryException(params->mountPoint->absoluteFilePath());
                }
                mountDirCreated = true;
            }
            MountFilesystemManager::mountFilesystem(virtualDevice, params->mountPoint, params->fileSystemType,
                                                    params->protection == VolumeProtection::ReadOnly, mountedForUserId, mountedForGroupId,
                                                    params->fileSystemOptions);
        }
    }
    catch (...)
    {
        QSharedPointer<DismountVolumeRequest> dismountParams(new DismountVolumeRequest);
        dismountParams->volumePath = params->path;
        dismountVolume(dismountParams);
        if (mountDirCreated)
        {
            QDir(params->mountPoint->absoluteFilePath()).rmdir(params->mountPoint->absoluteFilePath());
        }
        throw;
    }

    QSharedPointer<GetMountedVolumesRequest> getMountedVolumesParams(new GetMountedVolumesRequest);
    QSharedPointer<GetMountedVolumesResponse> getMountedVolumesResponse(new GetMountedVolumesResponse);
    getMountedVolumesParams->volumePath = params->path;

    getMountedVolumesResponse = getMountedVolumes(getMountedVolumesParams, false);
    response->volumeInfo = getMountedVolumesResponse->volumeInfoList.first();

    if (emitResponse)
    {
        emit sendMountVolume(response);
    }

    return response;
}

QSharedPointer<DismountVolumeResponse> CoreRoot::dismountVolume(
    QSharedPointer<DismountVolumeRequest> params, bool emitResponse)
{
    QSharedPointer<DismountVolumeResponse> response(new DismountVolumeResponse);

    /* Get mounted volume infos */
    QList<QSharedPointer<VolumeInformation>> mountedVolumes;
    {
        QSharedPointer<GetMountedVolumesRequest> getMountedVolumesParams(new GetMountedVolumesRequest);
        QSharedPointer<GetMountedVolumesResponse> getMountedVolumesResponse(new GetMountedVolumesResponse);
        if (params)
        {
            getMountedVolumesParams->volumePath = params->volumePath;
        }
        getMountedVolumesResponse = getMountedVolumes(getMountedVolumesParams, false);
        if (params && params->volumePath && getMountedVolumesResponse->volumeInfoList.isEmpty())
        {
            throw VolumeNotMountedException(params->volumePath);
        }
        mountedVolumes = getMountedVolumesResponse->volumeInfoList;
    }
    for (QSharedPointer<VolumeInformation> mountedVolume : mountedVolumes)
    {
        /* Unmount filesystem */
        try
        {
            if (mountedVolume->mountPoint)
            {
                MountFilesystemManager::dismountFilesystem(mountedVolume->mountPoint,
                        (params) ? params->force : false);
            }
        }
        catch (FailUnmountFilesystem& e)
        {
#ifdef QT_DEBUG
            qDebug().noquote() << e.qwhat();
#endif
        }

        /* Detach loop device */
        try
        {
            if (mountedVolume->virtualDevice)
            {
                LoopDeviceManager::detachLoopDevice(mountedVolume->virtualDevice);
            }
        }
        catch (FailedDetachLoopDevice& e)
        {
#ifdef QT_DEBUG
            qDebug().noquote() << e.qwhat();
#endif
        }

        // Probably not necessary to update mountedVolume

        /* Unmount Fuse filesystem */
        MountFilesystemManager::dismountFilesystem(mountedVolume->fuseMountPoint,
                (params) ? params->force : false);

        /* Delete fuse mount point directory */
        QDir(mountedVolume->fuseMountPoint->absoluteFilePath()).rmdir(
            mountedVolume->fuseMountPoint->absoluteFilePath());

        /* Saving the volume path to confirm to the QML that the volume was successfully dismounted */
        response.data()->volumePath.append(mountedVolume->volumePath);
    }

    if (emitResponse)
    {
        sendDismountVolume(response);
    }

    return response;
}

void CoreRoot::writeHeaderToFile(fstream& file,
                                 QSharedPointer<CreateVolumeRequest::VolumeParams> params, QSharedPointer<VolumeLayout> layout,
                                 quint64 containersize)
{
    // getting the volume header to fill it
    QSharedPointer<VolumeHeader> header(layout->GetHeader());
    QSharedPointer<GostCrypt::EncryptionAlgorithm> ea(getEncryptionAlgorithm(
                params->encryptionAlgorithm));
    QSharedPointer<Pkcs5Kdf> Kdf(getDerivationKeyFunction(params->volumeHeaderKdf));

    VolumeHeaderCreationOptions options;
    options.EA = ea;
    options.Kdf = Kdf;
    options.Type = layout->GetType();
    options.SectorSize = 512; // TODO : ALWAYS 512 !

    if (options.Type == VolumeType::Hidden)
    {
        options.VolumeDataStart = containersize - layout->GetHeaderSize() * 2 - params->size;
    }
    else
    {
        options.VolumeDataStart = layout->GetHeaderSize() * 2;
    }

    if (params->size > 1.0 || params->size <= 0.0) // a percentage not in [0, 1]
    {
        throw ContentSizeInvalidException(params->size);
    }

    options.VolumeDataSize = (quint64)params->size * layout->GetMaxDataSize(
                                 containersize); // unlike truecrypt, we let the user set its own size

    GostCrypt::SecureBuffer masterkey;  // decrypts the whole filesystem
    GostCrypt::SecureBuffer salt;       // salt to encrypt the header.
    GostCrypt::SecureBuffer headerkey;  // fusion of the password and the keyfiles, derived

    SecureBuffer headerBuffer(layout->GetHeaderSize());  // our header we'll write to the container

    // Master data key
    masterkey.Allocate(options.EA->GetKeySize() * 2);
    RandomNumberGenerator::GetData(masterkey);
    options.DataKey = masterkey;

    // PKCS5 salt
    salt.Allocate(VolumeHeader::GetSaltSize());
    RandomNumberGenerator::GetData(salt);
    options.Salt = salt;

    // Header key
    headerkey.Allocate(VolumeHeader::GetLargestSerializedKeySize());
    shared_ptr <KeyfileList> keyfiles;
    if (params->keyfiles)
        for (QSharedPointer<QFileInfo> keyfile : *params->keyfiles)
        {
            keyfiles->push_back(QSharedPointer<Keyfile>(new Keyfile(FilesystemPath(
                                    keyfile->absoluteFilePath().toStdWString()))));
        }
    shared_ptr<VolumePassword> password;
    if (!params->password.isNull())
    {
        password.reset(new VolumePassword(params->password->constData(), params->password->size()));
    }
    else
    {
        throw MissingParamException("password");
    }
    QSharedPointer <VolumePassword> passwordkey = Keyfile::ApplyListToPassword(keyfiles, password);
    options.Kdf->DeriveKey(headerkey, *passwordkey, salt);
    options.HeaderKey = headerkey;

    header->Create(headerBuffer, options);  // header created !

    // Write The Header
    if (layout->GetHeaderOffset() >= 0)
    {
        file.seekp(layout->GetHeaderOffset(), std::ios_base::beg);
    }
    else
    {
        if ((int64)layout->GetHeaderSize() + layout->GetHeaderOffset() < 0)
        {
            throw InvalidHeaderOffsetException(layout->GetHeaderOffset(), layout->GetHeaderSize());
        }
        file.seekp(containersize + layout->GetHeaderOffset(), std::ios_base::beg);
    }
    file.write((char*)headerBuffer.Ptr(), headerBuffer.Size()); // writing header

    if (!layout->HasBackupHeader())
    {
        return;
    }

    // Write The Backup Header if any

    RandomNumberGenerator::GetData(salt);  // getting new salt
    options.Kdf->DeriveKey(headerkey, *passwordkey, salt);
    options.HeaderKey = headerkey;
    header->Create(headerBuffer, options);  // creating new header

    if (layout->GetBackupHeaderOffset() >= 0)
    {
        file.seekp(layout->GetBackupHeaderOffset(), std::ios_base::beg);
    }
    else
    {
        file.seekp(containersize + layout->GetBackupHeaderOffset(), std::ios_base::beg);
    }
    file.write((char*)headerBuffer.Ptr(),
               headerBuffer.Size());  // writing backup header crypted with new salt

}

void CoreRoot::formatVolume(QSharedPointer<QFileInfo> volume, QSharedPointer<QByteArray> password,
                            QSharedPointer<QList<QSharedPointer<QFileInfo>>> keyfiles, QString filesystem)
{
    QString formatter = "mkfs." + filesystem;

    QSharedPointer<MountVolumeResponse> mountresponse;
    QSharedPointer<MountVolumeRequest> mountparams(new MountVolumeRequest());
    mountparams->keyfiles = keyfiles;
    mountparams->doMount = false;
    mountparams->password = password;
    mountparams->path = volume;

    QSharedPointer<DismountVolumeRequest> dismountparams(new DismountVolumeRequest());
    dismountparams->volumePath = volume;

    //try {
    mountresponse = mountVolume(mountparams, false);
    //} catch (CoreException &e){
    //    throw FormattingSubExceptionException(e);
    //}

    QStringList arguments;
    arguments << mountresponse->volumeInfo->virtualDevice->absoluteFilePath();

    QProcess* formatProcess = new QProcess();
    formatProcess->start(formatter, arguments);

    if (!formatProcess->waitForFinished() || formatProcess->exitStatus() != QProcess::NormalExit)
    {
        try
        {
            dismountVolume(dismountparams);
        }
        catch (CoreException& e)
        {
            throw FormattingSubExceptionException(e);
        }
        throw ProcessFailedException();
    }

    if (formatProcess->exitCode() == 127)  // command not found
    {
        try
        {
            dismountVolume(dismountparams);
        }
        catch (CoreException& e)
        {
            throw FormattingSubExceptionException(e);
        }
        throw FilesystemNotSupportedException(filesystem);
    }

    try
    {
        dismountVolume(dismountparams, false); // finally dismounting the volume
    }
    catch (CoreException& e)
    {
        throw FormattingSubExceptionException(e);
    }
}

QSharedPointer<CreateVolumeResponse> CoreRoot::createVolume(QSharedPointer<CreateVolumeRequest>
        params, bool emitResponse)
{
    QSharedPointer<CreateVolumeResponse> response(new CreateVolumeResponse);

    fstream volumefile;

    /*  Steps:
            write headers directly in file.
            mount with no-filesystem
            format the loop device
            unmount
    */

    if (!params)
    {
        throw MissingParamException("params");
    }
    if (!params->path)
    {
        throw MissingParamException("params->path");
    }
    if (!params->outerVolume)
    {
        throw MissingParamException("params->outervolume");
    }
    if (isVolumeMounted(params->path))
    {
        throw VolumeAlreadyMountedException(params->path);
    }

    // this is the CoreRoot class. we are assuming that it is launched as root.

    /*
     * WRITING RANDOM DATA ACROSS THE WHOLE VOLUME
     */

    createRandomFile(params->path, params->size, params->outerVolume->encryptionAlgorithm,
                     false); // no random to create the file faster.

    // opening file (or device)
    volumefile.open(params->path->absoluteFilePath().toStdString(), ios::in | ios::out | ios::binary);
    if (!volumefile.is_open())
    {
        throw FailedOpenVolumeException(params->path);
    }

    /*
     * WRITING HEADERS
     */

    // getting the outer volume layout to write the header
    QSharedPointer<VolumeLayout> outerlayout;
    outerlayout.reset(new VolumeLayoutV2Normal()); // we only use the V2

    writeHeaderToFile(volumefile, params->outerVolume, outerlayout, params->size);

    QSharedPointer<VolumeLayout> innerlayout;
    innerlayout.reset(new
                      VolumeLayoutV2Hidden()); // we ALWAYS have a hidden volume header, it can just be a fake one

    if (params->type == VolumeType::Hidden) // writing the inner volume headers if any
    {
        writeHeaderToFile(volumefile, params->innerVolume, innerlayout, params->size);
    }
    else     // writing random data to the hidden headers location
    {
        QSharedPointer<CreateVolumeRequest::VolumeParams> randomparams(new
                CreateVolumeRequest::VolumeParams());
        randomparams->size = 0.5;
        randomparams->encryptionAlgorithm = params->outerVolume->encryptionAlgorithm;
        randomparams->filesystem = params->outerVolume->filesystem;
        randomparams->volumeHeaderKdf = params->outerVolume->volumeHeaderKdf;
        // creating a completely random password for a non-existent hidden volume
        SecureBuffer pass;
        pass.Allocate(VolumePassword::MaxSize);
        RandomNumberGenerator::GetData(pass);
        randomparams->password.reset(new QByteArray((char*)pass.Ptr(), pass.Size()));
        writeHeaderToFile(volumefile, randomparams, innerlayout, params->size);
    }

    /*
     * FORMATTING THE VOLUME
     */

    formatVolume(params->path, params->outerVolume->password, params->outerVolume->keyfiles,
                 params->outerVolume->filesystem);

    if (params->type == VolumeType::Hidden)
    {
        formatVolume(params->path, params->innerVolume->password, params->innerVolume->keyfiles,
                     params->innerVolume->filesystem);
    }

    if (emitResponse)
    {
        sendCreateVolume(response);
    }

    return response;
}

QSharedPointer<ChangeVolumePasswordResponse> CoreRoot::changeVolumePassword(
    QSharedPointer<ChangeVolumePasswordRequest> params, bool emitResponse)
{
    QSharedPointer<ChangeVolumePasswordResponse> response(new ChangeVolumePasswordResponse());

    (void)params;

    //TODO

    if (emitResponse)
    {
        emit sendChangeVolumePassword(response);
    }

    return response;
}

}
}
