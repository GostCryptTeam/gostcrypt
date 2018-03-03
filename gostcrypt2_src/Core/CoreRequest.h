#ifndef COREPARAMS_H
#define COREPARAMS_H
#include <QVariant>
#include <QSharedPointer>
#include <QFileInfo>
#include "SerializationUtil.h"
#include "Volume/Volume.h"
#include "Volume/VolumeHeader.h"
#include "Volume/Keyfile.h"
#include "Volume/Volume.h"
#include "Volume/VolumePassword.h"
#include "Volume/VolumeType.h"

namespace GostCrypt {
namespace Core {
/**
 * @brief Initialize the CoreRequest class and subclass to make them serializable across processes
 */
void initCoreRequest();

/**
 * @brief Return the name of the prefered filesystem on the current platform
 *
 * @return Name of the prefered filesystem on the current platform
 */
QString GetFileSystemTypePlatformNative ();

/**
 * @brief Class storing all information used to report the request processing progress
 *
 */
struct ProgressTrackingParameters
{
    /**
     * @brief Constructor used to create the initial ProgressTrackingParameters within the CoreRequest object
     *
     * @param requestId Request Identifier given by the User Interface module
     */
    explicit ProgressTrackingParameters(quint32 requestId) : requestId(requestId), start(0), end(1) {}
    /**
     * @brief Default constructor used when deserializing
     *
     */
    explicit ProgressTrackingParameters() :  requestId(0), start(0), end(1) {}
    /**
     * @brief Constructor used to create sub-function ProgressTrackingParameters
     *
     * @param parent ProgressTrackingParameters of the current method call
     * @param subStart Progress achieved within the current method before the sub-function call in percentage
     * @param subEnd Progress achieved within the current method after the sub-function call in percentage
     */
    explicit ProgressTrackingParameters(ProgressTrackingParameters &parent, qreal subStart, qreal subEnd) : requestId(parent.requestId), start(parent.end*subStart+parent.start*(1-subStart)), end(parent.end*subEnd+parent.start*(1-subEnd)) {}


    //TODO implement here all ProgressTracking methods (currently MACROs)

