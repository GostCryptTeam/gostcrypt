/**
 *
 *	\file DriveFilter.c
 *	\brief Manage the link between the application, the driver, and the device
 *	\version 1.3.1
 *	\date 13/12/2016
 *
 */

/**
 *
 *	Copyright (c) 2008-2012 TrueCrypt Developers Association. All rights reserved.
 *	
 *	Governed by the TrueCrypt License 3.0 the full text of which is contained in
 *	the file License.txt included in TrueCrypt binary and source code distribution
 *	packages.
 *
*/

#include "Gstdefs.h"
#include <ntddk.h>
#include <ntddvol.h>
#include "Cache.h"
#include "Crc.h"
#include "Crypto.h"
#include "Apidrvr.h"
#include "EncryptedIoQueue.h"
#include "Common/Endian.h"
#include "Ntdriver.h"
#include "Ntvol.h"
#include "Volumes.h"
#include "VolumeFilter.h"
#include "Wipe.h"
#include "DriveFilter.h"
#include "Boot/Windows/BootCommon.h"

static BOOL DeviceFilterActive = FALSE;

BOOL BootArgsValid = FALSE;
BootArguments BootArgs;
static uint16 BootLoaderSegment;
static BOOL BootDriveSignatureValid = FALSE;

static KMUTEX MountMutex;

static volatile BOOL BootDriveFound = FALSE;
static DriveFilterExtension *BootDriveFilterExtension = NULL;
static LARGE_INTEGER BootDriveLength;

static BOOL CrashDumpEnabled = FALSE;
static BOOL HibernationEnabled = FALSE;

static BOOL LegacyHibernationDriverFilterActive = FALSE;
static byte *HibernationWriteBuffer = NULL;
static MDL *HibernationWriteBufferMdl = NULL;

static uint32 HibernationPreventionCount = 0;

static BootEncryptionSetupRequest SetupRequest;
static volatile BOOL SetupInProgress = FALSE;
PKTHREAD EncryptionSetupThread = NULL;
static volatile BOOL EncryptionSetupThreadAbortRequested;
static KSPIN_LOCK SetupStatusSpinLock;
static int64 SetupStatusEncryptedAreaEnd;
static BOOL TransformWaitingForIdle;
static NTSTATUS SetupResult;

static WipeDecoySystemRequest WipeDecoyRequest;
static volatile BOOL DecoySystemWipeInProgress = FALSE;
static volatile BOOL DecoySystemWipeThreadAbortRequested;
static KSPIN_LOCK DecoySystemWipeStatusSpinLock;
static int64 DecoySystemWipedAreaEnd;
PKTHREAD DecoySystemWipeThread = NULL;
static NTSTATUS DecoySystemWipeResult;


/**
 *
 *	\fn NTSTATUS LoadBootArguments ()
 *	\brief Read the boot part segment by segment and set the parameters for the cryptology, where the partition starts ...
 *	\return NTSTATUS status
 *
 */
NTSTATUS LoadBootArguments ()
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PHYSICAL_ADDRESS bootArgsAddr;
	byte *mappedBootArgs;
	uint16 bootLoaderSegment;

	KeInitializeMutex (&MountMutex, 0);

	//read each segment of the boot loader section
	for (bootLoaderSegment = GST_BOOT_LOADER_SEGMENT;
		bootLoaderSegment >= GST_BOOT_LOADER_SEGMENT - 64 * 1024 / 16 && status != STATUS_SUCCESS;
		bootLoaderSegment -= 32 * 1024 / 16)
	{
		bootArgsAddr.QuadPart = (bootLoaderSegment << 4) + GST_BOOT_LOADER_ARGS_OFFSET;
		Dump ("Checking BootArguments at 0x%x\n", bootArgsAddr.LowPart);

		mappedBootArgs = MmMapIoSpace (bootArgsAddr, sizeof (BootArguments), MmCached);
		if (!mappedBootArgs)
			return STATUS_INSUFFICIENT_RESOURCES;

		if (GST_IS_BOOT_ARGUMENTS_SIGNATURE (mappedBootArgs))
		{
			BootArguments *bootArguments = (BootArguments *) mappedBootArgs;
			Dump ("BootArguments found at 0x%x\n", bootArgsAddr.LowPart);

			DumpMem (mappedBootArgs, sizeof (BootArguments));

			if (bootArguments->BootLoaderVersion == VERSION_NUM
				&& bootArguments->BootArgumentsCrc32 != GetCrc32 ((byte *) bootArguments, (int) ((byte *) &bootArguments->BootArgumentsCrc32 - (byte *) bootArguments)))
			{
				Dump ("BootArguments CRC incorrect\n");
				GST_BUG_CHECK (STATUS_CRC_ERROR);
			}

			BootLoaderSegment = bootLoaderSegment;

			BootArgs = *bootArguments;
			BootArgsValid = TRUE;
			memset (bootArguments, 0, sizeof (*bootArguments));

			if (BootArgs.BootLoaderVersion < 0x600)
			{
				BootArgs.HiddenSystemPartitionStart = 0;
				BootArgs.DecoySystemPartitionStart = 0;
			}

			if (BootArgs.BootLoaderVersion < 0x630)
				BootArgs.Flags = 0;

			BootDriveSignatureValid = (BootArgs.BootLoaderVersion >= 0x710);

			Dump ("BootLoaderVersion = %x\n", (int) BootArgs.BootLoaderVersion);
			Dump ("HeaderSaltCrc32 = %x\n", (int) BootArgs.HeaderSaltCrc32);
			Dump ("CryptoInfoOffset = %x\n", (int) BootArgs.CryptoInfoOffset);
			Dump ("CryptoInfoLength = %d\n", (int) BootArgs.CryptoInfoLength);
			Dump ("HiddenSystemPartitionStart = %I64u\n", BootArgs.HiddenSystemPartitionStart);
			Dump ("DecoySystemPartitionStart = %I64u\n", BootArgs.DecoySystemPartitionStart);
			Dump ("Flags = %x\n", BootArgs.Flags);
			Dump ("BootDriveSignature = %x\n", BootArgs.BootDriveSignature);
			Dump ("BootArgumentsCrc32 = %x\n", BootArgs.BootArgumentsCrc32);

			if (CacheBootPassword && BootArgs.BootPassword.Length > 0)
				AddPasswordToCache (&BootArgs.BootPassword);

			status = STATUS_SUCCESS;
		}

		MmUnmapIoSpace (mappedBootArgs, sizeof (BootArguments));
	}

	return status;
}


/**
 *
 *	\fn NTSTATUS DriveFilterAddDevice (PDRIVER_OBJECT driverObject, PDEVICE_OBJECT pdo)
 *	\brief Add to the driveFilter a device and initialise it's value of encryptment.
 *	\param[in] PDEVICE_OBJECT pdo
 *	\param[in] PDRIVER_OBJECT driverObject
 *	\return NTSTATUS status
 *
 */
NTSTATUS DriveFilterAddDevice (PDRIVER_OBJECT driverObject, PDEVICE_OBJECT pdo)
{
	DriveFilterExtension *Extension;
	NTSTATUS status;
	PDEVICE_OBJECT filterDeviceObject = NULL;
	PDEVICE_OBJECT attachedDeviceObject;

	Dump ("DriveFilterAddDevice pdo=%p\n", pdo);

	attachedDeviceObject = IoGetAttachedDeviceReference (pdo);
	status = IoCreateDevice (driverObject, sizeof (DriveFilterExtension), NULL, attachedDeviceObject->DeviceType, 0, FALSE, &filterDeviceObject);

	ObDereferenceObject (attachedDeviceObject);

	if (!NT_SUCCESS (status))
	{
		filterDeviceObject = NULL;
		goto err;
	}

	Extension = (DriveFilterExtension *) filterDeviceObject->DeviceExtension;
	memset (Extension, 0, sizeof (DriveFilterExtension));

	Extension->LowerDeviceObject = IoAttachDeviceToDeviceStack (filterDeviceObject, pdo);  // IoAttachDeviceToDeviceStackSafe() is not required in AddDevice routine and is also unavailable on Windows 2000 SP4
	if (!Extension->LowerDeviceObject)
	{
		status = STATUS_DEVICE_REMOVED;
		goto err;
	}

	Extension->IsDriveFilterDevice = Extension->Queue.IsFilterDevice = TRUE;
	Extension->DeviceObject = Extension->Queue.DeviceObject = filterDeviceObject;
	Extension->Pdo = pdo;
	
	Extension->Queue.LowerDeviceObject = Extension->LowerDeviceObject;
	IoInitializeRemoveLock (&Extension->Queue.RemoveLock, 'LRCT', 0, 0);

	Extension->ConfiguredEncryptedAreaStart = -1;
	Extension->ConfiguredEncryptedAreaEnd = -1;
	Extension->Queue.EncryptedAreaStart = -1;
	Extension->Queue.EncryptedAreaEnd = -1;
	Extension->Queue.EncryptedAreaEndUpdatePending = FALSE;

	filterDeviceObject->Flags |= Extension->LowerDeviceObject->Flags & (DO_DIRECT_IO | DO_BUFFERED_IO | DO_POWER_PAGABLE);
	filterDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	DeviceFilterActive = TRUE;
	return status;

err:
	if (filterDeviceObject)
	{
		if (Extension->LowerDeviceObject)
			IoDetachDevice (Extension->LowerDeviceObject);

		IoDeleteDevice (filterDeviceObject);
	}

	return status;
}


/**
 *
 *	\fn static void DismountDrive (DriveFilterExtension *Extension, BOOL stopIoQueue)
 *	\brief Dismount the drive sent in argument
 *	\param[in] BOOL stopIoQueue
 *	\param[in] DriveFilterExtension * Extension
 *	\return static void
 *
 */
static void DismountDrive (DriveFilterExtension *Extension, BOOL stopIoQueue)
{
	Dump ("Dismounting drive\n");
	ASSERT (Extension->DriveMounted);
	
	if (stopIoQueue && EncryptedIoQueueIsRunning (&Extension->Queue))
		EncryptedIoQueueStop (&Extension->Queue);

	crypto_close (Extension->Queue.CryptoInfo);
	Extension->Queue.CryptoInfo = NULL;

	crypto_close (Extension->HeaderCryptoInfo);
	Extension->HeaderCryptoInfo = NULL;

	Extension->DriveMounted = FALSE;
}


/**
 *
 *	\fn static NTSTATUS MountDrive (DriveFilterExtension *Extension, Password *password, uint32 *headerSaltCrc32)
 *	\brief Mount the drive passed in argument and decrypt with the password return status in case of error. When mounting set the drive variables.
 *	\param[in] uint32 * headerSaltCrc32
 *	\param[in] Password * password
 *	\param[in] DriveFilterExtension * Extension
 *	\return static NTSTATUS status
 *
 */
