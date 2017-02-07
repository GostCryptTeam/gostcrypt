/*
 Legal Notice: Some portions of the source code contained in this file were
 derived from the source code of Encryption for the Masses 2.02a, which is
 Copyright (c) 1998-2000 Paul Le Roux and which is governed by the 'License
 Agreement for Encryption for the Masses'. Modifications and additions to
 the original source code (contained in this file) and all other portions
 of this file are Copyright (c) 2003-2010 TrueCrypt Developers Association
 and are governed by the TrueCrypt License 3.0 the full text of which is
 contained in the file License.txt included in TrueCrypt binary and source
 code distribution packages. */

#pragma once

#include "Gstdefs.h"
#include "Boot/Windows/BootDefs.h"
#include "Common.h"
#include "Crypto.h"
#include "Volumes.h"
#include "Wipe.h"

#ifdef _WIN32

/* WARNING: Modifying the following values or their meanings can introduce incompatibility with previous versions. */

#define GST_IOCTL(CODE) (CTL_CODE (FILE_DEVICE_UNKNOWN, 0x800 + (CODE), METHOD_BUFFERED, FILE_ANY_ACCESS))

#define GST_IOCTL_GET_DRIVER_VERSION						GST_IOCTL (1)
#define GST_IOCTL_GET_BOOT_LOADER_VERSION				GST_IOCTL (2)
#define GST_IOCTL_MOUNT_VOLUME							GST_IOCTL (3)
#define GST_IOCTL_DISMOUNT_VOLUME						GST_IOCTL (4)
#define GST_IOCTL_DISMOUNT_ALL_VOLUMES					GST_IOCTL (5)
#define GST_IOCTL_GET_MOUNTED_VOLUMES					GST_IOCTL (6)
#define GST_IOCTL_GET_VOLUME_PROPERTIES					GST_IOCTL (7)
#define GST_IOCTL_GET_DEVICE_REFCOUNT					GST_IOCTL (8)
#define GST_IOCTL_IS_DRIVER_UNLOAD_DISABLED				GST_IOCTL (9)
#define GST_IOCTL_IS_ANY_VOLUME_MOUNTED					GST_IOCTL (10)
#define GST_IOCTL_GET_PASSWORD_CACHE_STATUS				GST_IOCTL (11)
#define GST_IOCTL_WIPE_PASSWORD_CACHE					GST_IOCTL (12)
#define GST_IOCTL_OPEN_TEST								GST_IOCTL (13)
#define GST_IOCTL_GET_DRIVE_PARTITION_INFO				GST_IOCTL (14)
#define GST_IOCTL_GET_DRIVE_GEOMETRY						GST_IOCTL (15)
#define GST_IOCTL_PROBE_REAL_DRIVE_SIZE					GST_IOCTL (16)
#define GST_IOCTL_GET_RESOLVED_SYMLINK					GST_IOCTL (17)
#define GST_IOCTL_GET_BOOT_ENCRYPTION_STATUS				GST_IOCTL (18)
#define GST_IOCTL_BOOT_ENCRYPTION_SETUP					GST_IOCTL (19)
#define GST_IOCTL_ABORT_BOOT_ENCRYPTION_SETUP			GST_IOCTL (20)
#define GST_IOCTL_GET_BOOT_ENCRYPTION_SETUP_RESULT		GST_IOCTL (21)
#define GST_IOCTL_GET_BOOT_DRIVE_VOLUME_PROPERTIES		GST_IOCTL (22)
#define GST_IOCTL_REOPEN_BOOT_VOLUME_HEADER				GST_IOCTL (23)
#define GST_IOCTL_GET_BOOT_ENCRYPTION_ALGORITHM_NAME		GST_IOCTL (24)
#define GST_IOCTL_GET_PORTABLE_MODE_STATUS				GST_IOCTL (25)
#define GST_IOCTL_SET_PORTABLE_MODE_STATUS				GST_IOCTL (26)
#define GST_IOCTL_IS_HIDDEN_SYSTEM_RUNNING				GST_IOCTL (27)
#define GST_IOCTL_GET_SYSTEM_DRIVE_CONFIG				GST_IOCTL (28)
#define GST_IOCTL_DISK_IS_WRITABLE						GST_IOCTL (29)
#define GST_IOCTL_START_DECOY_SYSTEM_WIPE				GST_IOCTL (30)
#define GST_IOCTL_ABORT_DECOY_SYSTEM_WIPE				GST_IOCTL (31)
#define GST_IOCTL_GET_DECOY_SYSTEM_WIPE_STATUS			GST_IOCTL (32)
#define GST_IOCTL_GET_DECOY_SYSTEM_WIPE_RESULT			GST_IOCTL (33)
#define GST_IOCTL_WRITE_BOOT_DRIVE_SECTOR				GST_IOCTL (34)
#define GST_IOCTL_GET_WARNING_FLAGS						GST_IOCTL (35)
#define GST_IOCTL_SET_SYSTEM_FAVORITE_VOLUME_DIRTY		GST_IOCTL (36)
#define GST_IOCTL_REREAD_DRIVER_CONFIG					GST_IOCTL (37)
#define GST_IOCTL_GET_SYSTEM_DRIVE_DUMP_CONFIG			GST_IOCTL (38)

