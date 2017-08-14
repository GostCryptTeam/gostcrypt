#ifndef COREPARAMS_H
#define COREPARAMS_H
#include <QVariant>
#include <QSharedPointer>
#include <QFileInfo>
#include "SerializationUtil.h"
#include "Volume/Volume.h"
#include "Volume/Keyfile.h"
#include "Volume/Volume.h"
#include "Volume/VolumeSlot.h"
#include "Volume/VolumePassword.h"

namespace GostCrypt {
	namespace NewCore {
        void initCoreParams();

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

		struct CoreParams {};

		struct CreateVolumeParams : CoreParams {
            CreateVolumeParams(){
                type = VolumeType::Normal;
                size = 0;
                innerVolume.reset();
                outerVolume.reset(new GostCrypt::NewCore::CreateVolumeParams::VolumeParams());
            }
			struct VolumeParams {
                VolumeParams() {
                    password.reset();
                    keyfiles.reset();
                    filesystem = GetFileSystemTypePlatformNative();
                    size = 0;
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
			VolumeType::Enum type; // Normal or hidden ?
            quint64 size; // size of the container
			QSharedPointer <VolumeParams> outerVolume; // defines the outer volume (never null)
			QSharedPointer <VolumeParams> innerVolume; // defines the inner volume
			DEC_SERIALIZABLE(CreateVolumeParams);
		};

		struct ChangeVolumePasswordParams : CoreParams {
			QSharedPointer <QFileInfo> path; // path of the volume we want to change the password (never null)
			QSharedPointer <QByteArray> password; // old password, optional if volume is already opened
			QSharedPointer <QList<QSharedPointer<QFileInfo>>> keyfiles; // old keyfiles, optional if volume is already opened
			QString newVolumeHeaderKdf; // new key derivation function (never null)
			QSharedPointer <QByteArray> newPassword; // new password (never null)
			QSharedPointer <QList<QSharedPointer<QFileInfo>>> newKeyfiles; // new keyfiles
			DEC_SERIALIZABLE(ChangeVolumePasswordParams);
		};

		struct CreateKeyFileParams : CoreParams {
            QSharedPointer<QFileInfo> file; // the path of the file to fill with random data
			DEC_SERIALIZABLE(CreateKeyFileParams);
		};

		struct MountVolumeParams : CoreParams {
			QString fileSystemOptions; // additional options for fuse
            QString fileSystemType; // Impose a filesystem
			bool doMount; // does mount the volume at the end if true
			bool preserveTimestamps; // Preserve timestamps of file ?
			QSharedPointer <QList<QSharedPointer<QFileInfo>>> keyfiles; // keyfiles to mount the volume
			QSharedPointer <QByteArray> password; // password of the volume
			QSharedPointer <QFileInfo> mountPoint; // mountpoint of the volume
			QSharedPointer <QFileInfo> path; // path of the container or device to mount
			VolumeProtection::Enum protection; // none, readonly, hiddenvolumereadonly -> to write in outer volume without touching the inner volume
			QSharedPointer <QByteArray> protectionPassword; // password to mount the hidden protected volume
			QSharedPointer <QList<QSharedPointer<QFileInfo>>> protectionKeyfiles; // keyfiles to mount the hidden protected volume
			bool useBackupHeaders; // open the volume with its backup header.
			bool sharedAccessAllowed; // do we allow shared access to the container ?
			DEC_SERIALIZABLE(MountVolumeParams);
		};

		struct DismountVolumeParams : CoreParams {
            QSharedPointer<QFileInfo> volumepath; // path of the file mounted, not the mount point
            bool force;
            DEC_SERIALIZABLE(DismountVolumeParams);
		};

		struct GetHostDevicesParams : CoreParams {
			DEC_SERIALIZABLE(GetHostDevicesParams);
		}; // no parameters

		struct GetMountedVolumesParams : CoreParams {
            QSharedPointer<QFileInfo> volumePath; // optional path to select VolumeInfo from one particular volume
			DEC_SERIALIZABLE(GetMountedVolumesParams);
		};

        struct GetEncryptionAlgorithmsParams : CoreParams {
            DEC_SERIALIZABLE(GetEncryptionAlgorithmsParams);
        }; // no parameters

        struct GetDerivationFunctionsParams : CoreParams {
            DEC_SERIALIZABLE(GetDerivationFunctionsParams);
        }; // no parameters

        struct ExitParams : CoreParams {
            DEC_SERIALIZABLE(ExitParams);
        }; // no parameters

	}
}

SERIALIZABLE(GostCrypt::NewCore::CoreParams)
SERIALIZABLE(GostCrypt::NewCore::CreateVolumeParams)
SERIALIZABLE(GostCrypt::NewCore::CreateVolumeParams::VolumeParams)
SERIALIZABLE(GostCrypt::NewCore::ChangeVolumePasswordParams)
SERIALIZABLE(GostCrypt::NewCore::CreateKeyFileParams)
SERIALIZABLE(GostCrypt::NewCore::MountVolumeParams)
SERIALIZABLE(GostCrypt::NewCore::DismountVolumeParams)
SERIALIZABLE(GostCrypt::NewCore::GetHostDevicesParams)
SERIALIZABLE(GostCrypt::NewCore::GetMountedVolumesParams)
SERIALIZABLE(GostCrypt::NewCore::GetEncryptionAlgorithmsParams)
SERIALIZABLE(GostCrypt::NewCore::GetDerivationFunctionsParams)
SERIALIZABLE(GostCrypt::NewCore::ExitParams)

#endif // COREPARAMS_H