static NTSTATUS MountDrive (DriveFilterExtension *Extension, Password *password, uint32 *headerSaltCrc32)
{
	BOOL hiddenVolume = (BootArgs.HiddenSystemPartitionStart != 0);
	int64 hiddenHeaderOffset = BootArgs.HiddenSystemPartitionStart + GST_HIDDEN_VOLUME_HEADER_OFFSET;
	NTSTATUS status;
	LARGE_INTEGER offset;
	char *header;

	Dump ("MountDrive pdo=%p\n", Extension->Pdo);
	ASSERT (KeGetCurrentIrql() == PASSIVE_LEVEL);

	// Check boot drive signature first (header CRC search could fail if a user restored the header to a non-boot drive)
	if (BootDriveSignatureValid)
	{
		byte mbr[GST_SECTOR_SIZE_BIOS];

		offset.QuadPart = 0;
		status = GSTReadDevice (Extension->LowerDeviceObject, mbr, offset, GST_SECTOR_SIZE_BIOS);

		if (NT_SUCCESS (status) && BootArgs.BootDriveSignature != *(uint32 *) (mbr + 0x1b8))
			return STATUS_UNSUCCESSFUL;
	}

	header = GSTalloc (GST_BOOT_ENCRYPTION_VOLUME_HEADER_SIZE);
	if (!header)
		return STATUS_INSUFFICIENT_RESOURCES;

	offset.QuadPart = hiddenVolume ? hiddenHeaderOffset : GST_BOOT_VOLUME_HEADER_SECTOR_OFFSET;
	Dump ("Reading volume header at %I64u\n", offset.QuadPart);

	status = GSTReadDevice (Extension->LowerDeviceObject, header, offset, GST_BOOT_ENCRYPTION_VOLUME_HEADER_SIZE);
	if (!NT_SUCCESS (status))
	{
		Dump ("GSTReadDevice error %x\n", status);
		goto ret;
	}

	if (headerSaltCrc32)
	{
		uint32 saltCrc = GetCrc32 (header, PKCS5_SALT_SIZE);

		if (saltCrc != *headerSaltCrc32)
		{
			status = STATUS_UNSUCCESSFUL;
			goto ret;
		}

		Extension->VolumeHeaderSaltCrc32 = saltCrc;
	}

	Extension->HeaderCryptoInfo = crypto_open();
	if (!Extension->HeaderCryptoInfo)
	{
		status = STATUS_INSUFFICIENT_RESOURCES;
		goto ret;
	}

	if (ReadVolumeHeader (!hiddenVolume, header, password, &Extension->Queue.CryptoInfo, Extension->HeaderCryptoInfo) == 0)
	{
		// Header decrypted
		status = STATUS_SUCCESS;
		Dump ("Header decrypted\n");
			
		if (Extension->Queue.CryptoInfo->hiddenVolume)
		{
			int64 hiddenPartitionOffset = BootArgs.HiddenSystemPartitionStart;
			Dump ("Hidden volume start offset = %I64d\n", Extension->Queue.CryptoInfo->EncryptedAreaStart.Value + hiddenPartitionOffset);
			
			Extension->HiddenSystem = TRUE;

			Extension->Queue.RemapEncryptedArea = TRUE;
			Extension->Queue.RemappedAreaOffset = hiddenPartitionOffset + Extension->Queue.CryptoInfo->EncryptedAreaStart.Value - BootArgs.DecoySystemPartitionStart;
			Extension->Queue.RemappedAreaDataUnitOffset = Extension->Queue.CryptoInfo->EncryptedAreaStart.Value / ENCRYPTION_DATA_UNIT_SIZE - BootArgs.DecoySystemPartitionStart / ENCRYPTION_DATA_UNIT_SIZE;
			
			Extension->Queue.CryptoInfo->EncryptedAreaStart.Value = BootArgs.DecoySystemPartitionStart;
			
			if (Extension->Queue.CryptoInfo->VolumeSize.Value > hiddenPartitionOffset - BootArgs.DecoySystemPartitionStart)
				GST_THROW_FATAL_EXCEPTION;

			Dump ("RemappedAreaOffset = %I64d\n", Extension->Queue.RemappedAreaOffset);
			Dump ("RemappedAreaDataUnitOffset = %I64d\n", Extension->Queue.RemappedAreaDataUnitOffset);
		}
		else
		{
			Extension->HiddenSystem = FALSE;
			Extension->Queue.RemapEncryptedArea = FALSE;
		}

		Extension->ConfiguredEncryptedAreaStart = Extension->Queue.CryptoInfo->EncryptedAreaStart.Value;
		Extension->ConfiguredEncryptedAreaEnd = Extension->Queue.CryptoInfo->EncryptedAreaStart.Value + Extension->Queue.CryptoInfo->VolumeSize.Value - 1;

		Extension->Queue.EncryptedAreaStart = Extension->Queue.CryptoInfo->EncryptedAreaStart.Value;
		Extension->Queue.EncryptedAreaEnd = Extension->Queue.CryptoInfo->EncryptedAreaStart.Value + Extension->Queue.CryptoInfo->EncryptedAreaLength.Value - 1;

		if (Extension->Queue.CryptoInfo->EncryptedAreaLength.Value == 0)
		{
			Extension->Queue.EncryptedAreaStart = -1;
			Extension->Queue.EncryptedAreaEnd = -1;
		}

		Dump ("Loaded: ConfiguredEncryptedAreaStart=%I64d (%I64d)  ConfiguredEncryptedAreaEnd=%I64d (%I64d)\n", Extension->ConfiguredEncryptedAreaStart / 1024 / 1024, Extension->ConfiguredEncryptedAreaStart, Extension->ConfiguredEncryptedAreaEnd / 1024 / 1024, Extension->ConfiguredEncryptedAreaEnd);
		Dump ("Loaded: EncryptedAreaStart=%I64d (%I64d)  EncryptedAreaEnd=%I64d (%I64d)\n", Extension->Queue.EncryptedAreaStart / 1024 / 1024, Extension->Queue.EncryptedAreaStart, Extension->Queue.EncryptedAreaEnd / 1024 / 1024, Extension->Queue.EncryptedAreaEnd);

		// Erase boot loader scheduled keys
		if (BootArgs.CryptoInfoLength > 0)
		{
			PHYSICAL_ADDRESS cryptoInfoAddress;
			byte *mappedCryptoInfo;
			
			cryptoInfoAddress.QuadPart = (BootLoaderSegment << 4) + BootArgs.CryptoInfoOffset;
			mappedCryptoInfo = MmMapIoSpace (cryptoInfoAddress, BootArgs.CryptoInfoLength, MmCached);
			
			if (mappedCryptoInfo)
			{
				Dump ("Wiping memory %x %d\n", cryptoInfoAddress.LowPart, BootArgs.CryptoInfoLength);
				memset (mappedCryptoInfo, 0, BootArgs.CryptoInfoLength);
				MmUnmapIoSpace (mappedCryptoInfo, BootArgs.CryptoInfoLength);
			}
		}

		BootDriveFilterExtension = Extension;
		BootDriveFound = Extension->BootDrive = Extension->DriveMounted = Extension->VolumeHeaderPresent = TRUE;
		BootDriveFilterExtension->MagicNumber = GST_BOOT_DRIVE_FILTER_EXTENSION_MAGIC_NUMBER;

		burn (&BootArgs.BootPassword, sizeof (BootArgs.BootPassword));

		{
			STORAGE_DEVICE_NUMBER storageDeviceNumber;
			status = SendDeviceIoControlRequest (Extension->LowerDeviceObject, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &storageDeviceNumber, sizeof (storageDeviceNumber));

			if (!NT_SUCCESS (status))
			{
				Dump ("Failed to get drive number - error %x\n", status);
				Extension->SystemStorageDeviceNumberValid = FALSE;
			}
			else
			{
				Extension->SystemStorageDeviceNumber = storageDeviceNumber.DeviceNumber;
				Extension->SystemStorageDeviceNumberValid = TRUE;
			}
		}

		status = SendDeviceIoControlRequest (Extension->LowerDeviceObject, IOCTL_DISK_GET_LENGTH_INFO, NULL, 0, &BootDriveLength, sizeof (BootDriveLength));
		
		if (!NT_SUCCESS (status))
		{
			Dump ("Failed to get drive length - error %x\n", status);
			BootDriveLength.QuadPart = 0;
			Extension->Queue.MaxReadAheadOffset.QuadPart = 0;
		}
		else
			Extension->Queue.MaxReadAheadOffset = BootDriveLength;
		
		status = EncryptedIoQueueStart (&Extension->Queue);
		if (!NT_SUCCESS (status))
			GST_BUG_CHECK (status);

		if (IsOSAtLeast (WIN_VISTA))
		{
			CrashDumpEnabled = TRUE;
			HibernationEnabled = TRUE;
		}
		else if (!LegacyHibernationDriverFilterActive)
			StartLegacyHibernationDriverFilter();

		// Hidden system hibernation is not supported if an extra boot partition is present as the system is not allowed to update the boot partition
		if (IsHiddenSystemRunning() && (BootArgs.Flags & GST_BOOT_ARGS_FLAG_EXTRA_BOOT_PARTITION))
		{
			CrashDumpEnabled = FALSE;
			HibernationEnabled = FALSE;
		}
	}
	else
	{
		Dump ("Header not decrypted\n");
		crypto_close (Extension->HeaderCryptoInfo);
		Extension->HeaderCryptoInfo = NULL;

		status = STATUS_UNSUCCESSFUL;
	}

ret:
	GSTfree (header);
	return status;
}


/**
 *
 *	\fn static NTSTATUS SaveDriveVolumeHeader (DriveFilterExtension *Extension)
 *	\brief Save the header of the drive, return the status in case of error. This step is essential for recovering the drive
 *	\param[in] DriveFilterExtension * Extension
 *	\return static NTSTATUS status
 *
 */
static NTSTATUS SaveDriveVolumeHeader (DriveFilterExtension *Extension)
{
	NTSTATUS status = STATUS_SUCCESS;
	LARGE_INTEGER offset;
	byte *header;

	header = GSTalloc (GST_BOOT_ENCRYPTION_VOLUME_HEADER_SIZE);
	if (!header)
		return STATUS_INSUFFICIENT_RESOURCES;

	offset.QuadPart = GST_BOOT_VOLUME_HEADER_SECTOR_OFFSET;

	status = GSTReadDevice (Extension->LowerDeviceObject, header, offset, GST_BOOT_ENCRYPTION_VOLUME_HEADER_SIZE);
	if (!NT_SUCCESS (status))
	{
		Dump ("GSTReadDevice error %x", status);
		goto ret;
	}

	Dump ("Saving: ConfiguredEncryptedAreaStart=%I64d (%I64d)  ConfiguredEncryptedAreaEnd=%I64d (%I64d)\n", Extension->ConfiguredEncryptedAreaStart / 1024 / 1024, Extension->ConfiguredEncryptedAreaStart, Extension->ConfiguredEncryptedAreaEnd / 1024 / 1024, Extension->ConfiguredEncryptedAreaEnd);
	Dump ("Saving: EncryptedAreaStart=%I64d (%I64d)  EncryptedAreaEnd=%I64d (%I64d)\n", Extension->Queue.EncryptedAreaStart / 1024 / 1024, Extension->Queue.EncryptedAreaStart, Extension->Queue.EncryptedAreaEnd / 1024 / 1024, Extension->Queue.EncryptedAreaEnd);
	
	if (Extension->Queue.EncryptedAreaStart == -1 || Extension->Queue.EncryptedAreaEnd == -1
		|| Extension->Queue.EncryptedAreaEnd <= Extension->Queue.EncryptedAreaStart)
	{
		if (SetupRequest.SetupMode == SetupDecryption)
		{
			memset (header, 0, GST_BOOT_ENCRYPTION_VOLUME_HEADER_SIZE);
			Extension->VolumeHeaderPresent = FALSE;
		}
	}
	else
	{
		uint32 headerCrc32;
		uint64 encryptedAreaLength = Extension->Queue.EncryptedAreaEnd + 1 - Extension->Queue.EncryptedAreaStart;
		byte *fieldPos = header + GST_HEADER_OFFSET_ENCRYPTED_AREA_LENGTH;

		DecryptBuffer (header + HEADER_ENCRYPTED_DATA_OFFSET, HEADER_ENCRYPTED_DATA_SIZE, Extension->HeaderCryptoInfo);

		if (GetHeaderField32 (header, GST_HEADER_OFFSET_MAGIC) != 0x54525545)
		{
			Dump ("Header not decrypted");
			status = STATUS_UNKNOWN_REVISION;
			goto ret;
		}

		mputInt64 (fieldPos, encryptedAreaLength);

		headerCrc32 = GetCrc32 (header + GST_HEADER_OFFSET_MAGIC, GST_HEADER_OFFSET_HEADER_CRC - GST_HEADER_OFFSET_MAGIC);
		fieldPos = header + GST_HEADER_OFFSET_HEADER_CRC;
		mputLong (fieldPos, headerCrc32);

		EncryptBuffer (header + HEADER_ENCRYPTED_DATA_OFFSET, HEADER_ENCRYPTED_DATA_SIZE, Extension->HeaderCryptoInfo);
	}

	status = GSTWriteDevice (Extension->LowerDeviceObject, header, offset, GST_BOOT_ENCRYPTION_VOLUME_HEADER_SIZE);
	if (!NT_SUCCESS (status))
	{
		Dump ("GSTWriteDevice error %x", status);
		goto ret;
	}

ret:
	GSTfree (header);
	return status;
}


/**
 *
 *	\fn static NTSTATUS PassIrp (PDEVICE_OBJECT deviceObject, PIRP irp)
 *	\brief PIRP is a request packet sent to the driver. skip the current irp and return the call to the driver passed in argument
 *	\param[in] PIRP irp
 *	\param[in] PDEVICE_OBJECT deviceObject
 *	\return static NTSTATUS IoCallDriver (deviceObject, irp)
 *
 */
static NTSTATUS PassIrp (PDEVICE_OBJECT deviceObject, PIRP irp)
{
	IoSkipCurrentIrpStackLocation (irp);
	return IoCallDriver (deviceObject, irp);
}


/**
 *
 *	\fn static NTSTATUS PassFilteredIrp (PDEVICE_OBJECT deviceObject, PIRP irp, PIO_COMPLETION_ROUTINE completionRoutine, PVOID completionRoutineArg)
 *	\brief Send a call (and return it) to the driver containing an IRP request with a completion routine
 *	\param[in] PVOID completionRoutineArg
 *	\param[in] PIO_COMPLETION_ROUTINE completionRoutine
 *	\param[in] PIRP irp
 *	\param[in] PDEVICE_OBJECT deviceObject
 *	\return static NTSTATUS IoCallDriver (deviceObject, irp)
 *
 */
static NTSTATUS PassFilteredIrp (PDEVICE_OBJECT deviceObject, PIRP irp, PIO_COMPLETION_ROUTINE completionRoutine, PVOID completionRoutineArg)
{
	IoCopyCurrentIrpStackLocationToNext (irp);

	if (completionRoutine)
		IoSetCompletionRoutine (irp, completionRoutine, completionRoutineArg, TRUE, TRUE, TRUE);

	return IoCallDriver (deviceObject, irp);
}