// Legacy IOCTLs used before version 5.0
#define GST_IOCTL_LEGACY_GET_DRIVER_VERSION		466968
#define GST_IOCTL_LEGACY_GET_MOUNTED_VOLUMES		466948


/* Start of driver interface structures, the size of these structures may
   change between versions; so make sure you first send DRIVER_VERSION to
   check that it's the correct device driver */

#pragma pack (push)
#pragma pack(1)

typedef struct
{
	int nReturnCode;					/* Return code back from driver */
	BOOL FilesystemDirty;
	BOOL VolumeMountedReadOnlyAfterAccessDenied;
	BOOL VolumeMountedReadOnlyAfterDeviceWriteProtected;

	wchar_t wszVolume[GST_MAX_PATH];		/* Volume to be mounted */
	Password VolumePassword;			/* User password */
	BOOL bCache;						/* Cache passwords in driver */
	int nDosDriveNo;					/* Drive number to mount */
	uint32 BytesPerSector;
	BOOL bMountReadOnly;				/* Mount volume in read-only mode */
	BOOL bMountRemovable;				/* Mount volume as removable media */
	BOOL bExclusiveAccess;				/* Open host file/device in exclusive access mode */
	BOOL bMountManager;					/* Announce volume to mount manager */
	BOOL bPreserveTimestamp;			/* Preserve file container timestamp */
	BOOL bPartitionInInactiveSysEncScope;		/* If TRUE, we are to attempt to mount a partition located on an encrypted system drive without pre-boot authentication. */
	int nPartitionInInactiveSysEncScopeDriveNo;	/* If bPartitionInInactiveSysEncScope is TRUE, this contains the drive number of the system drive on which the partition is located. */
	BOOL SystemFavorite;
	// Hidden volume protection
	BOOL bProtectHiddenVolume;			/* TRUE if the user wants the hidden volume within this volume to be protected against being overwritten (damaged) */
	Password ProtectedHidVolPassword;	/* Password to the hidden volume to be protected against overwriting */
	BOOL UseBackupHeader;
	BOOL RecoveryMode;
} MOUNT_STRUCT;

typedef struct
{
	int nDosDriveNo;	/* Drive letter to unmount */
	BOOL ignoreOpenFiles;
	BOOL HiddenVolumeProtectionTriggered;
	int nReturnCode;	/* Return code back from driver */
} UNMOUNT_STRUCT;

typedef struct
{
	unsigned __int32 ulMountedDrives;	/* Bitfield of all mounted drive letters */
	wchar_t wszVolume[26][GST_MAX_PATH];	/* Volume names of mounted volumes */
	unsigned __int64 diskLength[26];
	int ea[26];
	int volumeType[26];	/* Volume type (e.g. PROP_VOL_TYPE_OUTER, PROP_VOL_TYPE_OUTER_VOL_WRITE_PREVENTED, etc.) */
} MOUNT_LIST_STRUCT;

typedef struct
{
	int driveNo;
	int uniqueId;
	wchar_t wszVolume[GST_MAX_PATH];
	unsigned __int64 diskLength;
	int ea;
	int mode;
	int pkcs5;
	int pkcs5Iterations;
	BOOL hiddenVolume;
	BOOL readOnly;
	BOOL removable;
	BOOL partitionInInactiveSysEncScope;
#if 0
	unsigned __int64 volumeCreationTime;	// Deprecated in v6.0
	unsigned __int64 headerCreationTime;	// Deprecated in v6.0
#endif
	uint32 volumeHeaderFlags;
	unsigned __int64 totalBytesRead;
	unsigned __int64 totalBytesWritten;
	int hiddenVolProtection;	/* Hidden volume protection status (e.g. HIDVOL_PROT_STATUS_NONE, HIDVOL_PROT_STATUS_ACTIVE, etc.) */
	int volFormatVersion;
} VOLUME_PROPERTIES_STRUCT;

typedef struct
{
	WCHAR symLinkName[GST_MAX_PATH];
	WCHAR targetName[GST_MAX_PATH];
} RESOLVE_SYMLINK_STRUCT;

typedef struct
{
	WCHAR deviceName[GST_MAX_PATH];
	PARTITION_INFORMATION partInfo;
	BOOL IsGPT;
	BOOL IsDynamic;
}
DISK_PARTITION_INFO_STRUCT;

typedef struct
{
	WCHAR deviceName[GST_MAX_PATH];
	DISK_GEOMETRY diskGeometry;
}
DISK_GEOMETRY_STRUCT;

typedef struct
{
	WCHAR DeviceName[GST_MAX_PATH];
	LARGE_INTEGER RealDriveSize;
	BOOL TimeOut;
} ProbeRealDriveSizeRequest;

