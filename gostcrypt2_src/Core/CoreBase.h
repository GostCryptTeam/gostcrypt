#ifndef COREBASE_H
#define COREBASE_H

#include <QObject>
#include <QCoreApplication>
#include "CoreRequest.h"
#include "CoreResponse.h"
#include "CoreException.h"
#include "RandomGenerator.h"
#include "Volume/EncryptionModeXTS.h"
#include "Volume/EncryptionAlgorithm.h"
#include "Service.h"

//used to generate random files (maybe to remove or at least move)
#define FILE_OPTIMAL_WRITE_SIZE 256*1024

#define GOSTCRYPT_FUSE_MOUNT_DIR_PREFIX ".gostcrypt_aux_mnt"
#define HANDLE_REQUEST(requestName, fct) \
    if(r.canConvert<QSharedPointer<requestName ## Request>>()) { \
        QSharedPointer<requestName ## Request> request; \
        request = r.value<QSharedPointer<requestName ## Request>>(); \
        fct (request); \
    }
#define DEC_REQUEST_SIGNAL(requestName) void send ## requestName (QSharedPointer<requestName ## Response> r)

/* Progress tracking analysis helpers */
#define TRACK track_result.append(QPair<QString,int>(QString(__FILE__) + QString(":") + QString::number(__LINE__), t.elapsed()));
#define START_TRACK \
    QTime t; \
    t.start(); \
    QList<QPair<QString,int>> track_result; \
    TRACK
#define FINISH_TRACK \
    TRACK \
    for(QPair<QString,int> p : track_result) { \
        qDebug().noquote() << p.first << " " << (float)p.second/track_result.last().second; \
    }
/* END */

namespace GostCrypt
{
namespace Core
{
/**
 * @brief Abstract class defining the core object containing methods for each action handled by GostCrypt (mount, dismount, create a volume, etc)
 *
 */
class CoreBase : public QObject
{
    Q_OBJECT
 public:
    /**
     * @brief Default Constructor
     *
     * @param parent parent object
     */
    explicit CoreBase(QObject* parent = nullptr);

    ~CoreBase();

 public slots:  // NOLINT
    /**
     * @brief Execute the requested action (mount, dismount, create a volume, etc)
     *
     * @param request QVariant containing the request object which is a child of CoreRequest
     * @sa CoreRequest
     */
    virtual void request(QVariant request) = 0;
    /**
     * @brief Close child processes used by this object
     *
     */
    virtual void exit() = 0;
    /**
     * @brief Receive password of the current user in order to get root priviliges using sudo utility. The current user must be authorized to use sudo to launch the GostCrypt executable as root user.
     *
     * @param password Password of the current user
     */
    virtual void receiveSudoPassword(QString password) = 0;
 protected:
    /**
     * @brief Give the supported encryption algorithms names. These names should be used to indicate which algorithm to use for other actions
     *
     * @param params Parameters of the action
     * @return QSharedPointer<GetEncryptionAlgorithmsResponse>, the response of the action
     */
    QSharedPointer<GetEncryptionAlgorithmsResponse> getEncryptionAlgorithms(
        QSharedPointer<GetEncryptionAlgorithmsRequest> params =
            QSharedPointer<GetEncryptionAlgorithmsRequest>());
    /**
     * @brief Give the supported key derivation functions names. These names should be used to indicate which algorithm to use for other actions
     *
     * @param params Parameters of the action
     * @return QSharedPointer<GetDerivationFunctionsResponse>, the response of the action
     */
    QSharedPointer<GetDerivationFunctionsResponse> getDerivationFunctions(
        QSharedPointer<GetDerivationFunctionsRequest> params =
            QSharedPointer<GetDerivationFunctionsRequest>());
    /**
     * @brief Give the availables storage devices on the computer (Disks, USB keys, etc)
     *
     * @param params Parameters of the action
     * @return QSharedPointer<GetHostDevicesResponse>, the response of the action
     */
    QSharedPointer<GetHostDevicesResponse> getHostDevices(QSharedPointer<GetHostDevicesRequest> params =
                QSharedPointer<GetHostDevicesRequest>());
    /**
     * @brief Give the list of currently mounted volumes
     *
     * @param params Parameters of the action
     * @return QSharedPointer<GetMountedVolumesResponse>, the response of the action
     */
    QSharedPointer<GetMountedVolumesResponse> getMountedVolumes(QSharedPointer<GetMountedVolumesRequest>
            params = QSharedPointer<GetMountedVolumesRequest>());
    /**
     * @brief Change the password of a GostCrypt volume
     *
     * @param params Parameters of the action
     * @return QSharedPointer<ChangeVolumePasswordResponse>, the response of the action
     */
    QSharedPointer<ChangeVolumePasswordResponse> changeVolumePassword(
        QSharedPointer<ChangeVolumePasswordRequest> params);