/**
 *
 *	\fn static NTSTATUS OnDeviceUsageNotificationCompleted (PDEVICE_OBJECT filterDeviceObject, PIRP Irp, DriveFilterExtension *Extension)
 *	\brief Add to the queue of the driver to remove the lock, return the status of the completion.
 *	\param[in] DriveFilterExtension * Extension
 *	\param[in] PIRP Irp
 *	\param[in] PDEVICE_OBJECT filterDeviceObject
 *	\return static NTSTATUS STATUS_CONTINUE_COMPLETION
 *
 */
static NTSTATUS OnDeviceUsageNotificationCompleted (PDEVICE_OBJECT filterDeviceObject, PIRP Irp, DriveFilterExtension *Extension)
{
	if (Irp->PendingReturned)
		IoMarkIrpPending (Irp);

	if (!(Extension->LowerDeviceObject->Flags & DO_POWER_PAGABLE))
		filterDeviceObject->Flags &= ~DO_POWER_PAGABLE;

	IoReleaseRemoveLock (&Extension->Queue.RemoveLock, Irp);
	return STATUS_CONTINUE_COMPLETION;
}


/**
 *
 *	\fn static BOOL IsVolumeDevice (PDEVICE_OBJECT deviceObject)
 *	\brief Return True if the Volume is a device, else otherwise.
 *	\param[in] PDEVICE_OBJECT deviceObject
 *	\return static BOOL NT_SUCCESS (SendDeviceIoControlRequest (deviceObject, IOCTL_VOLUME_SUPPORTS_ONLINE_OFFLINE, NULL, 0,  NULL, 0))
 *		|| NT_SUCCESS (SendDeviceIoControlRequest (deviceObject, IOCTL_VOLUME_IS_OFFLINE, NULL, 0,  NULL, 0))
 *		|| NT_SUCCESS (SendDeviceIoControlRequest (deviceObject, IOCTL_VOLUME_IS_IO_CAPABLE, NULL, 0,  NULL, 0))
 *		|| NT_SUCCESS (SendDeviceIoControlRequest (deviceObject, IOCTL_VOLUME_IS_PARTITION, NULL, 0,  NULL, 0))
 *		|| NT_SUCCESS (SendDeviceIoControlRequest (deviceObject, IOCTL_VOLUME_QUERY_VOLUME_NUMBER, NULL, 0, &volNumber, sizeof (volNumber)))
 *		|| NT_SUCCESS (extentStatus) || extentStatus == STATUS_BUFFER_OVERFLOW || extentStatus == STATUS_BUFFER_TOO_SMALL
 *
 */
static BOOL IsVolumeDevice (PDEVICE_OBJECT deviceObject)
{
	VOLUME_NUMBER volNumber;
	VOLUME_DISK_EXTENTS extents[2];
	NTSTATUS extentStatus = SendDeviceIoControlRequest (deviceObject, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, extents, sizeof (extents));

	return NT_SUCCESS (SendDeviceIoControlRequest (deviceObject, IOCTL_VOLUME_SUPPORTS_ONLINE_OFFLINE, NULL, 0,  NULL, 0))
		|| NT_SUCCESS (SendDeviceIoControlRequest (deviceObject, IOCTL_VOLUME_IS_OFFLINE, NULL, 0,  NULL, 0))
		|| NT_SUCCESS (SendDeviceIoControlRequest (deviceObject, IOCTL_VOLUME_IS_IO_CAPABLE, NULL, 0,  NULL, 0))
		|| NT_SUCCESS (SendDeviceIoControlRequest (deviceObject, IOCTL_VOLUME_IS_PARTITION, NULL, 0,  NULL, 0))
		|| NT_SUCCESS (SendDeviceIoControlRequest (deviceObject, IOCTL_VOLUME_QUERY_VOLUME_NUMBER, NULL, 0, &volNumber, sizeof (volNumber)))
		|| NT_SUCCESS (extentStatus) || extentStatus == STATUS_BUFFER_OVERFLOW || extentStatus == STATUS_BUFFER_TOO_SMALL;
}


/**
 *
 *	\fn static void CheckDeviceTypeAndMount (DriveFilterExtension *filterExtension)
 *	\brief Check the device Type and Mount it. Windows sometimes merges a removable drive PDO and its volume PDO to a single PDO having no volume interface (GUID_DEVINTERFACE_VOLUME).
 *			Therefore, we need to test whether the device supports volume IOCTLs.
 *	\param[in] DriveFilterExtension * filterExtension
 *	\return static void
 *
 */
static void CheckDeviceTypeAndMount (DriveFilterExtension *filterExtension)
{
	if (BootArgsValid)
	{
		// Windows sometimes merges a removable drive PDO and its volume PDO to a single PDO having no volume interface (GUID_DEVINTERFACE_VOLUME).
		// Therefore, we need to test whether the device supports volume IOCTLs.
		if (VolumeClassFilterRegistered
			&& BootArgs.HiddenSystemPartitionStart != 0
			&& IsVolumeDevice (filterExtension->LowerDeviceObject))
		{
			Dump ("Drive and volume merged pdo=%p", filterExtension->Pdo);

			filterExtension->IsVolumeFilterDevice = TRUE;
			filterExtension->IsDriveFilterDevice = FALSE;
		}
		else
		{
			NTSTATUS status = KeWaitForMutexObject (&MountMutex, Executive, KernelMode, FALSE, NULL);
			if (!NT_SUCCESS (status))
				GST_BUG_CHECK (status);

			if (!BootDriveFound)
				MountDrive (filterExtension, &BootArgs.BootPassword, &BootArgs.HeaderSaltCrc32);

			KeReleaseMutex (&MountMutex, FALSE);
		}
	}
}


/**
 *
 *	\fn static VOID MountDriveWorkItemRoutine (PDEVICE_OBJECT deviceObject, DriveFilterExtension *filterExtension)
 *	\brief Mount routine, check, mount and set event.
 *	\param[in] DriveFilterExtension * filterExtension
 *	\param[in] PDEVICE_OBJECT deviceObject
 *	\return static VOID
 *
 */
static VOID MountDriveWorkItemRoutine (PDEVICE_OBJECT deviceObject, DriveFilterExtension *filterExtension)
{
	CheckDeviceTypeAndMount (filterExtension);
	KeSetEvent (&filterExtension->MountWorkItemCompletedEvent, IO_NO_INCREMENT, FALSE);
}


/**
 *
 *	\fn static NTSTATUS OnStartDeviceCompleted (PDEVICE_OBJECT filterDeviceObject, PIRP Irp, DriveFilterExtension *Extension)
 *	\brief Return and unlock when the driver is finished to start himself.
 *	\param[in] DriveFilterExtension * Extension
 *	\param[in] PIRP Irp
 *	\param[in] PDEVICE_OBJECT filterDeviceObject
 *	\return static NTSTATUS STATUS_CONTINUE_COMPLETION
 *
 */
static NTSTATUS OnStartDeviceCompleted (PDEVICE_OBJECT filterDeviceObject, PIRP Irp, DriveFilterExtension *Extension)
{
	if (Irp->PendingReturned)
		IoMarkIrpPending (Irp);

	if (Extension->LowerDeviceObject->Characteristics & FILE_REMOVABLE_MEDIA)
		filterDeviceObject->Characteristics |= FILE_REMOVABLE_MEDIA;

	if (KeGetCurrentIrql() == PASSIVE_LEVEL)
	{
		CheckDeviceTypeAndMount (Extension);
	}
	else
	{
		PIO_WORKITEM workItem = IoAllocateWorkItem (filterDeviceObject);
		if (!workItem)
		{
			IoReleaseRemoveLock (&Extension->Queue.RemoveLock, Irp);
			return STATUS_INSUFFICIENT_RESOURCES;
		}

		KeInitializeEvent (&Extension->MountWorkItemCompletedEvent, SynchronizationEvent, FALSE);
		IoQueueWorkItem (workItem, MountDriveWorkItemRoutine, DelayedWorkQueue, Extension); 

		KeWaitForSingleObject (&Extension->MountWorkItemCompletedEvent, Executive, KernelMode, FALSE, NULL);
		IoFreeWorkItem (workItem);
	}

	IoReleaseRemoveLock (&Extension->Queue.RemoveLock, Irp);
	return STATUS_CONTINUE_COMPLETION;
}


/**
 *
 *	\fn static NTSTATUS DispatchPnp (PDEVICE_OBJECT DeviceObject, PIRP Irp, DriveFilterExtension *Extension, PIO_STACK_LOCATION irpSp)
 *	\brief Check the IRP value to perform action according to it before sending it to the drive.
 *	\param[in] PIO_STACK_LOCATION irpSp
 *	\param[in] DriveFilterExtension * Extension
 *	\param[in] PIRP Irp
 *	\param[in] PDEVICE_OBJECT DeviceObject
 *	\return static NTSTATUS status
 *
 */
static NTSTATUS DispatchPnp (PDEVICE_OBJECT DeviceObject, PIRP Irp, DriveFilterExtension *Extension, PIO_STACK_LOCATION irpSp)
{
	NTSTATUS status;

	status = IoAcquireRemoveLock (&Extension->Queue.RemoveLock, Irp);
	if (!NT_SUCCESS (status))
		return GSTCompleteIrp (Irp, status, 0);

	switch (irpSp->MinorFunction)
	{
	case IRP_MN_START_DEVICE:
		Dump ("IRP_MN_START_DEVICE pdo=%p\n", Extension->Pdo);
		return PassFilteredIrp (Extension->LowerDeviceObject, Irp, OnStartDeviceCompleted, Extension);


	case IRP_MN_DEVICE_USAGE_NOTIFICATION:
		Dump ("IRP_MN_DEVICE_USAGE_NOTIFICATION type=%d\n", (int) irpSp->Parameters.UsageNotification.Type);

		{
			PDEVICE_OBJECT attachedDevice = IoGetAttachedDeviceReference (DeviceObject);

			if (attachedDevice == DeviceObject || (attachedDevice->Flags & DO_POWER_PAGABLE))
				DeviceObject->Flags |= DO_POWER_PAGABLE;

			ObDereferenceObject (attachedDevice);
		}

		// Prevent creation of hibernation and crash dump files if required
		if (irpSp->Parameters.UsageNotification.InPath
			&& (
				(irpSp->Parameters.UsageNotification.Type == DeviceUsageTypeDumpFile && !CrashDumpEnabled)
				|| (irpSp->Parameters.UsageNotification.Type == DeviceUsageTypeHibernation && !HibernationEnabled)
				)
			)
		{
			IoReleaseRemoveLock (&Extension->Queue.RemoveLock, Irp);

			if (irpSp->Parameters.UsageNotification.Type == DeviceUsageTypeHibernation)
				++HibernationPreventionCount;

			Dump ("Preventing dump type=%d\n", (int) irpSp->Parameters.UsageNotification.Type);
			return GSTCompleteIrp (Irp, STATUS_UNSUCCESSFUL, 0);
		}

		return PassFilteredIrp (Extension->LowerDeviceObject, Irp, OnDeviceUsageNotificationCompleted, Extension);


	case IRP_MN_REMOVE_DEVICE:
		Dump ("IRP_MN_REMOVE_DEVICE pdo=%p\n", Extension->Pdo);

		IoReleaseRemoveLockAndWait (&Extension->Queue.RemoveLock, Irp);
		status = PassIrp (Extension->LowerDeviceObject, Irp);

		IoDetachDevice (Extension->LowerDeviceObject);

		if (Extension->DriveMounted)
			DismountDrive (Extension, TRUE);

		if (Extension->BootDrive)
		{
			BootDriveFound = FALSE;
			BootDriveFilterExtension = NULL;
		}

		IoDeleteDevice (DeviceObject);
		return status;


	default:
		status = PassIrp (Extension->LowerDeviceObject, Irp);
		IoReleaseRemoveLock (&Extension->Queue.RemoveLock, Irp);
	}
	return status;
}


/**
 *
 *	\fn static NTSTATUS DispatchPower (PDEVICE_OBJECT DeviceObject, PIRP Irp, DriveFilterExtension *Extension, PIO_STACK_LOCATION irpSp)
 *	\brief Manage the power for the devices.
 *	\param[in] PIO_STACK_LOCATION irpSp
 *	\param[in] DriveFilterExtension * Extension
 *	\param[in] PIRP Irp
 *	\param[in] PDEVICE_OBJECT DeviceObject
 *	\return static NTSTATUS status
 *
 */
