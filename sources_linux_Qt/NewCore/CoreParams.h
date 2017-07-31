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
        bool initCoreParams();

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
                path = VolumePath("");
                type = VolumeType::Normal;
                size = 0;
                innerVolume.reset();
                outerVolume.reset(new GostCrypt::NewCore::CreateVolumeParams::VolumeParams());
            }
			struct VolumeParams {
                VolumeParams() {
                    password.reset();
                    keyfiles.reset();
                    volumeHeaderKdf.reset(); // not supposed to be null
                    encryptionAlgorithm.reset(); // not supposed to be null
                    filesystem = GetFileSystemTypePlatformNative();
                    filesystemClusterSize = 4096; // default value, not supposed to change except for very specific requests
                    sectorSize = 512; // default value, not supposed to change except for very specific requests
                }
				QSharedPointer <VolumePassword> password; // password of the volume (never null)
				QSharedPointer <KeyfileList> keyfiles; // keyfiles to use
                QString volumeHeaderKdf; // derivation key function to use
                QString encryptionAlgorithm; // the algorithm to use
                QString filesystem; // the filesystem to use
                quint32 filesystemClusterSize; // watch out for wrong values ! TODO
                quint32 sectorSize; // watch out for wrong values ! TODO
                DEC_SERIALIZABLE(VolumeParams);
			};
            QFileInfo path; // path of the file to create or device to format
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
            QString fileSystemType; // Impose a filesystem
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

        struct GetFileSystemTypesParams : CoreParams {
            QString volumetypefilter; // used to test a specific volumetype
            DEC_SERIALIZABLE(GetFileSystemTypesParams);
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
SERIALIZABLE(GostCrypt::NewCore::GetFileSystemTypesParams)

#endif // COREPARAMS_H