    quint32 requestId; /**< Request Identifier given by the User Interface module */
    qreal start; /**< Progress achieved with the request processing before the current function call in percentage */
    qreal end; /**< Progress achieved with the request processing after the current function call in percentage */
    DEC_SERIALIZABLE(ProgressTrackingParameters);
};

/**
 * @brief Parent class of all requests objects used to store parameters for GostCrypt actions and transmit it to the coreservice
 *
 */
struct CoreRequest
{
    /**
     * @brief Default constructor
     *
     */
    CoreRequest() : emitResponse(true) {}
    bool emitResponse; /**< boolean true when the signal corresponding to the action must be emited after action has been successfully performed */
    QVariantMap passThrough; /**< QVariantMap storing data that need to be forwarded to the source code executed after the action has been successfully performed (in the response object) */
    ProgressTrackingParameters id; /**< Object used to report the request processing progress */
    DEC_SERIALIZABLE(CoreRequest);
};

/**
 * @brief Class storing all parameters for the volume creation action
 *
 */
struct CreateVolumeRequest : CoreRequest {
    /**
     * @brief Default constructor with default parameters
     *
     */
    CreateVolumeRequest() {
        type = Volume::VolumeType::Normal;
        size = 0;
		innerVolume.reset();
        outerVolume.reset(new GostCrypt::Core::CreateVolumeRequest::VolumeParams());
    }
    /**
     * @brief Class storing all parameters for the creation of of a volume instance inside the volume container (There can be currently be two volume instance: outer and inner (hidden) ones)
     *
     */
    struct VolumeParams {
        /**
         * @brief Default constructor with default parameters
         *
         */
        VolumeParams();
        QSharedPointer <QByteArray> password; /**< password of the volume instance */
        QSharedPointer<QList<QSharedPointer<QFileInfo>>> keyfiles; /**< keyfiles of the volume instance */
        qreal size; /**< size used within the volume container for this volume instance in percentage of available space */
        QString volumeHeaderKdf; /**< name of the key derivation function to use */
        QString encryptionAlgorithm; /**< name of the encryption algorithm to use */
        QString filesystem; /**< the name of the filesystem type to use */
        DEC_SERIALIZABLE(VolumeParams);
    };
    QFileInfo path; /**< path of the file or device to use for the volume container */
    Volume::VolumeType::Enum type; /**< Volume container type: Normal(only one outer volume instance) or Hidden (an outer volume instance and an inner hidden volume instance) */
    quint64 size; /**< size of the container in byte */
    QSharedPointer <VolumeParams> outerVolume; /**< VolumeParams object for the outer volume instance */
    QSharedPointer <VolumeParams> innerVolume; /**< VolumeParams object for the inner hidden volume instance (can be NULL if no hidden volume instance need to be created)*/
    DEC_SERIALIZABLE(CreateVolumeRequest);
};

/**
 * @brief Class storing all parameters for the volume instance password changement action
 *
 */
struct ChangeVolumePasswordRequest : CoreRequest {
    /**
     * @brief Default constructor with default parameters
     *
     */
    ChangeVolumePasswordRequest() : newVolumeHeaderKdf("HMAC-Whirlpool"), changeMasterKey(false) {}
    QFileInfo path; /**< path of the volume container hosting the volume instance */
    QSharedPointer <QByteArray> password; /**< old password of the volume instance*/
    QSharedPointer <QList<QSharedPointer<QFileInfo>>> keyfiles; /**< old keyfiles of the volume instance */
    QString newVolumeHeaderKdf; /**< name of the new key derivation function to use */
    QSharedPointer <QByteArray> newPassword; /**< new password */
    QSharedPointer <QList<QSharedPointer<QFileInfo>>> newKeyfiles; /**< new keyfiles */
    bool changeMasterKey; /**< Boolean true if we want to change the master key at the same time. This will required to reencrypt the whole volume instance. It can be usefull when we want to remove access from someone who had the previous password */
    DEC_SERIALIZABLE(ChangeVolumePasswordRequest);
};

/**
 * @brief Class storing all parameters for the key file creation action
 * This action will simply create a file of 64 bytes with random bytes
 *
 */
struct CreateKeyFileRequest : CoreRequest {
    QFileInfo file; /**< the path of the key file to create */
    DEC_SERIALIZABLE(CreateKeyFileRequest);
};

/**
 * @brief Class storing all parameters for the mount volume action
 *
 */
struct MountVolumeRequest : CoreRequest {
    /**
     * @brief Default Constructor with default parameters
     *
     */
    MountVolumeRequest();
    QString fileSystemOptions; /**< Additional options for the file system mount syscall */
    QString fileSystemType; /**< File system type to use for the filesystem mount syscall. If empty GostCrypt will try to automatically detect the filesystem type of the volume instance */
    bool doMount; /**< Boolean true if we want GostCrypt to mount the volume instance filesystem at the end of the mount action. If False, GostCrypt will only provide the block device */
    bool preserveTimestamps; /**< Boolean true if GostCrypt should not modify the Access and modification time of the file container */
    QSharedPointer <QList<QSharedPointer<QFileInfo>>> keyfiles; /**< keyfiles of the volume instance to mount */
    QSharedPointer <QByteArray> password; /**< password of the volume instance to mount */
    QFileInfo mountPoint; /**< path of the directory where the volume instance should be mounted. If left empty, GostCrypt will determined where the system usually mount devices and create a mount directory there. (example: /media/[username]). This parameter will not be used if doMount is set to false. */
    QFileInfo path; /**< path of the volume container (file or device) to mount */
    Volume::VolumeProtection::Enum protection; /**< Type of protection to use while mounting the volume */
    QSharedPointer <QByteArray> protectionPassword; /**< password of the hidden inner volume instance used when mounting outer volume instance in protected mode (HiddenVolumeReadOnly) */
    QSharedPointer <QList<QSharedPointer<QFileInfo>>> protectionKeyfiles; /**< keyfiles of the hidden inner volume instance used when mounting outer volume instance in protected mode (HiddenVolumeReadOnly) */
    bool useBackupHeaders; /**< Boolean true when we want to use the backup header to mount the volume instance. This option can be useful when the main header is corrupted */
    QString mountedForUser; /**< Name of the user who will own the volume block file once mounted. If empty, it will be the current user. */
    QString mountedForGroup; /**< Name of the group who will own the volume block file once mounted. If empty, it will be the current group. */
    bool forVolumeCreation; /**< Boolean true if this action had been requested during the volume creation action. Should always be set to false when the request come from the UI module. This parameter will be used to continue the volume creation after the volume instance has been mounted. For internal use only. */
    bool isDevice; /**< Boolean true if the volume container is a partition, false if it is a file. For internal use only, GostCrypt will always overwrite this parameter and derive the volume container type from the volume container path. */
    QFileInfo fuseMountPoint; /**< path of the mount directory for the intermediate fuse filesystem. For internal use only, GostCrypt will always overwrite this parameter and determine the correct path itself. */
    DEC_SERIALIZABLE(MountVolumeRequest);
};

/**
 * @brief Class storing all parameters for the dismount volume action
 *
 */
struct DismountVolumeRequest : CoreRequest {
    /**
     * @brief Default Constructor with default parameters
     *
     */
    DismountVolumeRequest();
    bool all; /**< Boolean true if all the currently mounted volume should be dismounted */
    QFileInfo volumePath; /**< path of the volume container to dismount */
    bool force; /**< Boolean true to force the dismounting of the volume even if busy. This can cause data loss. */
    bool forVolumeCreation; /**< Boolean true if this action had been requested during the volume creation action. Should always be set to false when the request come from the UI module. This parameter will be used to continue the volume creation after the volume instance has been dismounted. For internal use only. */
    DEC_SERIALIZABLE(DismountVolumeRequest);
};

/**
 * @brief Class storing all parameters for the action to get the current host devices on the system.
 *
 */
struct GetHostDevicesRequest : CoreRequest {
    DEC_SERIALIZABLE(GetHostDevicesRequest);
};

/**
 * @brief Class storing all parameters for the action to get the current mounted GostCrypt volumes on the system and information about them.
 *
 */
struct GetMountedVolumesRequest : CoreRequest {
    /**
     * @brief Default Constructor with default parameters
     *
     */
    GetMountedVolumesRequest();
    bool all; /**< Boolean true if the information about all currently mounted GostCrypt volumes should be returned (true by default) */
    QFileInfo volumePath; /**< Path of the volume container (file or device) for which we want to get mounting information. This parameter is not use if the boolean all is set to true. */
    DEC_SERIALIZABLE(GetMountedVolumesRequest);
};

/**
 * @brief Class storing all parameters for the action to get the list of available encryption algorithms
 *
 */
struct GetEncryptionAlgorithmsRequest : CoreRequest {
    DEC_SERIALIZABLE(GetEncryptionAlgorithmsRequest);
};

/**
 * @brief Class storing all parameters for the action to get the list of available key derivation functions
 *
 */
struct GetDerivationFunctionsRequest : CoreRequest {
    DEC_SERIALIZABLE(GetDerivationFunctionsRequest);
};

/**
 * @brief Class storing all parameters for the volume header backup action
 * This action will save the headers of the volume instances contained inside the given volume container
 */
struct BackupHeaderRequest : CoreRequest {
    /**
     * @brief Default Constructor with default parameters
     *
     */
    BackupHeaderRequest();
    QFileInfo volumePath; /**< path of the volume container for which we want to backup the header(s) */
    QFileInfo backupHeaderFile; /**< path of the file where to store the backup header */
    QSharedPointer <QList<QSharedPointer<QFileInfo>>> keyfiles; /**< keyfiles of the outer volume instance */
    QSharedPointer <QByteArray> password; /**< password of the outer volume instance */
    QSharedPointer <QList<QSharedPointer<QFileInfo>>> hiddenVolumeKeyfiles; /**< keyfiles of the inner hidden volume instance */
    QSharedPointer <QByteArray> hiddenVolumePassword; /**< password of the inner hidden volume instance */
    bool hiddenVolume; /**< Boolean true if the volume container contain an inner hidden volume instance and be want to backup its header too. If set to true, this parameters hiddenVolumePassword and hiddenVolumeKeyfiles must be provided */
    DEC_SERIALIZABLE(BackupHeaderRequest);
};

/**
 * @brief Class storing all parameters for the volume header restore action
 * This action will restore the header of the volume instance corresponding to given credentials.
 * It will either use the internal backup or an external header backup file.
 * For this action to succeed the password and keyfiles of the backup header should correspond to the password and keyfiles of the volume instance for which to restore the header
 * TODO: Implement a way to restore the header when the password have changed
 */
struct RestoreHeaderRequest : CoreRequest {
    /**
     * @brief Default Constructor with default parameters
     *
     */
    RestoreHeaderRequest();
    QFileInfo volumePath; /**< path of the volume container for which to restore the header */
    bool useInternalBackup; /**< Boolean true if the internal backup header should be used. If set to false the parameter backupHeaderFile should be provided. */
    QFileInfo backupHeaderFile; /**< Path of the external backup header file from which to restore the header. Will not be used if useInternalBackup is set to true. */
    QSharedPointer <QList<QSharedPointer<QFileInfo>>> keyfiles; /**< keyfiles of the volume instance for which to restore the header. These keyfiles will also be used to decrypt the backup header */
    QSharedPointer <QByteArray> password; /**< password of the volume instance for which to restore the header. This password will also be used to decrypt the backup header */
    DEC_SERIALIZABLE(RestoreHeaderRequest);
};

/**
 * @brief Class storing all parameters for the encryption algorithm benchmark action
 *
 */
struct BenchmarkAlgorithmsRequest : CoreRequest {
    quint32 bufferSize; /**< Size of the buffer to encrypt/decrypt for the benchmark in bytes (If the buffer size is too small and the time to encrypt/decrypt is lower than 20ms , the buffer will be encrypted/decrypted serval time to improve benchmark result accuracy) */
    DEC_SERIALIZABLE(BenchmarkAlgorithmsRequest);
};

}
}