static NTSTATUS DispatchPower (PDEVICE_OBJECT DeviceObject, PIRP Irp, DriveFilterExtension *Extension, PIO_STACK_LOCATION irpSp)
{
	NTSTATUS status;
	Dump ("IRP_MJ_POWER minor=%d type=%d shutdown=%d\n", (int) irpSp->MinorFunction, (int) irpSp->Parameters.Power.Type, (int) irpSp->Parameters.Power.ShutdownType);

	if (SetupInProgress
		&& irpSp->MinorFunction == IRP_MN_SET_POWER
		&& irpSp->Parameters.Power.ShutdownType == PowerActionHibernate)
	{
		while (SendDeviceIoControlRequest (RootDeviceObject, GST_IOCTL_ABORT_BOOT_ENCRYPTION_SETUP, NULL, 0, NULL, 0) == STATUS_INSUFFICIENT_RESOURCES);
	}

#if 0	// Dismount of the system drive is disabled until there is a way to do it without causing system errors (see the documentation for more info)
	if (DriverShuttingDown
		&& Extension->BootDrive
		&& Extension->DriveMounted
		&& irpSp->MinorFunction == IRP_MN_SET_POWER
		&& irpSp->Parameters.Power.Type == DevicePowerState)
	{
		DismountDrive (Extension, TRUE);
	}
#endif // 0

	PoStartNextPowerIrp (Irp);

	status = IoAcquireRemoveLock (&Extension->Queue.RemoveLock, Irp);
	if (!NT_SUCCESS (status))
		return GSTCompleteIrp (Irp, status, 0);

	IoSkipCurrentIrpStackLocation (Irp);
	status = PoCallDriver (Extension->LowerDeviceObject, Irp);

	IoReleaseRemoveLock (&Extension->Queue.RemoveLock, Irp);
	return status;
}


/**
 *
 *	\fn NTSTATUS DriveFilterDispatchIrp (PDEVICE_OBJECT DeviceObject, PIRP Irp)
 *	\brief Read the irp major function and use the right function to manage the type of irp sent.
 *	\param[in] PIRP Irp
 *	\param[in] PDEVICE_OBJECT DeviceObject
 *	\return NTSTATUS status
 *
 */
NTSTATUS DriveFilterDispatchIrp (PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	DriveFilterExtension *Extension = (DriveFilterExtension *) DeviceObject->DeviceExtension;
	PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation (Irp);
	NTSTATUS status;

	ASSERT (!Extension->bRootDevice && Extension->IsDriveFilterDevice);

	switch (irpSp->MajorFunction)
	{
	case IRP_MJ_READ:
	case IRP_MJ_WRITE:
		if (Extension->BootDrive)
		{
			status = EncryptedIoQueueAddIrp (&Extension->Queue, Irp);
			
			if (status != STATUS_PENDING)
				GSTCompleteDiskIrp (Irp, status, 0);

			return status;
		}
		break;

	case IRP_MJ_PNP:
		return DispatchPnp (DeviceObject, Irp, Extension, irpSp);

	case IRP_MJ_POWER:
		return DispatchPower (DeviceObject, Irp, Extension, irpSp);
	}

	status = IoAcquireRemoveLock (&Extension->Queue.RemoveLock, Irp);
	if (!NT_SUCCESS (status))
		return GSTCompleteIrp (Irp, status, 0);

	status = PassIrp (Extension->LowerDeviceObject, Irp);

	IoReleaseRemoveLock (&Extension->Queue.RemoveLock, Irp);
	return status;
}


/**
 *
 *	\fn void ReopenBootVolumeHeader (PIRP irp, PIO_STACK_LOCATION irpSp)
 *	\brief Send a request to open the boot volume header and read if accepted
 *	\param[in] PIO_STACK_LOCATION irpSp
 *	\param[in] PIRP irp
 *	\return void
 *
 */
void ReopenBootVolumeHeader (PIRP irp, PIO_STACK_LOCATION irpSp)
{
	LARGE_INTEGER offset;
	char *header;
	ReopenBootVolumeHeaderRequest *request = (ReopenBootVolumeHeaderRequest *) irp->AssociatedIrp.SystemBuffer;

	irp->IoStatus.Information = 0;

	if (!IoIsSystemThread (PsGetCurrentThread()) && !UserCanAccessDriveDevice())
	{
		irp->IoStatus.Status = STATUS_ACCESS_DENIED;
		return;
	}

	if (!ValidateIOBufferSize (irp, sizeof (ReopenBootVolumeHeaderRequest), ValidateInput))
		return;

	if (!BootDriveFound || !BootDriveFilterExtension || !BootDriveFilterExtension->DriveMounted || !BootDriveFilterExtension->HeaderCryptoInfo
		|| request->VolumePassword.Length > MAX_PASSWORD)
	{
		irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
		goto wipe;
	}

	header = GSTalloc (GST_BOOT_ENCRYPTION_VOLUME_HEADER_SIZE);
	if (!header)
	{
		irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
		goto wipe;
	}

	if (BootDriveFilterExtension->HiddenSystem)
		offset.QuadPart = BootArgs.HiddenSystemPartitionStart + GST_HIDDEN_VOLUME_HEADER_OFFSET;
	else
		offset.QuadPart = GST_BOOT_VOLUME_HEADER_SECTOR_OFFSET;

	irp->IoStatus.Status = GSTReadDevice (BootDriveFilterExtension->LowerDeviceObject, header, offset, GST_BOOT_ENCRYPTION_VOLUME_HEADER_SIZE);
	if (!NT_SUCCESS (irp->IoStatus.Status))
	{
		Dump ("GSTReadDevice error %x\n", irp->IoStatus.Status);
		goto ret;
	}

	if (ReadVolumeHeader (!BootDriveFilterExtension->HiddenSystem, header, &request->VolumePassword, NULL, BootDriveFilterExtension->HeaderCryptoInfo) == 0)
	{
		Dump ("Header reopened\n");
		
		BootDriveFilterExtension->Queue.CryptoInfo->header_creation_time = BootDriveFilterExtension->HeaderCryptoInfo->header_creation_time;
		BootDriveFilterExtension->Queue.CryptoInfo->pkcs5 = BootDriveFilterExtension->HeaderCryptoInfo->pkcs5;
		BootDriveFilterExtension->Queue.CryptoInfo->noIterations = BootDriveFilterExtension->HeaderCryptoInfo->noIterations;

		irp->IoStatus.Status = STATUS_SUCCESS;
	}
	else
	{
		crypto_close (BootDriveFilterExtension->HeaderCryptoInfo);
		BootDriveFilterExtension->HeaderCryptoInfo = NULL;

		Dump ("Header not reopened\n");
		irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
	}

ret:
	GSTfree (header);
wipe:
	burn (request, sizeof (*request));
}


// Legacy Windows XP/2003 hibernation dump filter

typedef NTSTATUS (*HiberDriverWriteFunctionA) (ULONG arg0, PLARGE_INTEGER writeOffset, PMDL dataMdl, PVOID arg3);
typedef NTSTATUS (*HiberDriverWriteFunctionB) (PLARGE_INTEGER writeOffset, PMDL dataMdl);

typedef struct
{
#ifdef _WIN64
	byte FieldPad1[64];
	HiberDriverWriteFunctionB WriteFunctionB;
	byte FieldPad2[56];
#else
	byte FieldPad1[48];
	HiberDriverWriteFunctionB WriteFunctionB;
	byte FieldPad2[32];
#endif
	HiberDriverWriteFunctionA WriteFunctionA;
	byte FieldPad3[24];
	LARGE_INTEGER PartitionStartOffset;
} HiberDriverContext;

typedef NTSTATUS (*HiberDriverEntry) (PVOID arg0, HiberDriverContext *hiberDriverContext);

typedef struct
{
	LIST_ENTRY ModuleList;
#ifdef _WIN64
	byte FieldPad1[32];
#else
	byte FieldPad1[16];
#endif
	PVOID ModuleBaseAddress;
	HiberDriverEntry ModuleEntryAddress;
#ifdef _WIN64
	byte FieldPad2[24];
#else
	byte FieldPad2[12];
#endif
	UNICODE_STRING ModuleName;
} ModuleTableItem;


#define GST_MAX_HIBER_FILTER_COUNT 3
static int LastHiberFilterNumber = 0;

static HiberDriverEntry OriginalHiberDriverEntries[GST_MAX_HIBER_FILTER_COUNT];
static HiberDriverWriteFunctionA OriginalHiberDriverWriteFunctionsA[GST_MAX_HIBER_FILTER_COUNT];
static HiberDriverWriteFunctionB OriginalHiberDriverWriteFunctionsB[GST_MAX_HIBER_FILTER_COUNT];

static LARGE_INTEGER HiberPartitionOffset;


/**
 *
 *	\fn static NTSTATUS HiberDriverWriteFunctionFilter (int filterNumber, PLARGE_INTEGER writeOffset, PMDL dataMdl, BOOL writeB, ULONG arg0WriteA, PVOID arg3WriteA)
 *	\brief Hibernate the driver write function filter number filterNumber passed in argument.
 *	\param[in] PVOID arg3WriteA
 *	\param[in] ULONG arg0WriteA
 *	\param[in] BOOL writeB
 *	\param[in] PMDL dataMdl
 *	\param[in] PLARGE_INTEGER writeOffset
 *	\param[in] int filterNumber
 *	\return static NTSTATUS (*OriginalHiberDriverWriteFunctionsA[filterNumber]) (arg0WriteA, writeOffset, encryptedDataMdl, arg3WriteA)
 *
 */
static NTSTATUS HiberDriverWriteFunctionFilter (int filterNumber, PLARGE_INTEGER writeOffset, PMDL dataMdl, BOOL writeB, ULONG arg0WriteA, PVOID arg3WriteA)
{
	MDL *encryptedDataMdl = dataMdl;

	if (writeOffset && dataMdl && BootDriveFilterExtension && BootDriveFilterExtension->DriveMounted)
	{
		ULONG dataLength = MmGetMdlByteCount (dataMdl);

		if (dataMdl->MappedSystemVa && dataLength > 0)
		{
			uint64 offset = HiberPartitionOffset.QuadPart + writeOffset->QuadPart;
			uint64 intersectStart;
			uint32 intersectLength;

			if (dataLength > GST_HIBERNATION_WRITE_BUFFER_SIZE)
				GST_BUG_CHECK (STATUS_BUFFER_OVERFLOW);

			if ((dataLength & (ENCRYPTION_DATA_UNIT_SIZE - 1)) != 0)
				GST_BUG_CHECK (STATUS_INVALID_PARAMETER);

			if ((offset & (ENCRYPTION_DATA_UNIT_SIZE - 1)) != 0)
				GST_BUG_CHECK (STATUS_INVALID_PARAMETER);

			GetIntersection (offset,
				dataLength,
				BootDriveFilterExtension->Queue.EncryptedAreaStart,
				BootDriveFilterExtension->Queue.EncryptedAreaEnd,
				&intersectStart,
				&intersectLength);

			if (intersectLength > 0)
			{
				UINT64_STRUCT dataUnit;
				dataUnit.Value = intersectStart / ENCRYPTION_DATA_UNIT_SIZE;

				memcpy (HibernationWriteBuffer, dataMdl->MappedSystemVa, dataLength);

				if (BootDriveFilterExtension->Queue.RemapEncryptedArea)
					dataUnit.Value += BootDriveFilterExtension->Queue.RemappedAreaDataUnitOffset;

				EncryptDataUnitsCurrentThread (HibernationWriteBuffer + (intersectStart - offset),
					&dataUnit,
					intersectLength / ENCRYPTION_DATA_UNIT_SIZE,
					BootDriveFilterExtension->Queue.CryptoInfo);

				encryptedDataMdl = HibernationWriteBufferMdl;
				MmInitializeMdl (encryptedDataMdl, HibernationWriteBuffer, dataLength);
				encryptedDataMdl->MdlFlags = dataMdl->MdlFlags;
			}
		}
	}

	if (writeB)
		return (*OriginalHiberDriverWriteFunctionsB[filterNumber]) (writeOffset, encryptedDataMdl);
	
	return (*OriginalHiberDriverWriteFunctionsA[filterNumber]) (arg0WriteA, writeOffset, encryptedDataMdl, arg3WriteA);
}


/**
 *
 *	\fn static NTSTATUS HiberDriverWriteFunctionAFilter0 (ULONG arg0, PLARGE_INTEGER writeOffset, PMDL dataMdl, PVOID arg3)
 *	\brief H>bernate the filter function 0
 *	\param[in] PVOID arg3
 *	\param[in] PMDL dataMdl
 *	\param[in] PLARGE_INTEGER writeOffset
 *	\param[in] ULONG arg0
 *	\return static NTSTATUS HiberDriverWriteFunctionFilter (0, writeOffset, dataMdl, FALSE, arg0, arg3)
 *
 */
static NTSTATUS HiberDriverWriteFunctionAFilter0 (ULONG arg0, PLARGE_INTEGER writeOffset, PMDL dataMdl, PVOID arg3)
{
	return HiberDriverWriteFunctionFilter (0, writeOffset, dataMdl, FALSE, arg0, arg3);
}