    /**
     * @brief Create a random key file
     * This action will simply create a file of 64 bytes with random bytes
     *
     * @param params Parameters of the action
     * @return QSharedPointer<CreateKeyFileResponse>, the response of the action
     */
    QSharedPointer<CreateKeyFileResponse> createKeyFile(QSharedPointer<CreateKeyFileRequest> params =
                QSharedPointer<CreateKeyFileRequest>());

    /**
     * @brief Backup the header of the given volume
     * This action will save the headers of the volume instances contained inside the given volume container
     *
     * @param params Parameters of the action
     * @return QSharedPointer<BackupHeaderResponse>, the response of the action
     */
    QSharedPointer<BackupHeaderResponse> backupHeader(QSharedPointer<BackupHeaderRequest> params);

    /**
     * @brief Restore the header of the given volume
     * This action will restore the header of the volume instance corresponding to given credentials.
     * It will either use the internal backup or an external header backup file.
     * For this action to succeed the password and keyfiles of the backup header should correspond to the password and keyfiles of the volume instance for which to restore the header
     *
     * @param params Parameters of the action
     * @return QSharedPointer<BackupHeaderResponse>, the response of the action
     */
    QSharedPointer<RestoreHeaderResponse> restoreHeader(QSharedPointer<RestoreHeaderRequest> params);

    /**
     * @brief Launch a benchmark of EncryptionAlgorthims supported by GostCrypt and return encryption speed
     *
     * @param params Parameters of the action
     * @return QSharedPointer<BackupHeaderResponse>, the response of the action
     */
    QSharedPointer<BenchmarkAlgorithmsResponse> benchmarkAlgorithms(
        QSharedPointer<BenchmarkAlgorithmsRequest> params);

    /**
     * @brief Give the list of mounted filesystems. It can filter only filesystems referring to the given device, or mounted in a specific mountpoint.
     *
     * @param devicePath Path of the device we want to get the list of corresponding mounted filesystems.
     * @param mountPoint Path of the mountpoint we want to get the list of corresponding mounted filesystems.
     * @return QList<QSharedPointer<MountedFilesystem> > the list of mounted filesystems
     */
    QList<QSharedPointer<MountedFilesystem>> getMountedFilesystems(const QFileInfo& devicePath =
            QFileInfo(), const QFileInfo& mountPoint = QFileInfo());
    /**
     * @brief Return the encryption algorithm object corresponding to the given name
     *
     * @param algorithm encryption algorithm name
     * @return QSharedPointer<EncryptionAlgorithm> encryption algorithm object
     */
    QSharedPointer<Volume::EncryptionAlgorithm> getEncryptionAlgorithm(QString algorithm);
    /**
     * @brief Return the key derivation function object corresponding to the given name
     *
     * @param function key derivation function name
     * @return QSharedPointer<VolumeHash> key derivation function object
     */
    QSharedPointer<Volume::VolumeHash> getDerivationKeyFunction(QString function);
    /**
     * @brief Return the directory where the given device is mounted
     *
     * @param devicePath Path of the device file (/dev/loopN)
     * @return QSharedPointer<QFileInfo> Path where the given device is mounted
     * @exception DeviceNotMounted
     */
    QFileInfo getDeviceMountPoint(const QFileInfo devicePath);
    /**
     * @brief Check if the given path correspond to an host device on the computer
     *
     * @param path Path of the file to check
     * @return bool True if the given path correspond to an host device on the computer.
     */
    bool isDevice(QString path);
    /**
     * @brief Check if the given volume is currently mounted
     *
     * @param volumeFile Path of the volume file
     * @return bool True if the given volume is currently mounted
     */
    bool isVolumeMounted(QFileInfo volumeFile);
    /**
     * @brief Give the next available fuse mountpoint (/tmp/.gostcrypt_aux_mntN)
     *
     * @return QSharedPointer<QFileInfo> Path of the next available fuse mountpoint
     */
    QFileInfo getFreeFuseMountPoint();
    /**
     * @brief Create a random file. (Used for volume and key file creation)
     *
     * @param path Path of the random file to create
     * @param size Size of the random file to create
     * @param id Progress tracking id used notify about random file creation progression
     * @param algorithm Name of the cipher algorithm to use for random generation
     * @param random If False null bytes will be used instead of random data
     */
    void createRandomFile(QFileInfo path, quint64 size, ProgressTrackingParameters id,
                          QString algorithm = "", bool random = true);
    /**
     * @brief Set a random key for the given cipher algorithm. This function is used by createRandomFile
     *
     * @param encryptionAlgorithm Name of the cipher algorithm for which to change the key
     * @sa CoreBase::createRandomFile
     */
    void randomizeEncryptionAlgorithmKey(QSharedPointer <Volume::EncryptionAlgorithm>
                                         encryptionAlgorithm) const;
    /**
     * @brief Give the next available default mountpoint for the given user
     *
     * @param userId UserID of the user for which we want the next available default mountpoint
     * @return QFileInfo Path of the next available default mountpoint
     */
    QFileInfo getFreeDefaultMountPoint(uid_t userId);