typedef struct
{
	wchar_t wszFileName[GST_MAX_PATH];		// Volume to be "open tested"
	BOOL bDetectGSTBootLoader;			// Whether the driver is to determine if the first sector contains a portion of the GostCrypt Boot Loader
	BOOL GSTBootLoaderDetected;
	BOOL DetectFilesystem;
	BOOL FilesystemDetected;
} OPEN_TEST_STRUCT;


typedef enum
{
	SetupNone = 0,
	SetupEncryption,
	SetupDecryption
} BootEncryptionSetupMode;


typedef struct
{
	// New fields must be added at the end of the structure to maintain compatibility with previous versions
	BOOL DeviceFilterActive;

	uint16 BootLoaderVersion;

	BOOL DriveMounted;
	BOOL VolumeHeaderPresent;
	BOOL DriveEncrypted;

	LARGE_INTEGER BootDriveLength;

	int64 ConfiguredEncryptedAreaStart;
	int64 ConfiguredEncryptedAreaEnd;
	int64 EncryptedAreaStart;
	int64 EncryptedAreaEnd;

	uint32 VolumeHeaderSaltCrc32;

	BOOL SetupInProgress;
	BootEncryptionSetupMode SetupMode;
	BOOL TransformWaitingForIdle;

	uint32 HibernationPreventionCount;

	BOOL HiddenSystem;
	int64 HiddenSystemPartitionStart;

	// Number of times the filter driver answered that an unencrypted volume
	// is read-only (or mounted an outer/normal GostCrypt volume as read only)
	uint32 HiddenSysLeakProtectionCount;

} BootEncryptionStatus;


typedef struct
{
	BootEncryptionSetupMode SetupMode;
	WipeAlgorithmId WipeAlgorithm;
	BOOL ZeroUnreadableSectors;
	BOOL DiscardUnreadableEncryptedSectors;
} BootEncryptionSetupRequest;


typedef struct
{
	Password VolumePassword;
} ReopenBootVolumeHeaderRequest;


typedef struct
{
	char BootEncryptionAlgorithmName[256];
} GetBootEncryptionAlgorithmNameRequest;

typedef struct
{
	wchar_t DevicePath[GST_MAX_PATH];
	byte Configuration;
	BOOL DriveIsDynamic;
	uint16 BootLoaderVersion;
	byte UserConfiguration;
	char CustomUserMessage[GST_BOOT_SECTOR_USER_MESSAGE_MAX_LENGTH + 1];
} GetSystemDriveConfigurationRequest;

typedef struct
{
	WipeAlgorithmId WipeAlgorithm;
	byte WipeKey[MASTER_KEYDATA_SIZE];
} WipeDecoySystemRequest;

typedef struct
{
	BOOL WipeInProgress;
	WipeAlgorithmId WipeAlgorithm;
	int64 WipedAreaEnd;
} DecoySystemWipeStatus;

typedef struct
{
	LARGE_INTEGER Offset;
	byte Data[GST_SECTOR_SIZE_BIOS];
} WriteBootDriveSectorRequest;

typedef struct
{
	BOOL PagingFileCreationPrevented;
	BOOL SystemFavoriteVolumeDirty;
} GetWarningFlagsRequest;

typedef struct
{
	struct _DriveFilterExtension *BootDriveFilterExtension;
	BOOL HwEncryptionEnabled;
} GetSystemDriveDumpConfigRequest;

#pragma pack (pop)

#ifdef GST_WINDOWS_DRIVER
#define DRIVER_STR WIDE
#else
#define DRIVER_STR
#endif

#define GST_UNIQUE_ID_PREFIX "GostCryptVolume"
#define GST_MOUNT_PREFIX L"\\Device\\GostCryptVolume"

#define NT_MOUNT_PREFIX DRIVER_STR("\\Device\\GostCryptVolume")
#define NT_ROOT_PREFIX DRIVER_STR("\\Device\\GostCrypt")
#define DOS_MOUNT_PREFIX DRIVER_STR("\\DosDevices\\")
#define DOS_ROOT_PREFIX DRIVER_STR("\\DosDevices\\GostCrypt")
#define WIN32_ROOT_PREFIX DRIVER_STR("\\\\.\\GostCrypt")

#define GST_DRIVER_CONFIG_REG_VALUE_NAME DRIVER_STR("GostCryptConfig")
#define GST_ENCRYPTION_FREE_CPU_COUNT_REG_VALUE_NAME DRIVER_STR("GostCryptEncryptionFreeCpuCount")

// WARNING: Modifying the following values can introduce incompatibility with previous versions.
#define GST_DRIVER_CONFIG_CACHE_BOOT_PASSWORD						0x1
#define GST_DRIVER_CONFIG_CACHE_BOOT_PASSWORD_FOR_SYS_FAVORITES		0x2
#define GST_DRIVER_CONFIG_DISABLE_NONADMIN_SYS_FAVORITES_ACCESS		0x4
#define GST_DRIVER_CONFIG_DISABLE_HARDWARE_ENCRYPTION				0x8

#endif		/* _WIN32 */