/**
 *
 *	\fn static NTSTATUS HiberDriverWriteFunctionAFilter1 (ULONG arg0, PLARGE_INTEGER writeOffset, PMDL dataMdl, PVOID arg3)
 *	\brief Hibernate the filter function 1
 *	\param[in] PVOID arg3
 *	\param[in] PMDL dataMdl
 *	\param[in] PLARGE_INTEGER writeOffset
 *	\param[in] ULONG arg0
 *	\return static NTSTATUS HiberDriverWriteFunctionFilter (1, writeOffset, dataMdl, FALSE, arg0, arg3)
 *
 */
static NTSTATUS HiberDriverWriteFunctionAFilter1 (ULONG arg0, PLARGE_INTEGER writeOffset, PMDL dataMdl, PVOID arg3)
{
	return HiberDriverWriteFunctionFilter (1, writeOffset, dataMdl, FALSE, arg0, arg3);
}

/**
 *
 *	\fn static NTSTATUS HiberDriverWriteFunctionAFilter2 (ULONG arg0, PLARGE_INTEGER writeOffset, PMDL dataMdl, PVOID arg3)
 *	\brief Hibernate the function filter 2
 *	\param[in] PVOID arg3
 *	\param[in] PMDL dataMdl
 *	\param[in] PLARGE_INTEGER writeOffset
 *	\param[in] ULONG arg0
 *	\return static NTSTATUS HiberDriverWriteFunctionFilter (2, writeOffset, dataMdl, FALSE, arg0, arg3)
 *
 */
static NTSTATUS HiberDriverWriteFunctionAFilter2 (ULONG arg0, PLARGE_INTEGER writeOffset, PMDL dataMdl, PVOID arg3)
{
	return HiberDriverWriteFunctionFilter (2, writeOffset, dataMdl, FALSE, arg0, arg3);
}


/**
 *
 *	\fn static NTSTATUS HiberDriverWriteFunctionBFilter0 (PLARGE_INTEGER writeOffset, PMDL dataMdl)
 *	\brief Hibernate the function B 0
 *	\param[in] PMDL dataMdl
 *	\param[in] PLARGE_INTEGER writeOffset
 *	\return static NTSTATUS HiberDriverWriteFunctionFilter (0, writeOffset, dataMdl, TRUE, 0, NULL)
 *
 */
static NTSTATUS HiberDriverWriteFunctionBFilter0 (PLARGE_INTEGER writeOffset, PMDL dataMdl)
{
	return HiberDriverWriteFunctionFilter (0, writeOffset, dataMdl, TRUE, 0, NULL);
}

/**
 *
 *	\fn static NTSTATUS HiberDriverWriteFunctionBFilter1 (PLARGE_INTEGER writeOffset, PMDL dataMdl)
 *	\brief Hibernate the function B 1
 *	\param[in] PMDL dataMdl
 *	\param[in] PLARGE_INTEGER writeOffset
 *	\return static NTSTATUS HiberDriverWriteFunctionFilter (1, writeOffset, dataMdl, TRUE, 0, NULL)
 *
 */
static NTSTATUS HiberDriverWriteFunctionBFilter1 (PLARGE_INTEGER writeOffset, PMDL dataMdl)
{
	return HiberDriverWriteFunctionFilter (1, writeOffset, dataMdl, TRUE, 0, NULL);
}

/**
 *
 *	\fn static NTSTATUS HiberDriverWriteFunctionBFilter2 (PLARGE_INTEGER writeOffset, PMDL dataMdl)
 *	\brief Hibernate the function B 2
 *	\param[in] PMDL dataMdl
 *	\param[in] PLARGE_INTEGER writeOffset
 *	\return static NTSTATUS HiberDriverWriteFunctionFilter (2, writeOffset, dataMdl, TRUE, 0, NULL)
 *
 */
static NTSTATUS HiberDriverWriteFunctionBFilter2 (PLARGE_INTEGER writeOffset, PMDL dataMdl)
{
	return HiberDriverWriteFunctionFilter (2, writeOffset, dataMdl, TRUE, 0, NULL);
}


/**
 *
 *	\fn static NTSTATUS HiberDriverEntryFilter (int filterNumber, PVOID arg0, HiberDriverContext *hiberDriverContext)
 *	\brief Hibernate the Entry filter number filterNumber given in the argument.
 *	\param[in] HiberDriverContext * hiberDriverContext
 *	\param[in] PVOID arg0
 *	\param[in] int filterNumber
 *	\return static NTSTATUS STATUS_SUCCESS if success
 *			STATUS_UNSUCCESSFUL if not
 *	
 */
static NTSTATUS HiberDriverEntryFilter (int filterNumber, PVOID arg0, HiberDriverContext *hiberDriverContext)
{
	BOOL filterInstalled = FALSE;
	NTSTATUS status;

	if (!OriginalHiberDriverEntries[filterNumber])
		return STATUS_UNSUCCESSFUL;

	status = (*OriginalHiberDriverEntries[filterNumber]) (arg0, hiberDriverContext);

	if (!NT_SUCCESS (status) || !hiberDriverContext)
		return status;

	if (SetupInProgress)
		GST_BUG_CHECK (STATUS_INVALID_PARAMETER);

	if (hiberDriverContext->WriteFunctionA)
	{
		Dump ("Filtering WriteFunctionA %d\n", filterNumber);
		OriginalHiberDriverWriteFunctionsA[filterNumber] = hiberDriverContext->WriteFunctionA;

		switch (filterNumber)
		{
		case 0: hiberDriverContext->WriteFunctionA = HiberDriverWriteFunctionAFilter0; break;
		case 1: hiberDriverContext->WriteFunctionA = HiberDriverWriteFunctionAFilter1; break;
		case 2: hiberDriverContext->WriteFunctionA = HiberDriverWriteFunctionAFilter2; break;
		default: GST_THROW_FATAL_EXCEPTION;
		}

		filterInstalled = TRUE;
	}

	if (hiberDriverContext->WriteFunctionB)
	{
		Dump ("Filtering WriteFunctionB %d\n", filterNumber);
		OriginalHiberDriverWriteFunctionsB[filterNumber] = hiberDriverContext->WriteFunctionB;

		switch (filterNumber)
		{
		case 0: hiberDriverContext->WriteFunctionB = HiberDriverWriteFunctionBFilter0; break;
		case 1: hiberDriverContext->WriteFunctionB = HiberDriverWriteFunctionBFilter1; break;
		case 2: hiberDriverContext->WriteFunctionB = HiberDriverWriteFunctionBFilter2; break;
		default: GST_THROW_FATAL_EXCEPTION;
		}

		filterInstalled = TRUE;
	}

	if (filterInstalled && hiberDriverContext->PartitionStartOffset.QuadPart != 0)
	{
		HiberPartitionOffset = hiberDriverContext->PartitionStartOffset;

		if (BootDriveFilterExtension->Queue.RemapEncryptedArea)
			hiberDriverContext->PartitionStartOffset.QuadPart += BootDriveFilterExtension->Queue.RemappedAreaOffset;
	}

	return STATUS_SUCCESS;
}


/**
 *
 *	\fn static NTSTATUS HiberDriverEntryFilter0 (PVOID arg0, HiberDriverContext *hiberDriverContext)
 *	\brief Hibernate the Entry filter number 0.
 *	\param[in] HiberDriverContext * hiberDriverContext
 *	\param[in] PVOID arg0
 *	\return static NTSTATUS HiberDriverEntryFilter (0, arg0, hiberDriverContext)
 *
 */
static NTSTATUS HiberDriverEntryFilter0 (PVOID arg0, HiberDriverContext *hiberDriverContext)
{
	return HiberDriverEntryFilter (0, arg0, hiberDriverContext);
}


/**
 *
 *	\fn static NTSTATUS HiberDriverEntryFilter1 (PVOID arg0, HiberDriverContext *hiberDriverContext)
 *	\brief Hibernate the Entry filter number 1
 *	\param[in] HiberDriverContext * hiberDriverContext
 *	\param[in] PVOID arg0
 *	\return static NTSTATUS HiberDriverEntryFilter (1, arg0, hiberDriverContext)
 *
 */
static NTSTATUS HiberDriverEntryFilter1 (PVOID arg0, HiberDriverContext *hiberDriverContext)
{
	return HiberDriverEntryFilter (1, arg0, hiberDriverContext);
}


/**
 *
 *	\fn static NTSTATUS HiberDriverEntryFilter2 (PVOID arg0, HiberDriverContext *hiberDriverContext)
 *	\brief Hibernate the Entry filter number 2
 *	\param[in] HiberDriverContext * hiberDriverContext
 *	\param[in] PVOID arg0
 *	\return static NTSTATUS HiberDriverEntryFilter (2, arg0, hiberDriverContext)
 *
 */
static NTSTATUS HiberDriverEntryFilter2 (PVOID arg0, HiberDriverContext *hiberDriverContext)
{
	return HiberDriverEntryFilter (2, arg0, hiberDriverContext);
}


/**
 *
 *	\fn static VOID LoadImageNotifyRoutine (PUNICODE_STRING fullImageName, HANDLE processId, PIMAGE_INFO imageInfo)
 *	\brief FullImageName argument not used. set driverfilter entry in hibernate mode and then install a filter
 *	\param[in] PIMAGE_INFO imageInfo
 *	\param[in] HANDLE processId
 *	\param[in] PUNICODE_STRING fullImageName
 *	\return static VOID
 *
 */
static VOID LoadImageNotifyRoutine (PUNICODE_STRING fullImageName, HANDLE processId, PIMAGE_INFO imageInfo)
{
	ModuleTableItem *moduleItem;
	LIST_ENTRY *listEntry;
	KIRQL origIrql;

	if (!imageInfo || !imageInfo->SystemModeImage || !imageInfo->ImageBase || !GSTDriverObject->DriverSection)
		return;

	moduleItem = *(ModuleTableItem **) GSTDriverObject->DriverSection;
	if (!moduleItem || !moduleItem->ModuleList.Flink)
		return;

	// Search loaded system modules for hibernation driver
	origIrql = KeRaiseIrqlToDpcLevel();

	for (listEntry = moduleItem->ModuleList.Flink->Blink;
		listEntry && listEntry != GSTDriverObject->DriverSection;
		listEntry = listEntry->Flink)
	{
		moduleItem = CONTAINING_RECORD (listEntry, ModuleTableItem, ModuleList);

		if (moduleItem && imageInfo->ImageBase == moduleItem->ModuleBaseAddress)
		{
			if (moduleItem->ModuleName.Buffer && moduleItem->ModuleName.Length >= 5 * sizeof (wchar_t))
			{
				if (memcmp (moduleItem->ModuleName.Buffer, L"hiber", 5 * sizeof (wchar_t)) == 0
					|| memcmp (moduleItem->ModuleName.Buffer, L"Hiber", 5 * sizeof (wchar_t)) == 0
					|| memcmp (moduleItem->ModuleName.Buffer, L"HIBER", 5 * sizeof (wchar_t)) == 0)
				{
					HiberDriverEntry filterEntry;

					switch (LastHiberFilterNumber)
					{
					case 0: filterEntry = HiberDriverEntryFilter0; break;
					case 1: filterEntry = HiberDriverEntryFilter1; break;
					case 2: filterEntry = HiberDriverEntryFilter2; break;
					default: GST_THROW_FATAL_EXCEPTION;
					}

					if (moduleItem->ModuleEntryAddress != filterEntry)
					{
						// Install filter
						OriginalHiberDriverEntries[LastHiberFilterNumber] = moduleItem->ModuleEntryAddress;
						moduleItem->ModuleEntryAddress = filterEntry;

						if (++LastHiberFilterNumber > GST_MAX_HIBER_FILTER_COUNT - 1)
							LastHiberFilterNumber = 0;
					}
				}
			}
			break;
		}
	}

	KeLowerIrql (origIrql);
}


/**
 *
 *	\fn void StartLegacyHibernationDriverFilter ()
 *	\brief Start the hibernation of the driver filter.
 *	\return void
 *
 */
void StartLegacyHibernationDriverFilter ()
{
	PHYSICAL_ADDRESS highestAcceptableWriteBufferAddr;
	NTSTATUS status;

	ASSERT (KeGetCurrentIrql() == PASSIVE_LEVEL);
	ASSERT (!IsOSAtLeast (WIN_VISTA));

	if (!GSTDriverObject->DriverSection || !*(ModuleTableItem **) GSTDriverObject->DriverSection)
		goto err;

	// All buffers required for hibernation must be allocated here
#ifdef _WIN64
	highestAcceptableWriteBufferAddr.QuadPart = 0x7FFffffFFFFULL;
#else
	highestAcceptableWriteBufferAddr.QuadPart = 0xffffFFFFULL;
#endif

	HibernationWriteBuffer = MmAllocateContiguousMemory (GST_HIBERNATION_WRITE_BUFFER_SIZE, highestAcceptableWriteBufferAddr);
	if (!HibernationWriteBuffer)
		goto err;

	HibernationWriteBufferMdl = IoAllocateMdl (HibernationWriteBuffer, GST_HIBERNATION_WRITE_BUFFER_SIZE, FALSE, FALSE, NULL);
	if (!HibernationWriteBufferMdl)
		goto err;

	MmBuildMdlForNonPagedPool (HibernationWriteBufferMdl);

	status = PsSetLoadImageNotifyRoutine (LoadImageNotifyRoutine);
	if (!NT_SUCCESS (status))
		goto err;

	LegacyHibernationDriverFilterActive = TRUE;
	CrashDumpEnabled = FALSE;
	HibernationEnabled = TRUE;
	return;

err:
	LegacyHibernationDriverFilterActive = FALSE;
	CrashDumpEnabled = FALSE;
	HibernationEnabled = FALSE;

	if (HibernationWriteBufferMdl)
	{
		IoFreeMdl (HibernationWriteBufferMdl);
		HibernationWriteBufferMdl = NULL;
	}

	if (HibernationWriteBuffer)
	{
		MmFreeContiguousMemory (HibernationWriteBuffer);
		HibernationWriteBuffer = NULL;
	}
}