    void ReEncryptVolumeHeaderWithNewSalt(BufferPtr& newHeaderBuffer,
                                          QSharedPointer <Volume::VolumeHeader> header, QSharedPointer<Volume::VolumePassword> password,
                                          QSharedPointer <Volume::KeyfileList> keyfiles) const;
    /**
     * @brief Try to process the given request. Only requests which don't need root privilileges will be handled.
     *
     * @param r QVariant containing the request to process (child of CoreRequest)
     * @return bool True if the request has been processed, False otherwise. (Can return False if the request need root privileges)
     */
    bool processNonRootRequest(QVariant r);

    /**
     * @brief Send the current progress in the processing of a request to the UI using the progress in the current function and the context stored in the ProgressTrackingParameters object
     * @param subProgress Progress in the current function
     * @param id ProgressTrackingParameters object containing the request identifier and the information on where this function stand in the processing of the request
     */
    void updateProgress(qreal subProgress, ProgressTrackingParameters id);

 signals:
    DEC_REQUEST_SIGNAL(CreateVolume);
    DEC_REQUEST_SIGNAL(MountVolume);
    DEC_REQUEST_SIGNAL(DismountVolume);
    DEC_REQUEST_SIGNAL(ChangeVolumePassword);
    DEC_REQUEST_SIGNAL(GetEncryptionAlgorithms);
    DEC_REQUEST_SIGNAL(GetDerivationFunctions);
    DEC_REQUEST_SIGNAL(GetHostDevices);
    DEC_REQUEST_SIGNAL(GetMountedVolumes);
    DEC_REQUEST_SIGNAL(CreateKeyFile);
    DEC_REQUEST_SIGNAL(ProgressUpdate);
    DEC_REQUEST_SIGNAL(BackupHeader);
    DEC_REQUEST_SIGNAL(RestoreHeader);
    DEC_REQUEST_SIGNAL(BenchmarkAlgorithms);

    /**
    * @brief Send the current progress in the processing of a request to the UI
    *
    * @param requestId Request Identifier given by the User Interface module
    * @param progress Progress of the request processing time between 0 and 1
    */
    void sendProgressUpdate(quint32 requestId, qreal progress);
    /**
     * @brief Signal emitted when the program can exit (when the coreservice is closed)
     *
     */
    void exited();
    /**
     * @brief Signal emited when the password used to get root priviliges using sudo utility is necessary, that is to say when we need to launch the coreservice through sudo. This event can be emitted several times if the password is wrong.
     *
     */
    void askSudoPassword();
};


uid_t getUserId(QString username);
gid_t getGroupId(QString groupname);

/**
 * @brief Give the core object to use (CoreRoot instance if current user is root, CoreUser instance otherwise).
 *
 * @return QSharedPointer<CoreBase> core object to use
 */
QSharedPointer<CoreBase> getCore();
}
}

#endif // COREBASE_H
