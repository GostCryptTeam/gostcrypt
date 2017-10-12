#include <QDir>
#include <QThread>
#include <QTime>
#include "CoreRoot.h"
#include "LoopDeviceManager.h"
#include "MountFilesystemManager.h"
#include "NewFuseService/FuseServiceHandler.h"

namespace GostCrypt {
namespace NewCore {

CoreRoot::CoreRoot(QObject* parent): CoreBase(parent)
{
    realGroupId = static_cast <gid_t>(0);
    realUserId = static_cast <gid_t>(0);

    const char *envSudoGID = getenv ("SUDO_GID");
    if (envSudoGID) {
        realGroupId =  static_cast <gid_t> (QString(envSudoGID).toLong());
    }
    const char *envSudoUID = getenv ("SUDO_UID");
    if (envSudoUID) {
        realUserId =  static_cast <gid_t> (QString(envSudoUID).toLong());
    }
    connect(&fuseServiceHandler, SIGNAL(continueMountVolume(QSharedPointer<NewCore::MountVolumeRequest>,QSharedPointer<NewCore::MountVolumeResponse>)), this, SLOT(continueMountVolume(QSharedPointer<NewCore::MountVolumeRequest>,QSharedPointer<NewCore::MountVolumeResponse>)));
}

void CoreRoot::exit() {
#ifdef DEBUG_CORESERVICE_HANDLER
    qDebug() << "CoreRoot exiting";
#endif
    if(fuseServiceHandler.isRunning()) {
        connect(&fuseServiceHandler, SIGNAL(exited()), this, SIGNAL(exited()));
        fuseServiceHandler.exit();
    } else {
        // The main loop was not started, so an imediate call to app.quit() would not be working.
        QMetaObject::invokeMethod(this, "exited", Qt::QueuedConnection);
    }
}

void CoreRoot::request(QVariant r)
{
    HANDLE_REQUEST(MountVolume, mountVolume)
    else HANDLE_REQUEST(DismountVolume, dismountVolume)
        else HANDLE_REQUEST(CreateVolume, createVolume)
            else HANDLE_REQUEST(ChangeVolumePassword, changeVolumePassword)
                else if(!processNonRootRequest(r)) {
                    throw UnknowRequestException(r.typeName());
                }
}

//TODO
void CoreRoot::receiveSudoPassword(QString password)//QSharedPointer<QByteArray> password)
{
    password.fill('\0');
}

void CoreRoot::continueMountVolume(QSharedPointer<MountVolumeRequest> params, QSharedPointer<MountVolumeResponse> response)
{
    uid_t mountedForUserId;
    gid_t mountedForGroupId;

    //#define TRACKING
    #ifdef TRACKING
    START_TRACK
    #endif

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

    bool mountDirCreated = false;
    try {

        QSharedPointer<QFileInfo> virtualDevice;
        QSharedPointer<QFileInfo> imageFile(new QFileInfo(params->fuseMountPoint->absoluteFilePath() + NewFuseDriver::getVolumeImagePath()));
        virtualDevice = LoopDeviceManager::attachLoopDevice(imageFile, params->protection == VolumeProtection::ReadOnly);

        try {
            NewFuseDriver::sendAuxDeviceInfo(params->fuseMountPoint, virtualDevice);
        } catch(...) {
            LoopDeviceManager::detachLoopDevice(virtualDevice);
            throw;
        }
		UPDATE_PROGRESS(0.961340206);
		#ifdef TRACKING
		TRACK
		#endif

        if(params->doMount) {
            if(params->mountPoint.isNull() || params->mountPoint->absoluteFilePath().isEmpty()) {
                params->mountPoint = getFreeDefaultMountPoint(mountedForUserId);
            }

            QDir mountpoint(params->mountPoint->absoluteFilePath());
            if(!mountpoint.exists()) {
                if(!mountpoint.mkpath(params->mountPoint->absoluteFilePath()))
                    throw FailedCreateDirectoryException(params->mountPoint->absoluteFilePath());
                mountDirCreated = true;
            }
            MountFilesystemManager::mountFilesystem(virtualDevice, params->mountPoint, params->fileSystemType, params->protection == VolumeProtection::ReadOnly, mountedForUserId, mountedForGroupId, params->fileSystemOptions);
        }
		UPDATE_PROGRESS(0.974226804);
		#ifdef TRACKING
		TRACK
		#endif
    }
    catch (...)
    {
        QSharedPointer<DismountVolumeRequest> dismountParams(new DismountVolumeRequest);
        dismountParams->volumePath = params->path;
        dismountParams->emitResponse = false;
        // Try to dismount
        try {
            dismountVolume(dismountParams);
        } catch (...) {}
        if(mountDirCreated)
            QDir(params->mountPoint->absoluteFilePath()).rmdir(params->mountPoint->absoluteFilePath());
        throw;
    }

    QSharedPointer<GetMountedVolumesRequest> getMountedVolumesParams(new GetMountedVolumesRequest);
    QSharedPointer<GetMountedVolumesResponse> getMountedVolumesResponse(new GetMountedVolumesResponse);
    getMountedVolumesParams->volumePath = params->path;
    getMountedVolumesParams->emitResponse = false;
    getMountedVolumesResponse = getMountedVolumes(getMountedVolumesParams);
    QList<QSharedPointer<VolumeInformation>> volumeInfoList = getMountedVolumesResponse->volumeInfoList;
    if(volumeInfoList.size() < 1) {
        throw CoreException(); //TODO
    }
    response->volumeInfo = volumeInfoList.first();

    if(params->forVolumeCreation) {
        continueMountFormat(response);
        return;
    }

    emit sendMountVolume(response);
}

void CoreRoot::mountVolume(QSharedPointer<MountVolumeRequest> params)
{
    if(!params)
        throw MissingParamException("params");

    if(isVolumeMounted(params->path))
        throw VolumeAlreadyMountedException(params->path);

    params->fuseMountPoint = getFreeFuseMountPoint();
    params->isDevice = isDevice(params->path->canonicalFilePath());

    /* Create FUSE mounting */
#ifndef FUSE_SERVICE_DEBUG
    fuseServiceHandler.mount(params);
#else

    NewFuseDriver::NewFuseService tmpnfs;
    tmpnfs.mountRequestHandler(QVariant::fromValue(params));
#endif
}

QSharedPointer<DismountVolumeResponse> CoreRoot::dismountVolume(QSharedPointer<DismountVolumeRequest> params)
{
    QSharedPointer<DismountVolumeResponse> response(new DismountVolumeResponse);
    if(!params.isNull())
        response->passThrough = params->passThrough;
    /* Get mounted volume infos */
    QList<QSharedPointer<VolumeInformation>> mountedVolumes;
    {
        QSharedPointer<GetMountedVolumesRequest> getMountedVolumesParams(new GetMountedVolumesRequest);
        QSharedPointer<GetMountedVolumesResponse> getMountedVolumesResponse(new GetMountedVolumesResponse);
        if(params)
            getMountedVolumesParams->volumePath = params->volumePath;
        getMountedVolumesParams->emitResponse = false;
        getMountedVolumesResponse = getMountedVolumes(getMountedVolumesParams);
        if(params && params->volumePath && getMountedVolumesResponse->volumeInfoList.isEmpty())
            throw VolumeNotMountedException(params->volumePath);
        mountedVolumes = getMountedVolumesResponse->volumeInfoList;
    }
    for (QSharedPointer<VolumeInformation> mountedVolume : mountedVolumes) {
        /* Unmount filesystem */
        try {
            if(mountedVolume->mountPoint) {
                MountFilesystemManager::dismountFilesystem(mountedVolume->mountPoint, (params) ? params->force : false);
            }
        } catch(FailUnmountFilesystem &e) {
#ifdef QT_DEBUG
            qDebug().noquote() << e.qwhat();
#endif
        }

        /* Detach loop device */
        try {
            if(mountedVolume->virtualDevice) {
                LoopDeviceManager::detachLoopDevice(mountedVolume->virtualDevice);
            }
        } catch(FailedDetachLoopDevice &e) {
#ifdef QT_DEBUG
            qDebug().noquote() << e.qwhat();
#endif
        }

        // Probably not necessary to update mountedVolume

        /* Unmount Fuse filesystem */
        MountFilesystemManager::dismountFilesystem(mountedVolume->fuseMountPoint, (params) ? params->force : false);

        /* Delete fuse mount point directory */
        QDir(mountedVolume->fuseMountPoint->absoluteFilePath()).rmdir(mountedVolume->fuseMountPoint->absoluteFilePath());

        /* Saving the volume path to confirm to the QML that the volume was successfully dismounted */
        response.data()->volumePath.append(mountedVolume->volumePath);
    }

    if(params->forVolumeCreation) {
        continueDismountFormat(response);
        return response;
    }

    if(params->emitResponse)
        sendDismountVolume(response);

    return response;
}

void CoreRoot::writeHeaderToFile(fstream &file, QSharedPointer<CreateVolumeRequest::VolumeParams> params, QSharedPointer<VolumeLayout> layout, quint64 containersize)
{
    // getting the volume header to fill it
    QSharedPointer<VolumeHeader> header (layout->GetHeader());
    QSharedPointer<GostCrypt::EncryptionAlgorithm> ea (getEncryptionAlgorithm(params->encryptionAlgorithm));
    QSharedPointer<Pkcs5Kdf> Kdf (getDerivationKeyFunction(params->volumeHeaderKdf));

    VolumeHeaderCreationOptions options;
    options.EA = ea;
    options.Kdf = Kdf;
    options.Type = layout->GetType();
    options.SectorSize = 512; // TODO : ALWAYS 512 !

    if(options.Type == VolumeType::Hidden) {
        options.VolumeDataStart = containersize - layout->GetHeaderSize() * 2 - params->size;
    } else {
        options.VolumeDataStart = layout->GetHeaderSize() * 2;
    }

    if(params->size > 1.0 || params->size <= 0.0) // a percentage not in [0, 1]
        throw ContentSizeInvalidException(params->size);

    //TODO: Why multiply ? Why not just checking that asked size (params->size) is lower than the maximum data size given by layout->GetMaxDataSize
    options.VolumeDataSize = (quint64)params->size*layout->GetMaxDataSize(containersize); // unlike truecrypt, we let the user set its own size

    GostCrypt::SecureBuffer masterkey;  // decrypts the whole filesystem
    GostCrypt::SecureBuffer salt;       // salt to encrypt the header.
    GostCrypt::SecureBuffer headerkey;  // fusion of the password and the keyfiles, derived

    SecureBuffer headerBuffer (layout->GetHeaderSize()); // our header we'll write to the container

    // Master data key
    masterkey.Allocate (options.EA->GetKeySize() * 2);
    RandomNumberGenerator::GetData (masterkey);
    options.DataKey = masterkey;

    // PKCS5 salt
    salt.Allocate (VolumeHeader::GetSaltSize());
    RandomNumberGenerator::GetData (salt);
    options.Salt = salt;

    // Header key
    headerkey.Allocate (VolumeHeader::GetLargestSerializedKeySize());
    shared_ptr <KeyfileList> keyfiles;
    if(params->keyfiles)
        for(QSharedPointer<QFileInfo> keyfile : *params->keyfiles) {
            keyfiles->push_back(QSharedPointer<Keyfile>(new Keyfile(FilesystemPath(keyfile->absoluteFilePath().toStdWString()))));
        }
    shared_ptr<VolumePassword> password;
    if(!params->password.isNull())
        password.reset(new VolumePassword(params->password->constData(), params->password->size()));
    else
        throw MissingParamException("password");
    QSharedPointer <VolumePassword> passwordkey = Keyfile::ApplyListToPassword (keyfiles, password);
    options.Kdf->DeriveKey (headerkey, *passwordkey, salt);
    options.HeaderKey = headerkey;

    header->Create (headerBuffer, options); // header created !

    // Write The Header
    if (layout->GetHeaderOffset() >= 0) {
        file.seekp(layout->GetHeaderOffset(), std::ios_base::beg);
    } else {
        if((int64)layout->GetHeaderSize() + layout->GetHeaderOffset() < 0)
            throw InvalidHeaderOffsetException(layout->GetHeaderOffset(), layout->GetHeaderSize());
        file.seekp(containersize + layout->GetHeaderOffset(), std::ios_base::beg);
    }
    file.write((char*)headerBuffer.Ptr(), headerBuffer.Size()); // writing header

    if(!layout->HasBackupHeader())
        return;

    // Write The Backup Header if any

    RandomNumberGenerator::GetData (salt); // getting new salt
    options.Kdf->DeriveKey (headerkey, *passwordkey, salt);
    options.HeaderKey = headerkey;
    header->Create (headerBuffer, options); // creating new header

    if (layout->GetBackupHeaderOffset() >= 0)
        file.seekp(layout->GetBackupHeaderOffset(), std::ios_base::beg);
    else
        file.seekp(containersize + layout->GetBackupHeaderOffset(), std::ios_base::beg);
    file.write((char *)headerBuffer.Ptr(), headerBuffer.Size()); // writing backup header crypted with new salt

}

void CoreRoot::mountFormatVolume(QSharedPointer<QFileInfo> volume, QSharedPointer<QByteArray> password, QSharedPointer<QList<QSharedPointer<QFileInfo>>> keyfiles, QString filesystem, QSharedPointer<CreateVolumeRequest> parentParams, QSharedPointer<CreateVolumeResponse> parentResponse)
{
    QSharedPointer<MountVolumeResponse> mountresponse;
    QSharedPointer<MountVolumeRequest> mountparams(new MountVolumeRequest());
    mountparams->keyfiles = keyfiles;
    mountparams->doMount = false;
    mountparams->password = password;
    mountparams->path = volume;
    mountparams->forVolumeCreation = true;
    mountparams->passThrough.insert("volume", QVariant::fromValue(volume));
    mountparams->passThrough.insert("password", QVariant::fromValue(password));
    mountparams->passThrough.insert("keyiles", QVariant::fromValue(keyfiles));
    mountparams->passThrough.insert("filesystem", QVariant::fromValue(filesystem));
    mountparams->passThrough.insert("parentParams", QVariant::fromValue(parentParams));
    mountparams->passThrough.insert("parentResponse", QVariant::fromValue(parentResponse));
    mountVolume(mountparams);

}

void CoreRoot::continueMountFormat(QSharedPointer<MountVolumeResponse> mountResponse)
{
    QSharedPointer<QFileInfo> volume = mountResponse->passThrough.value("volume").value<QSharedPointer<QFileInfo>>();
    QSharedPointer<QByteArray> password = mountResponse->passThrough.value("password").value<QSharedPointer<QByteArray>>();
    QSharedPointer<QList<QSharedPointer<QFileInfo>>> keyfiles = mountResponse->passThrough.value("keyfiles").value<QSharedPointer<QList<QSharedPointer<QFileInfo>>>>();
    QString filesystem = mountResponse->passThrough.value("filesystem").value<QString>();
    QString formatter = "mkfs."+filesystem;


    QSharedPointer<DismountVolumeRequest> dismountparams(new DismountVolumeRequest());
    dismountparams->volumePath = volume;
    dismountparams->emitResponse = false;
    dismountparams->forVolumeCreation = true;
    dismountparams->passThrough.insert("parentParams", mountResponse->passThrough.value("parentParams"));
    dismountparams->passThrough.insert("parentResponse", mountResponse->passThrough.value("parentResponse"));

    QStringList arguments;
    arguments << mountResponse->volumeInfo->virtualDevice->absoluteFilePath();

    QProcess *formatProcess = new QProcess();
    formatProcess->start(formatter, arguments);

    if (!formatProcess->waitForFinished() || formatProcess->exitStatus() != QProcess::NormalExit) {
        try {
            dismountparams->forVolumeCreation = false;
            dismountVolume(dismountparams);
        } catch (CoreException &e) {
            throw FormattingSubExceptionException(e.qwhat());
        }
        throw ProcessFailedException();
    }

    if (formatProcess->exitCode() == 127) { // command not found
        try {
            dismountparams->forVolumeCreation = false;
            dismountVolume(dismountparams);
        } catch (CoreException &e) {
            throw FormattingSubExceptionException(e.qwhat());
        }
        throw FilesystemNotSupportedException(filesystem);
    }

    // Gve time for formatting to apply change on file
    QThread::msleep(100);
    try {
        dismountVolume(dismountparams); // finally dismounting the volume
    } catch (CoreException &e) {
        throw FormattingSubExceptionException(e.qwhat());
    }
}

void CoreRoot::continueDismountFormat(QSharedPointer<DismountVolumeResponse> dismountResponse)
{
    emit formatVolumeDone(dismountResponse->passThrough.value("parentParams").value<QSharedPointer<CreateVolumeRequest>>(), dismountResponse->passThrough.value("parentResponse").value<QSharedPointer<CreateVolumeResponse>>());
}

void CoreRoot::createVolume(QSharedPointer<CreateVolumeRequest> params)
{
    QSharedPointer<CreateVolumeResponse> response(new CreateVolumeResponse);
    if(!params.isNull())
        response->passThrough = params->passThrough;
    fstream volumefile;

    /*  Steps:
            write headers directly in file.
            mount with no-filesystem
            format the loop device
            unmount
    */

    if(!params)
        throw MissingParamException("params");
    if(!params->path)
        throw MissingParamException("params->path");
    if(!params->outerVolume)
        throw MissingParamException("params->outervolume");
    if(isVolumeMounted(params->path))
        throw VolumeAlreadyMountedException(params->path);

    // this is the CoreRoot class. we are assuming that it is launched as root.

    /*
     * WRITING RANDOM DATA ACROSS THE WHOLE VOLUME
     */

    createRandomFile(params->path, params->size, params->outerVolume->encryptionAlgorithm, false); // no random to create the file faster.

    // opening file (or device)
    volumefile.open(params->path->absoluteFilePath().toStdString(), ios::in | ios::out | ios::binary);
    if(!volumefile.is_open())
        throw FailedOpenVolumeException(params->path);

    /*
     * WRITING HEADERS
     */

    // getting the outer volume layout to write the header
    QSharedPointer<VolumeLayout> outerlayout;
    outerlayout.reset(new VolumeLayoutV2Normal()); // we only use the V2

    writeHeaderToFile(volumefile, params->outerVolume, outerlayout, params->size);

    QSharedPointer<VolumeLayout> innerlayout;
    innerlayout.reset(new VolumeLayoutV2Hidden()); // we ALWAYS have a hidden volume header, it can just be a fake one

    if(params->type == VolumeType::Hidden) { // writing the inner volume headers if any
        writeHeaderToFile(volumefile, params->innerVolume, innerlayout, params->size);
    } else { // writing random data to the hidden headers location
        QSharedPointer<CreateVolumeRequest::VolumeParams> randomparams(new CreateVolumeRequest::VolumeParams());
        randomparams->size = 0.5;
        randomparams->encryptionAlgorithm = params->outerVolume->encryptionAlgorithm;
        randomparams->filesystem = params->outerVolume->filesystem;
        randomparams->volumeHeaderKdf = params->outerVolume->volumeHeaderKdf;
        // creating a completely random password for a non-existent hidden volume
        SecureBuffer pass;
        pass.Allocate(VolumePassword::MaxSize);
        RandomNumberGenerator::GetData(pass);
        randomparams->password.reset(new QByteArray((char *)pass.Ptr(), pass.Size()));
        writeHeaderToFile(volumefile, randomparams, innerlayout, params->size);
    }

    /*
     * FORMATTING THE VOLUME
     */
    if(params->type == VolumeType::Hidden)
        connect(this, SIGNAL(formatVolumeDone(QSharedPointer<CreateVolumeRequest>,QSharedPointer<CreateVolumeResponse>)), this, SLOT(continueFormatHidden(QSharedPointer<CreateVolumeRequest>,QSharedPointer<CreateVolumeResponse>)));
    else
        connect(this, SIGNAL(formatVolumeDone(QSharedPointer<CreateVolumeRequest>,QSharedPointer<CreateVolumeResponse>)), this, SLOT(finishCreateVolume(QSharedPointer<CreateVolumeRequest>,QSharedPointer<CreateVolumeResponse>)));
    mountFormatVolume(params->path, params->outerVolume->password, params->outerVolume->keyfiles, params->outerVolume->filesystem, params, response);
}

void CoreRoot::continueFormatHidden(QSharedPointer<CreateVolumeRequest> params, QSharedPointer<CreateVolumeResponse> response)
{
    disconnect(this, SIGNAL(formatVolumeDone(QVariantMap)), 0, 0);
    connect(this, SIGNAL(formatVolumeDone(QSharedPointer<CreateVolumeRequest>)), this, SLOT(finishCreateVolume(QSharedPointer<CreateVolumeRequest>)));
    mountFormatVolume(params->path, params->innerVolume->password, params->innerVolume->keyfiles, params->innerVolume->filesystem, params, response);
}

void CoreRoot::finishCreateVolume(QSharedPointer<CreateVolumeRequest> params, QSharedPointer<CreateVolumeResponse> response)
{
    if(params->emitResponse)
        emit sendCreateVolume(response);
}

QSharedPointer<ChangeVolumePasswordResponse> CoreRoot::changeVolumePassword(QSharedPointer<ChangeVolumePasswordRequest> params)
{
    QSharedPointer<ChangeVolumePasswordResponse> response(new ChangeVolumePasswordResponse());
    if(!params.isNull())
        response->passThrough = params->passThrough;
    (void)params;

    //TODO

    if(params->emitResponse)
        emit sendChangeVolumePassword(response);

    return response;
}

}
}