/**
 *
 *	\fn static VOID SetupThreadProc (PVOID threadArg)
 *	\brief Start the setup of a volume with a argument. Encrypt, decrypt. Read the bootdrive section and discard sections that cannot be read.
 *			Write in the device then update the header of the drive
 *	\param[in] PVOID threadArg
 *	\return static VOID
 *
 */
static VOID SetupThreadProc (PVOID threadArg)
{
	DriveFilterExtension *Extension = BootDriveFilterExtension;

	LARGE_INTEGER offset;
	UINT64_STRUCT dataUnit;
	ULONG setupBlockSize = GST_ENCRYPTION_SETUP_IO_BLOCK_SIZE;
	BOOL headerUpdateRequired = FALSE;
	int64 bytesWrittenSinceHeaderUpdate = 0;

	byte *buffer = NULL;
	byte *wipeBuffer = NULL;
	byte wipeRandChars[GST_WIPE_RAND_CHAR_COUNT];
	byte wipeRandCharsUpdate[GST_WIPE_RAND_CHAR_COUNT];
	
	KIRQL irql;
	NTSTATUS status;

	SetupResult = STATUS_UNSUCCESSFUL;

	// Make sure volume header can be updated
	if (Extension->HeaderCryptoInfo == NULL)
	{
		SetupResult = STATUS_INVALID_PARAMETER;
		goto ret;
	}

	buffer = GSTalloc (GST_ENCRYPTION_SETUP_IO_BLOCK_SIZE);
	if (!buffer)
	{
		SetupResult = STATUS_INSUFFICIENT_RESOURCES;
		goto ret;
	}

	if (SetupRequest.SetupMode == SetupEncryption && SetupRequest.WipeAlgorithm != GST_WIPE_NONE)
	{
		wipeBuffer = GSTalloc (GST_ENCRYPTION_SETUP_IO_BLOCK_SIZE);
		if (!wipeBuffer)
		{
			SetupResult = STATUS_INSUFFICIENT_RESOURCES;
			goto ret;
		}
	}

	while (!NT_SUCCESS (EncryptedIoQueueHoldWhenIdle (&Extension->Queue, 1000)))
	{
		if (EncryptionSetupThreadAbortRequested)
			goto abort;

		TransformWaitingForIdle = TRUE;
	}
	TransformWaitingForIdle = FALSE;

	switch (SetupRequest.SetupMode)
	{
	case SetupEncryption:
		Dump ("Encrypting...\n");
		if (Extension->Queue.EncryptedAreaStart == -1 || Extension->Queue.EncryptedAreaEnd == -1)
		{
			// Start encryption
			Extension->Queue.EncryptedAreaStart = Extension->ConfiguredEncryptedAreaStart;
			Extension->Queue.EncryptedAreaEnd = -1;
			offset.QuadPart = Extension->ConfiguredEncryptedAreaStart;
		}
		else
		{
			// Resume aborted encryption
			if (Extension->Queue.EncryptedAreaEnd == Extension->ConfiguredEncryptedAreaEnd)
				goto err;

			offset.QuadPart = Extension->Queue.EncryptedAreaEnd + 1;
		}

		break;

	case SetupDecryption:
		Dump ("Decrypting...\n");
		if (Extension->Queue.EncryptedAreaStart == -1 || Extension->Queue.EncryptedAreaEnd == -1)
		{
			SetupResult = STATUS_SUCCESS;
			goto abort;
		}

		offset.QuadPart = Extension->Queue.EncryptedAreaEnd + 1;
		break;

	default:
		goto err;
	}

	EncryptedIoQueueResumeFromHold (&Extension->Queue);
		
	Dump ("EncryptedAreaStart=%I64d\n", Extension->Queue.EncryptedAreaStart);
	Dump ("EncryptedAreaEnd=%I64d\n", Extension->Queue.EncryptedAreaEnd);
	Dump ("ConfiguredEncryptedAreaStart=%I64d\n", Extension->ConfiguredEncryptedAreaStart);
	Dump ("ConfiguredEncryptedAreaEnd=%I64d\n", Extension->ConfiguredEncryptedAreaEnd);
	Dump ("offset=%I64d\n", offset.QuadPart);
	Dump ("EncryptedAreaStart=%I64d (%I64d)  EncryptedAreaEnd=%I64d\n", Extension->Queue.EncryptedAreaStart / 1024 / 1024, Extension->Queue.EncryptedAreaStart, Extension->Queue.EncryptedAreaEnd / 1024 / 1024);

	while (!EncryptionSetupThreadAbortRequested)
	{
		if (SetupRequest.SetupMode == SetupEncryption)
		{
			if (offset.QuadPart + setupBlockSize > Extension->ConfiguredEncryptedAreaEnd + 1)
				setupBlockSize = (ULONG) (Extension->ConfiguredEncryptedAreaEnd + 1 - offset.QuadPart);

			if (offset.QuadPart > Extension->ConfiguredEncryptedAreaEnd)
				break;
		}
		else
		{
			if (offset.QuadPart - setupBlockSize < Extension->Queue.EncryptedAreaStart)
				setupBlockSize = (ULONG) (offset.QuadPart - Extension->Queue.EncryptedAreaStart);

			offset.QuadPart -= setupBlockSize;

			if (setupBlockSize == 0 || offset.QuadPart < Extension->Queue.EncryptedAreaStart)
				break;
		}

		while (!NT_SUCCESS (EncryptedIoQueueHoldWhenIdle (&Extension->Queue, 500)))
		{
			if (EncryptionSetupThreadAbortRequested)
				goto abort;

			TransformWaitingForIdle = TRUE;
		}
		TransformWaitingForIdle = FALSE;

		status = GSTReadDevice (BootDriveFilterExtension->LowerDeviceObject, buffer, offset, setupBlockSize);
		if (!NT_SUCCESS (status))
		{
			Dump ("GSTReadDevice error %x  offset=%I64d\n", status, offset.QuadPart);

			if (SetupRequest.ZeroUnreadableSectors && SetupRequest.SetupMode == SetupEncryption)
			{
				// Zero unreadable sectors
				uint64 zeroedSectorCount;

				status = ZeroUnreadableSectors (BootDriveFilterExtension->LowerDeviceObject, offset, setupBlockSize, &zeroedSectorCount);
				if (!NT_SUCCESS (status))
				{
					SetupResult = status;
					goto err;
				}

				// Retry read
				status = GSTReadDevice (BootDriveFilterExtension->LowerDeviceObject, buffer, offset, setupBlockSize);
				if (!NT_SUCCESS (status))
				{
					SetupResult = status;
					goto err;
				}
			}
			else if (SetupRequest.DiscardUnreadableEncryptedSectors && SetupRequest.SetupMode == SetupDecryption)
			{
				// Discard unreadable encrypted sectors
				uint64 badSectorCount;

				status = ReadDeviceSkipUnreadableSectors (BootDriveFilterExtension->LowerDeviceObject, buffer, offset, setupBlockSize, &badSectorCount);
				if (!NT_SUCCESS (status))
				{
					SetupResult = status;
					goto err;
				}
			}
			else
			{
				SetupResult = status;
				goto err;
			}
		}

		dataUnit.Value = offset.QuadPart / ENCRYPTION_DATA_UNIT_SIZE;

		if (SetupRequest.SetupMode == SetupEncryption)
		{
			EncryptDataUnits (buffer, &dataUnit, setupBlockSize / ENCRYPTION_DATA_UNIT_SIZE, Extension->Queue.CryptoInfo);

			if (SetupRequest.WipeAlgorithm != GST_WIPE_NONE)
			{
				byte wipePass;
				for (wipePass = 1; wipePass <= GetWipePassCount (SetupRequest.WipeAlgorithm); ++wipePass)
				{
					if (!WipeBuffer (SetupRequest.WipeAlgorithm, wipeRandChars, wipePass, wipeBuffer, setupBlockSize))
					{
						ULONG i;
						for (i = 0; i < setupBlockSize; ++i)
						{
							wipeBuffer[i] = buffer[i] + wipePass;
						}

						EncryptDataUnits (wipeBuffer, &dataUnit, setupBlockSize / ENCRYPTION_DATA_UNIT_SIZE, Extension->Queue.CryptoInfo);
						memcpy (wipeRandCharsUpdate, wipeBuffer, sizeof (wipeRandCharsUpdate)); 
					}

					status = GSTWriteDevice (BootDriveFilterExtension->LowerDeviceObject, wipeBuffer, offset, setupBlockSize);
					if (!NT_SUCCESS (status))
					{
						// Undo failed write operation
						DecryptDataUnits (buffer, &dataUnit, setupBlockSize / ENCRYPTION_DATA_UNIT_SIZE, Extension->Queue.CryptoInfo);
						GSTWriteDevice (BootDriveFilterExtension->LowerDeviceObject, buffer, offset, setupBlockSize);

						SetupResult = status;
						goto err;
					}
				}

				memcpy (wipeRandChars, wipeRandCharsUpdate, sizeof (wipeRandCharsUpdate)); 
			}
		}
		else
		{
			DecryptDataUnits (buffer, &dataUnit, setupBlockSize / ENCRYPTION_DATA_UNIT_SIZE, Extension->Queue.CryptoInfo);
		}

		status = GSTWriteDevice (BootDriveFilterExtension->LowerDeviceObject, buffer, offset, setupBlockSize);
		if (!NT_SUCCESS (status))
		{
			Dump ("GSTWriteDevice error %x\n", status);

			// Undo failed write operation
			if (SetupRequest.SetupMode == SetupEncryption)
				DecryptDataUnits (buffer, &dataUnit, setupBlockSize / ENCRYPTION_DATA_UNIT_SIZE, Extension->Queue.CryptoInfo);
			else
				EncryptDataUnits (buffer, &dataUnit, setupBlockSize / ENCRYPTION_DATA_UNIT_SIZE, Extension->Queue.CryptoInfo);

			GSTWriteDevice (BootDriveFilterExtension->LowerDeviceObject, buffer, offset, setupBlockSize);

			SetupResult = status;
			goto err;
		}

		if (SetupRequest.SetupMode == SetupEncryption)
			offset.QuadPart += setupBlockSize;

		Extension->Queue.EncryptedAreaEndUpdatePending = TRUE;
		Extension->Queue.EncryptedAreaEnd = offset.QuadPart - 1;
		Extension->Queue.EncryptedAreaEndUpdatePending = FALSE;

		headerUpdateRequired = TRUE;

		EncryptedIoQueueResumeFromHold (&Extension->Queue);

		KeAcquireSpinLock (&SetupStatusSpinLock, &irql);
		SetupStatusEncryptedAreaEnd = Extension->Queue.EncryptedAreaEnd;
		KeReleaseSpinLock (&SetupStatusSpinLock, irql);

		// Update volume header
		bytesWrittenSinceHeaderUpdate += setupBlockSize;
		if (bytesWrittenSinceHeaderUpdate >= GST_ENCRYPTION_SETUP_HEADER_UPDATE_THRESHOLD)
		{
			status = SaveDriveVolumeHeader (Extension);
			ASSERT (NT_SUCCESS (status));

			headerUpdateRequired = FALSE;
			bytesWrittenSinceHeaderUpdate = 0;
		}
	}

abort:
	SetupResult = STATUS_SUCCESS;
err:

	if (Extension->Queue.EncryptedAreaEnd == -1)
		Extension->Queue.EncryptedAreaStart = -1;

	if (EncryptedIoQueueIsSuspended (&Extension->Queue))
		EncryptedIoQueueResumeFromHold (&Extension->Queue);

	if (SetupRequest.SetupMode == SetupDecryption && Extension->Queue.EncryptedAreaStart >= Extension->Queue.EncryptedAreaEnd)
	{
		while (!NT_SUCCESS (EncryptedIoQueueHoldWhenIdle (&Extension->Queue, 0)));

		Extension->ConfiguredEncryptedAreaStart = Extension->ConfiguredEncryptedAreaEnd = -1;
		Extension->Queue.EncryptedAreaStart = Extension->Queue.EncryptedAreaEnd = -1;

		EncryptedIoQueueResumeFromHold (&Extension->Queue);

		headerUpdateRequired = TRUE;
	}

	Dump ("Setup completed:  EncryptedAreaStart=%I64d (%I64d)  EncryptedAreaEnd=%I64d (%I64d)\n", Extension->Queue.EncryptedAreaStart / 1024 / 1024, Extension->Queue.EncryptedAreaStart, Extension->Queue.EncryptedAreaEnd / 1024 / 1024, Extension->Queue.EncryptedAreaEnd);

	if (headerUpdateRequired)
	{
		status = SaveDriveVolumeHeader (Extension);

		if (!NT_SUCCESS (status) && NT_SUCCESS (SetupResult))
			SetupResult = status;
	}

	if (SetupRequest.SetupMode == SetupDecryption && Extension->ConfiguredEncryptedAreaEnd == -1 && Extension->DriveMounted)
	{
		while (!RootDeviceControlMutexAcquireNoWait() && !EncryptionSetupThreadAbortRequested)
		{
			GSTSleep (10);
		}

		// Disable hibernation (resume would fail due to a change in the system memory map)
		HibernationEnabled = FALSE;

		DismountDrive (Extension, FALSE);

		if (!EncryptionSetupThreadAbortRequested)
			RootDeviceControlMutexRelease();
	}

ret:
	if (buffer)
		GSTfree (buffer);
	if (wipeBuffer)
		GSTfree (wipeBuffer);

	SetupInProgress = FALSE;
	PsTerminateSystemThread (SetupResult);
}


