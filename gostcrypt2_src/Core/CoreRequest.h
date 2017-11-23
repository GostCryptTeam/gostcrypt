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
#include "Volume/VolumeSlot.h"
#include "Volume/VolumePassword.h"
#include "Volume/VolumeType.h"

namespace GostCrypt {
namespace Core {
void initCoreRequest();

static QString GetFileSystemTypePlatformNative () {
#ifdef GST_LINUX
    return "ext3";
#elif defined (GST_MACOSX)
    return "MacOsExt";
#elif defined (GST_FREEBSD) || defined (GST_SOLARIS)
    return "fs";
#else
    return "fat";
#endif
}

struct ProgressTrackingParameters
{
    ProgressTrackingParameters(quint32 requestId) : requestId(requestId), start(0), end(1) {}
    ProgressTrackingParameters() : start(0), end(1) {}
    ProgressTrackingParameters(ProgressTrackingParameters &parent, qreal subStart, qreal subEnd) : requestId(parent.requestId), start(parent.end*subStart+parent.start*(1-subStart)), end(parent.end*subEnd+parent.start*(1-subEnd)) {}


    quint32 requestId;
    qreal start;
    qreal end;
    DEC_SERIALIZABLE(ProgressTrackingParameters);
};

struct CoreRequest
{
    CoreRequest() : emitResponse(true) {}
    bool emitResponse;
    QVariantMap passThrough;
	ProgressTrackingParameters id;
    DEC_SERIALIZABLE(CoreRequest);
};

struct CreateVolumeRequest : CoreRequest {
    CreateVolumeRequest() {
        type = Volume::VolumeType::Normal;
        size = 0;
		innerVolume.reset();
        outerVolume.reset(new GostCrypt::Core::CreateVolumeRequest::VolumeParams());
    }
    struct VolumeParams {
        VolumeParams() {
            filesystem = GetFileSystemTypePlatformNative();
            size = 0;
            volumeHeaderKdf = "HMAC-Whirlpool";
            encryptionAlgorithm = "Gost Grasshopper";
        }
        QSharedPointer <QByteArray> password; // password of the volume (never null)
        QSharedPointer<QList<QSharedPointer<QFileInfo>>> keyfiles; // keyfiles to use
        qreal size; // size of the volume in percentage
        QString volumeHeaderKdf; // derivation key function to use
        QString encryptionAlgorithm; // the algorithm to use
        QString filesystem; // the filesystem to use
        DEC_SERIALIZABLE(VolumeParams);
    };
    QSharedPointer <QFileInfo> path; // path of the file to create or device to format
    Volume::VolumeType::Enum type; // Normal or hidden ?
    quint64 size; // size of the container
    QSharedPointer <VolumeParams> outerVolume; // defines the outer volume (never null)
    QSharedPointer <VolumeParams> innerVolume; // defines the inner volume
    DEC_SERIALIZABLE(CreateVolumeRequest);
};

struct ChangeVolumePasswordRequest : CoreRequest {
    ChangeVolumePasswordRequest() : newVolumeHeaderKdf("HMAC-Whirlpool") {}
    QSharedPointer <QFileInfo> path; // path of the volume we want to change the password (never null)
    QSharedPointer <QByteArray> password; // old password, optional if volume is already opened
    QSharedPointer <QList<QSharedPointer<QFileInfo>>> keyfiles; // old keyfiles, optional if volume is already opened
    QString newVolumeHeaderKdf; // new key derivation function (never null)
    QSharedPointer <QByteArray> newPassword; // new password (never null)
    QSharedPointer <QList<QSharedPointer<QFileInfo>>> newKeyfiles; // new keyfiles
    DEC_SERIALIZABLE(ChangeVolumePasswordRequest);
};

struct CreateKeyFileRequest : CoreRequest {
    QSharedPointer<QFileInfo> file; // the path of the file to fill with random data
    DEC_SERIALIZABLE(CreateKeyFileRequest);
};

struct MountVolumeRequest : CoreRequest {
    MountVolumeRequest();
    QString fileSystemOptions; // additional options for fuse
    QString fileSystemType; // Impose a filesystem
    bool doMount; // does mount the volume at the end if true
    bool isDevice;
    bool preserveTimestamps; // Preserve timestamps of file ?
    QSharedPointer <QList<QSharedPointer<QFileInfo>>> keyfiles; // keyfiles to mount the volume
    QSharedPointer <QByteArray> password; // password of the volume
    QSharedPointer <QFileInfo> mountPoint; // mountpoint of the volume
    QSharedPointer <QFileInfo> path; // path of the container or device to mount
    QSharedPointer <QFileInfo> fuseMountPoint; // mountpoint for the special fuse filesystem
    Volume::VolumeProtection::Enum protection; // none, readonly, hiddenvolumereadonly -> to write in outer volume without touching the inner volume
    QSharedPointer <QByteArray> protectionPassword; // password to mount the hidden protected volume
    QSharedPointer <QList<QSharedPointer<QFileInfo>>> protectionKeyfiles; // keyfiles to mount the hidden protected volume
    bool useBackupHeaders; // open the volume with its backup header.
    bool sharedAccessAllowed; // do we allow shared access to the container ? TODO make it actually do something
    QString mountedForUser;
    QString mountedForGroup;
    bool forVolumeCreation;
    DEC_SERIALIZABLE(MountVolumeRequest);
};

struct DismountVolumeRequest : CoreRequest {
    DismountVolumeRequest();
    QSharedPointer<QFileInfo> volumePath; // path of the file mounted, not the mount point
    bool force;
    bool forVolumeCreation;
    DEC_SERIALIZABLE(DismountVolumeRequest);
};

struct GetHostDevicesRequest : CoreRequest {
    DEC_SERIALIZABLE(GetHostDevicesRequest);
}; // no parameters

struct GetMountedVolumesRequest : CoreRequest {
    QSharedPointer<QFileInfo> volumePath; // optional path to select VolumeInfo from one particular volume
    DEC_SERIALIZABLE(GetMountedVolumesRequest);
};

struct GetEncryptionAlgorithmsRequest : CoreRequest {
    DEC_SERIALIZABLE(GetEncryptionAlgorithmsRequest);
}; // no parameters

struct GetDerivationFunctionsRequest : CoreRequest {
    DEC_SERIALIZABLE(GetDerivationFunctionsRequest);
}; // no parameters
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

#endif // COREPARAMS_H