SERIALIZABLE(GostCrypt::Core::CoreRequest)
SERIALIZABLE(GostCrypt::Core::CreateVolumeRequest)
SERIALIZABLE(GostCrypt::Core::CreateVolumeRequest::VolumeParams)
SERIALIZABLE(GostCrypt::Core::ChangeVolumePasswordRequest)
SERIALIZABLE(GostCrypt::Core::CreateKeyFileRequest)
SERIALIZABLE(GostCrypt::Core::MountVolumeRequest)
SERIALIZABLE(GostCrypt::Core::DismountVolumeRequest)
SERIALIZABLE(GostCrypt::Core::GetHostDevicesRequest)
SERIALIZABLE(GostCrypt::Core::GetMountedVolumesRequest)
SERIALIZABLE(GostCrypt::Core::GetEncryptionAlgorithmsRequest)
SERIALIZABLE(GostCrypt::Core::GetDerivationFunctionsRequest)
SERIALIZABLE(GostCrypt::Core::ProgressTrackingParameters)
SERIALIZABLE(GostCrypt::Core::BackupHeaderRequest)
SERIALIZABLE(GostCrypt::Core::RestoreHeaderRequest)
SERIALIZABLE(GostCrypt::Core::BenchmarkAlgorithmsRequest)

#endif // COREPARAMS_H