/**
 *
 *	\fn NTSTATUS StartBootEncryptionSetup (PDEVICE_OBJECT DeviceObject, PIRP irp, PIO_STACK_LOCATION irpSp)
 *	\brief Send to the device the packet that will ask to encrypt.
 *	\param[in] PIO_STACK_LOCATION irpSp
 *	\param[in] PIRP irp
 *	\param[in] PDEVICE_OBJECT DeviceObject
 *	\return NTSTATUS status
 *
 */
NTSTATUS StartBootEncryptionSetup (PDEVICE_OBJECT DeviceObject, PIRP irp, PIO_STACK_LOCATION irpSp)
{
	NTSTATUS status;

	if (!UserCanAccessDriveDevice())
		return STATUS_ACCESS_DENIED;

	if (SetupInProgress || !BootDriveFound || !BootDriveFilterExtension
		|| !BootDriveFilterExtension->DriveMounted
		|| BootDriveFilterExtension->HiddenSystem
		|| irpSp->Parameters.DeviceIoControl.InputBufferLength < sizeof (BootEncryptionSetupRequest))
		return STATUS_INVALID_PARAMETER;

	if (EncryptionSetupThread)
		AbortBootEncryptionSetup();

	SetupRequest = *(BootEncryptionSetupRequest *) irp->AssociatedIrp.SystemBuffer;

	EncryptionSetupThreadAbortRequested = FALSE;
	KeInitializeSpinLock (&SetupStatusSpinLock);
	SetupStatusEncryptedAreaEnd = BootDriveFilterExtension ? BootDriveFilterExtension->Queue.EncryptedAreaEnd : -1;

	SetupInProgress = TRUE;
	status = GSTStartThread (SetupThreadProc, DeviceObject, &EncryptionSetupThread);
	
	if (!NT_SUCCESS (status))
		SetupInProgress = FALSE;

	return status;
}


/**
 *
 *	\fn void GetBootDriveVolumeProperties (PIRP irp, PIO_STACK_LOCATION irpSp)
 *	\brief Get the boot properties of the volume 
 *	\param[in] PIO_STACK_LOCATION irpSp
 *	\param[in] PIRP irp
 *	\return void
 *
 */
void GetBootDriveVolumeProperties (PIRP irp, PIO_STACK_LOCATION irpSp)
{
	if (ValidateIOBufferSize (irp, sizeof (VOLUME_PROPERTIES_STRUCT), ValidateOutput))
	{
		DriveFilterExtension *Extension = BootDriveFilterExtension;
		VOLUME_PROPERTIES_STRUCT *prop = (VOLUME_PROPERTIES_STRUCT *) irp->AssociatedIrp.SystemBuffer;
		memset (prop, 0, sizeof (*prop));

		if (!BootDriveFound || !Extension || !Extension->DriveMounted)
		{
			irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			irp->IoStatus.Information = 0;
		}
		else
		{
			prop->hiddenVolume = Extension->Queue.CryptoInfo->hiddenVolume;
			prop->diskLength = Extension->ConfiguredEncryptedAreaEnd + 1 - Extension->ConfiguredEncryptedAreaStart;
			prop->ea = Extension->Queue.CryptoInfo->ea;
			prop->mode = Extension->Queue.CryptoInfo->mode;
			prop->pkcs5 = Extension->Queue.CryptoInfo->pkcs5;
			prop->pkcs5Iterations = Extension->Queue.CryptoInfo->noIterations;
#if 0
			prop->volumeCreationTime = Extension->Queue.CryptoInfo->volume_creation_time;
			prop->headerCreationTime = Extension->Queue.CryptoInfo->header_creation_time;
#endif
			prop->volFormatVersion = Extension->Queue.CryptoInfo->LegacyVolume ? GST_VOLUME_FORMAT_VERSION_PRE_6_0 : GST_VOLUME_FORMAT_VERSION;

			prop->totalBytesRead = Extension->Queue.TotalBytesRead;
			prop->totalBytesWritten = Extension->Queue.TotalBytesWritten;

			irp->IoStatus.Information = sizeof (VOLUME_PROPERTIES_STRUCT);
			irp->IoStatus.Status = STATUS_SUCCESS;
		}
	}
}


/**
 *
 *	\fn void GetBootEncryptionStatus (PIRP irp, PIO_STACK_LOCATION irpSp)
 *	\brief Get the boot encryption status and put into the structure bootEncStatus.
 *	\param[in] PIO_STACK_LOCATION irpSp
 *	\param[in] PIRP irp
 *	\return void
 *
 */
void GetBootEncryptionStatus (PIRP irp, PIO_STACK_LOCATION irpSp)
{
	/* IMPORTANT: Do NOT add any potentially time-consuming operations to this function. */

	if (ValidateIOBufferSize (irp, sizeof (BootEncryptionStatus), ValidateOutput))
	{
		DriveFilterExtension *Extension = BootDriveFilterExtension;
		BootEncryptionStatus *bootEncStatus = (BootEncryptionStatus *) irp->AssociatedIrp.SystemBuffer;
		memset (bootEncStatus, 0, sizeof (*bootEncStatus));

		if (BootArgsValid)
			bootEncStatus->BootLoaderVersion = BootArgs.BootLoaderVersion;

		bootEncStatus->DeviceFilterActive = DeviceFilterActive;
		bootEncStatus->SetupInProgress = SetupInProgress;
		bootEncStatus->SetupMode = SetupRequest.SetupMode;
		bootEncStatus->TransformWaitingForIdle = TransformWaitingForIdle;

		if (!BootDriveFound || !Extension || !Extension->DriveMounted)
		{
			bootEncStatus->DriveEncrypted = FALSE;
			bootEncStatus->DriveMounted = FALSE;
			bootEncStatus->VolumeHeaderPresent = FALSE;
		}
		else
		{
			bootEncStatus->DriveMounted = Extension->DriveMounted;
			bootEncStatus->VolumeHeaderPresent = Extension->VolumeHeaderPresent;
			bootEncStatus->DriveEncrypted = Extension->Queue.EncryptedAreaStart != -1;
			bootEncStatus->BootDriveLength = BootDriveLength;

			bootEncStatus->ConfiguredEncryptedAreaStart = Extension->ConfiguredEncryptedAreaStart;
			bootEncStatus->ConfiguredEncryptedAreaEnd = Extension->ConfiguredEncryptedAreaEnd;
			bootEncStatus->EncryptedAreaStart = Extension->Queue.EncryptedAreaStart;

			if (SetupInProgress)
			{
				KIRQL irql;
				KeAcquireSpinLock (&SetupStatusSpinLock, &irql);
				bootEncStatus->EncryptedAreaEnd = SetupStatusEncryptedAreaEnd;
				KeReleaseSpinLock (&SetupStatusSpinLock, irql);
			}
			else
				bootEncStatus->EncryptedAreaEnd = Extension->Queue.EncryptedAreaEnd;

			bootEncStatus->VolumeHeaderSaltCrc32 = Extension->VolumeHeaderSaltCrc32;
			bootEncStatus->HibernationPreventionCount = HibernationPreventionCount;
			bootEncStatus->HiddenSysLeakProtectionCount = HiddenSysLeakProtectionCount;

			bootEncStatus->HiddenSystem = Extension->HiddenSystem;
			
			if (Extension->HiddenSystem)
				bootEncStatus->HiddenSystemPartitionStart = BootArgs.HiddenSystemPartitionStart;
		}

		irp->IoStatus.Information = sizeof (BootEncryptionStatus);
		irp->IoStatus.Status = STATUS_SUCCESS;
	}
}


/**
 *
 *	\fn void GetBootLoaderVersion (PIRP irp, PIO_STACK_LOCATION irpSp)
 *	\brief Get the boot loader version by sending a packet to the driver.
 *	\param[in] PIO_STACK_LOCATION irpSp
 *	\param[in] PIRP irp
 *	\return void
 *
 */
void GetBootLoaderVersion (PIRP irp, PIO_STACK_LOCATION irpSp)
{
	if (ValidateIOBufferSize (irp, sizeof (uint16), ValidateOutput))
	{
		if (BootArgsValid)
		{
			*(uint16 *) irp->AssociatedIrp.SystemBuffer = BootArgs.BootLoaderVersion;
			irp->IoStatus.Information = sizeof (uint16);
			irp->IoStatus.Status = STATUS_SUCCESS;
		}
		else
		{
			irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			irp->IoStatus.Information = 0;
		}
	}
}


/**
 *
 *	\fn void GetBootEncryptionAlgorithmName (PIRP irp, PIO_STACK_LOCATION irpSp)
 *	\brief Send a packet to the driver to know the algorithm used
 *	\param[in] PIO_STACK_LOCATION irpSp
 *	\param[in] PIRP irp
 *	\return void
 *
 */
void GetBootEncryptionAlgorithmName (PIRP irp, PIO_STACK_LOCATION irpSp)
{
	if (ValidateIOBufferSize (irp, sizeof (GetBootEncryptionAlgorithmNameRequest), ValidateOutput))
	{
		if (BootDriveFilterExtension && BootDriveFilterExtension->DriveMounted)
		{
			GetBootEncryptionAlgorithmNameRequest *request = (GetBootEncryptionAlgorithmNameRequest *) irp->AssociatedIrp.SystemBuffer;
			EAGetName (request->BootEncryptionAlgorithmName, BootDriveFilterExtension->Queue.CryptoInfo->ea);

			irp->IoStatus.Information = sizeof (GetBootEncryptionAlgorithmNameRequest);
			irp->IoStatus.Status = STATUS_SUCCESS;
		}
		else
		{
			irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			irp->IoStatus.Information = 0;
		}
	}
}


/**
 *
 *	\fn NTSTATUS GetSetupResult()
 *	\brief Getter for Setupd Result
 *	\return NTSTATUS SetupResult
 *
 */
NTSTATUS GetSetupResult()
{
	return SetupResult;
}


/**
 *
 *	\fn BOOL IsBootDriveMounted ()
 *	\brief Getter to know if a drive is mounted
 *	\return BOOL BootDriveFilterExtension && BootDriveFilterExtension->DriveMounted
 *
 */
BOOL IsBootDriveMounted ()
{
	return BootDriveFilterExtension && BootDriveFilterExtension->DriveMounted;
}


/**
 *
 *	\fn BOOL IsBootEncryptionSetupInProgress ()
 *	\brief Getter for Setud in progress variable
 *	\return BOOL SetupInProgress
 *
 */
BOOL IsBootEncryptionSetupInProgress ()
{
	return SetupInProgress;
}


/**
 *
 *	\fn BOOL IsHiddenSystemRunning ()
 *	\brief Getter to know if the system is running
 *	\return BOOL BootDriveFilterExtension && BootDriveFilterExtension->HiddenSystem
 *
 */
BOOL IsHiddenSystemRunning ()
{
	return BootDriveFilterExtension && BootDriveFilterExtension->HiddenSystem;
}


/**
 *
 *	\fn DriveFilterExtension *GetBootDriveFilterExtension ()
 *	\brief Getter for the boot drive filter extension
 *	\return DriveFilterExtension * BootDriveFilterExtension
 *
 */
DriveFilterExtension *GetBootDriveFilterExtension ()
{
	return BootDriveFilterExtension;
}


/**
 *
 *	\fn CRYPTO_INFO *GetSystemDriveCryptoInfo ()
 *	\brief Getter for the cryptologic information about the system drive
 *	\return CRYPTO_INFO * BootDriveFilterExtension->Queue.CryptoInfo
 *
 */
