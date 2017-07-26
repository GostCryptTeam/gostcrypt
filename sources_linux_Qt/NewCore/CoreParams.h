#ifndef COREPARAMS_H
#define COREPARAMS_H
#include <QVariant>
#include <QSharedPointer>
#include "SerializationUtil.h"
#include "Volume/Volume.h"
#include "Volume/Keyfile.h"
#include "Volume/Volume.h"
#include "Volume/VolumeSlot.h"
#include "Volume/VolumePassword.h"

namespace GostCrypt {
	namespace NewCore {
        bool initCoreParams();

        struct FilesystemType {
            enum Enum {
                Unknown = 0,
                None,
                FAT,
                NTFS,
                Ext2,
                Ext3,
                Ext4,
                MacOsExt,
                UFS
            };
            static Enum GetPlatformNative () {
#ifdef GST_LINUX
                return GostCrypt::NewCore::FilesystemType::Ext3;
#elif defined (GST_MACOSX)
                return GostCrypt::NewCore::FilesystemType::MacOsExt;
#elif defined (GST_FREEBSD) || defined (GST_SOLARIS)
                return GostCrypt::NewCore::FilesystemType::UFS;
#else
                return GostCrypt::NewCore::FilesystemType::FAT;
#endif
            }
        };

		struct CoreParams {};

		struct CreateVolumeParams : CoreParams {
			struct VolumeParams {
				QSharedPointer <VolumePassword> password; // password of the volume (never null)
				QSharedPointer <KeyfileList> keyfiles; // keyfiles to use
				QSharedPointer <Pkcs5Kdf> volumeHeaderKdf; // derivation key function to use (never null)
				QSharedPointer <EncryptionAlgorithm> encryptionAlgorithm; // the algorithm to use (never null)
				FilesystemType::Enum filesystem; // the filesystem to use
				uint32 filesystemClusterSize; // filesystem dependant. watch out for wrong values ! TODO
				uint32 sectorSize; // filesystem dependant. watch out for wrong values ! TODO
                DEC_SERIALIZABLE(VolumeParams);
			};
			VolumePath path; // path of the file to create or device to format
			VolumeType::Enum type; // Normal or hidden ?
			quint64 size; // size
			QSharedPointer <VolumeParams> outerVolume; // defines the outer volume (never null)
			QSharedPointer <VolumeParams> innerVolume; // defines the inner volume
			DEC_SERIALIZABLE(CreateVolumeParams);
		};

		struct ChangeVolumePasswordParams : CoreParams {
			QSharedPointer <VolumePath> path; // path of the volume we want to change the password (never null)
			QSharedPointer <VolumePassword> password; // old password, optional if volume is already opened
			QSharedPointer <KeyfileList> keyfiles; // old keyfiles, optional if volume is already opened
			QSharedPointer <Pkcs5Kdf> newVolumeHeaderKdf; // new key derivation function (never null)
			QSharedPointer <VolumePassword> newPassword; // new password (never null)
			QSharedPointer <KeyfileList> newKeyfiles; // new keyfiles
			DEC_SERIALIZABLE(ChangeVolumePasswordParams);
		};

		struct CreateKeyFileParams : CoreParams {
			FilePath file; // the path of the file to fill with random data
			DEC_SERIALIZABLE(CreateKeyFileParams);
		};

		struct MountVolumeParams : CoreParams {
			QString fileSystemOptions; // additional options for fuse
			FilesystemType::Enum fileSystemType; // Impose a filesystem
			bool noFileSystem; // does not mount the volume at the end if true
			bool preserveTimestamps; // Preserve timestamps of file ?
			QSharedPointer <KeyfileList> keyfiles; // keyfiles to mount the volume
			QSharedPointer <VolumePassword> password; // password of the volume
			QSharedPointer <DirectoryPath> mountPoint; // mountpoint of the volume
			QSharedPointer <VolumePath> path; // path of the container or device to mount
			VolumeProtection::Enum protection; // none, readonly, hiddenvolumereadonly -> to write in outer volume without touching the inner volume
			bool useBackupHeaders; // open the volume with its backup header.
			bool sharedAccessAllowed; // do we allow shared access to the container ?
			DEC_SERIALIZABLE(MountVolumeParams);
		};

		struct DismountVolumeParams : CoreParams {
			QString volumepath; // path of the file mounted, not the mount point
			DEC_SERIALIZABLE(DismountVolumeParams);
		};

		struct GetHostDevicesParams : CoreParams {
			DEC_SERIALIZABLE(GetHostDevicesParams);
		}; // no parameters

		struct GetMountedVolumesParams : CoreParams {
			VolumePath volumePath; // optional path to select VolumeInfo from one particular volume
			DEC_SERIALIZABLE(GetMountedVolumesParams);
		};

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

#endif // COREPARAMS_H