CRYPTO_INFO *GetSystemDriveCryptoInfo ()
{
	return BootDriveFilterExtension->Queue.CryptoInfo;
}


/**
 *
 *	\fn NTSTATUS AbortBootEncryptionSetup ()
 *	\brief Stop the encryption setup for the boot.
 *	\return NTSTATUS STATUS_SUCCESS if success
 *			STATUS_ACCESS_DENIED if not
 *
 */
NTSTATUS AbortBootEncryptionSetup ()
{
	if (!IoIsSystemThread (PsGetCurrentThread()) && !UserCanAccessDriveDevice())
		return STATUS_ACCESS_DENIED;

	if (EncryptionSetupThread)
	{
		EncryptionSetupThreadAbortRequested = TRUE;

		GSTStopThread (EncryptionSetupThread, NULL);
		EncryptionSetupThread = NULL;
	}

	return STATUS_SUCCESS;
}


/**
 *
 *	\fn static VOID DecoySystemWipeThreadProc (PVOID threadArg)
 *	\brief Reinitiliase the crypto info and change it. Encrypt data in a random order
 *	\param[in] PVOID threadArg
 *	\return static VOID
 *
 */
static VOID DecoySystemWipeThreadProc (PVOID threadArg)
{
	DriveFilterExtension *Extension = BootDriveFilterExtension;

	LARGE_INTEGER offset;
	UINT64_STRUCT dataUnit;
	ULONG wipeBlockSize = GST_ENCRYPTION_SETUP_IO_BLOCK_SIZE;

	CRYPTO_INFO *wipeCryptoInfo = NULL;
	byte *wipeBuffer = NULL;
	byte *wipeRandBuffer = NULL;
	byte wipeRandChars[GST_WIPE_RAND_CHAR_COUNT];
	int wipePass;
	int ea = Extension->Queue.CryptoInfo->ea;

	KIRQL irql;
	NTSTATUS status;

	DecoySystemWipeResult = STATUS_UNSUCCESSFUL;

	wipeBuffer = GSTalloc (GST_ENCRYPTION_SETUP_IO_BLOCK_SIZE);
	if (!wipeBuffer)
	{
		DecoySystemWipeResult = STATUS_INSUFFICIENT_RESOURCES;
		goto ret;
	}
	
	wipeRandBuffer = GSTalloc (GST_ENCRYPTION_SETUP_IO_BLOCK_SIZE);
	if (!wipeRandBuffer)
	{
		DecoySystemWipeResult = STATUS_INSUFFICIENT_RESOURCES;
		goto ret;
	}

	wipeCryptoInfo = crypto_open();
	if (!wipeCryptoInfo)
	{
		DecoySystemWipeResult = STATUS_INSUFFICIENT_RESOURCES;
		goto ret;
	}

	wipeCryptoInfo->ea = ea;
	wipeCryptoInfo->mode = Extension->Queue.CryptoInfo->mode;

	if (EAInit (ea, WipeDecoyRequest.WipeKey, wipeCryptoInfo->ks) != ERR_SUCCESS)
	{
		DecoySystemWipeResult = STATUS_INVALID_PARAMETER;
		goto ret;
	}

	memcpy (wipeCryptoInfo->k2, WipeDecoyRequest.WipeKey + EAGetKeySize (ea), EAGetKeySize (ea));
	
	if (!EAInitMode (wipeCryptoInfo))
	{
		DecoySystemWipeResult = STATUS_INVALID_PARAMETER;
		goto err;
	}

	EncryptDataUnits (wipeRandBuffer, &dataUnit, wipeBlockSize / ENCRYPTION_DATA_UNIT_SIZE, wipeCryptoInfo);
	memcpy (wipeRandChars, wipeRandBuffer, sizeof (wipeRandChars));

	burn (WipeDecoyRequest.WipeKey, sizeof (WipeDecoyRequest.WipeKey));

	offset.QuadPart = Extension->ConfiguredEncryptedAreaStart;
		
	Dump ("Wiping decoy system:  start offset = %I64d\n", offset.QuadPart);

	while (!DecoySystemWipeThreadAbortRequested)
	{
		if (offset.QuadPart + wipeBlockSize > Extension->ConfiguredEncryptedAreaEnd + 1)
			wipeBlockSize = (ULONG) (Extension->ConfiguredEncryptedAreaEnd + 1 - offset.QuadPart);

		if (offset.QuadPart > Extension->ConfiguredEncryptedAreaEnd)
			break;

		for (wipePass = 1; wipePass <= GetWipePassCount (WipeDecoyRequest.WipeAlgorithm); ++wipePass)
		{
			if (!WipeBuffer (WipeDecoyRequest.WipeAlgorithm, wipeRandChars, wipePass, wipeBuffer, wipeBlockSize))
			{
				dataUnit.Value = offset.QuadPart / ENCRYPTION_DATA_UNIT_SIZE;
				EncryptDataUnits (wipeRandBuffer, &dataUnit, wipeBlockSize / ENCRYPTION_DATA_UNIT_SIZE, wipeCryptoInfo);
				memcpy (wipeBuffer, wipeRandBuffer, wipeBlockSize);
			}

			while (!NT_SUCCESS (EncryptedIoQueueHoldWhenIdle (&Extension->Queue, 500)))
			{
				if (DecoySystemWipeThreadAbortRequested)
					goto abort;
			}

			status = GSTWriteDevice (BootDriveFilterExtension->LowerDeviceObject, wipeBuffer, offset, wipeBlockSize);

			if (!NT_SUCCESS (status))
			{
				DecoySystemWipeResult = status;
				goto err;
			}

			EncryptedIoQueueResumeFromHold (&Extension->Queue);
		}

		offset.QuadPart += wipeBlockSize;

		KeAcquireSpinLock (&DecoySystemWipeStatusSpinLock, &irql);
		DecoySystemWipedAreaEnd = offset.QuadPart - 1;
		KeReleaseSpinLock (&DecoySystemWipeStatusSpinLock, irql);
	}

abort:
	DecoySystemWipeResult = STATUS_SUCCESS;
err:

	if (EncryptedIoQueueIsSuspended (&Extension->Queue))
		EncryptedIoQueueResumeFromHold (&Extension->Queue);

	Dump ("Wipe end: DecoySystemWipedAreaEnd=%I64d (%I64d)\n", DecoySystemWipedAreaEnd, DecoySystemWipedAreaEnd / 1024 / 1024);

ret:
	if (wipeCryptoInfo)
		crypto_close (wipeCryptoInfo);

	if (wipeRandBuffer)
		GSTfree (wipeRandBuffer);

	if (wipeBuffer)
		GSTfree (wipeBuffer);

	DecoySystemWipeInProgress = FALSE;
	PsTerminateSystemThread (DecoySystemWipeResult);
}


/**
 *
 *	\fn NTSTATUS StartDecoySystemWipe (PDEVICE_OBJECT DeviceObject, PIRP irp, PIO_STACK_LOCATION irpSp)
 *	\brief Launch the abort of the decoy encrypting system
 *	\param[in] PIO_STACK_LOCATION irpSp
 *	\param[in] PIRP irp
 *	\param[in] PDEVICE_OBJECT DeviceObject
 *	\return NTSTATUS status
 *
 */
NTSTATUS StartDecoySystemWipe (PDEVICE_OBJECT DeviceObject, PIRP irp, PIO_STACK_LOCATION irpSp)
{
	NTSTATUS status;
	WipeDecoySystemRequest *request;

	if (!UserCanAccessDriveDevice())
		return STATUS_ACCESS_DENIED;

	if (!IsHiddenSystemRunning()
		|| irpSp->Parameters.DeviceIoControl.InputBufferLength < sizeof (WipeDecoySystemRequest))
		return STATUS_INVALID_PARAMETER;

	if (DecoySystemWipeInProgress)
		return STATUS_SUCCESS;

	if (DecoySystemWipeThread)
		AbortDecoySystemWipe();

	request = (WipeDecoySystemRequest *) irp->AssociatedIrp.SystemBuffer;
	WipeDecoyRequest = *request;

	burn (request->WipeKey, sizeof (request->WipeKey));

	DecoySystemWipeThreadAbortRequested = FALSE;
	KeInitializeSpinLock (&DecoySystemWipeStatusSpinLock);
	DecoySystemWipedAreaEnd = BootDriveFilterExtension->ConfiguredEncryptedAreaStart;

	DecoySystemWipeInProgress = TRUE;
	status = GSTStartThread (DecoySystemWipeThreadProc, DeviceObject, &DecoySystemWipeThread);
	
	if (!NT_SUCCESS (status))
		DecoySystemWipeInProgress = FALSE;

	return status;
}


/**
 *
 *	\fn BOOL IsDecoySystemWipeInProgress()
 *	\brief Getter for the decoy wipe system is running or not.
 *	\return BOOL DecoySystemWipeInProgress
 *
 */
BOOL IsDecoySystemWipeInProgress()
{
	return DecoySystemWipeInProgress;
}


/**
 *
 *	\fn void GetDecoySystemWipeStatus (PIRP irp, PIO_STACK_LOCATION irpSp)
 *	\brief Getter for the decoy wipe system status
 *	\param[in] PIO_STACK_LOCATION irpSp
 *	\param[in] PIRP irp
 *	\return void
 *
 */
void GetDecoySystemWipeStatus (PIRP irp, PIO_STACK_LOCATION irpSp)
{
	if (ValidateIOBufferSize (irp, sizeof (DecoySystemWipeStatus), ValidateOutput))
	{
		DecoySystemWipeStatus *wipeStatus = (DecoySystemWipeStatus *) irp->AssociatedIrp.SystemBuffer;

		if (!IsHiddenSystemRunning())
		{
			irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			irp->IoStatus.Information = 0;
		}
		else
		{
			wipeStatus->WipeInProgress = DecoySystemWipeInProgress;
			wipeStatus->WipeAlgorithm = WipeDecoyRequest.WipeAlgorithm;

			if (DecoySystemWipeInProgress)
			{
				KIRQL irql;
				KeAcquireSpinLock (&DecoySystemWipeStatusSpinLock, &irql);
				wipeStatus->WipedAreaEnd = DecoySystemWipedAreaEnd;
				KeReleaseSpinLock (&DecoySystemWipeStatusSpinLock, irql);
			}
			else
				wipeStatus->WipedAreaEnd = DecoySystemWipedAreaEnd;
			
			irp->IoStatus.Information = sizeof (DecoySystemWipeStatus);
			irp->IoStatus.Status = STATUS_SUCCESS;
		}
	}
}


/**
 *
 *	\fn NTSTATUS GetDecoySystemWipeResult()
 *	\brief Getter for the decoy system wipe result
 *	\return NTSTATUS DecoySystemWipeResult
 *
 */
NTSTATUS GetDecoySystemWipeResult()
{
	return DecoySystemWipeResult;
}


/**
 *
 *	\fn NTSTATUS AbortDecoySystemWipe ()
 *	\brief Abort decoy system wipe
 *	\return NTSTATUS STATUS_SUCCESS if success
 *			STATUS_ACCESS_DENIED if not
 *
 */
NTSTATUS AbortDecoySystemWipe ()
{
	if (!IoIsSystemThread (PsGetCurrentThread()) && !UserCanAccessDriveDevice())
		return STATUS_ACCESS_DENIED;

	if (DecoySystemWipeThread)
	{
		DecoySystemWipeThreadAbortRequested = TRUE;

		GSTStopThread (DecoySystemWipeThread, NULL);
		DecoySystemWipeThread = NULL;
	}

	return STATUS_SUCCESS;
}


/**
 *
 *	\fn uint64 GetBootDriveLength ()
 *	\brief Getter for the boot drive length
 *	\return uint64 BootDriveLength.QuadPart
 *
 */
uint64 GetBootDriveLength ()
{
	return BootDriveLength.QuadPart;
}


/**
 *
 *	\fn NTSTATUS WriteBootDriveSector (PIRP irp, PIO_STACK_LOCATION irpSp)
 *	\brief Write into a soctor of the boot drive.
 *	\param[in] PIO_STACK_LOCATION irpSp
 *	\param[in] PIRP irp
 *	\return NTSTATUS STATUS_ACCESS_DENIED
 *			STATUS_INVALID_PARAMETER
 *
 */
NTSTATUS WriteBootDriveSector (PIRP irp, PIO_STACK_LOCATION irpSp)
{
	WriteBootDriveSectorRequest *request;

	if (!UserCanAccessDriveDevice())
		return STATUS_ACCESS_DENIED;

	if (!BootDriveFilterExtension
		|| irpSp->Parameters.DeviceIoControl.InputBufferLength < sizeof (WriteBootDriveSectorRequest))
		return STATUS_INVALID_PARAMETER;

	request = (WriteBootDriveSectorRequest *) irp->AssociatedIrp.SystemBuffer;
	return GSTWriteDevice (BootDriveFilterExtension->LowerDeviceObject, request->Data, request->Offset, sizeof (request->Data));
}
