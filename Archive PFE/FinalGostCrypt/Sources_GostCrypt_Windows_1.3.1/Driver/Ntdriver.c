/**
 *
 *	\file Ntdriver.c
 *	\brief The driver main functionsthat will open / mount / unload / add devices / dump memory ...
 *	\version 1.3.1
 *	\date 13/12/2016
 *
 */

/**
 *
 *	Legal Notice: Some portions of the source code contained in this file were
 *	derived from the source code of Encryption for the Masses 2.02a, which is
 *	Copyright (c) 1998-2000 Paul Le Roux and which is governed by the 'License
 *	Agreement for Encryption for the Masses'. Modifications and additions to
 *	the original source code (contained in this file) and all other portions
 *	of this file are Copyright (c) 2003-2012 TrueCrypt Developers Association
 *	and are governed by the TrueCrypt License 3.0 the full text of which is
 *	contained in the file License.txt included in TrueCrypt binary and source
 *	code distribution packages.
 *
 */

#include "Gstdefs.h"
#include <ntddk.h>
#include "Crypto.h"
#include "Fat.h"
#include "Tests.h"

#include "Endian.h"

#include "Apidrvr.h"
#include "Boot/Windows/BootDefs.h"
#include "EncryptedIoQueue.h"
#include "EncryptionThreadPool.h"
#include "Ntdriver.h"
#include "Ntvol.h"
#include "DriveFilter.h"
#include "DumpFilter.h"
#include "Cache.h"
#include "Volumes.h"
#include "VolumeFilter.h"

#include <tchar.h>
#include <initguid.h>
#include <mountmgr.h>
#include <mountdev.h>
#include <ntddvol.h>

/* Init section, which is thrown away as soon as DriverEntry returns */
#pragma alloc_text(INIT,DriverEntry)
#pragma alloc_text(INIT,GSTCreateRootDeviceObject)

PDRIVER_OBJECT GSTDriverObject;
PDEVICE_OBJECT RootDeviceObject = NULL;
static KMUTEX RootDeviceControlMutex;
BOOL DriverShuttingDown = FALSE;
BOOL SelfTestsPassed;
int LastUniqueVolumeId;
ULONG OsMajorVersion = 0;
ULONG OsMinorVersion;
BOOL DriverUnloadDisabled = FALSE;
BOOL PortableMode = FALSE;
BOOL VolumeClassFilterRegistered = FALSE;
BOOL CacheBootPassword = FALSE;
BOOL NonAdminSystemFavoritesAccessDisabled = FALSE;
static size_t EncryptionThreadPoolFreeCpuCountLimit = 0;
static BOOL SystemFavoriteVolumeDirty = FALSE;
static BOOL PagingFileCreationPrevented = FALSE;

PDEVICE_OBJECT VirtualVolumeDeviceObjects[MAX_MOUNTED_VOLUME_DRIVE_NUMBER + 1];


/**
 *
 *	\fn NTSTATUS DriverEntry (PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
 *	\brief Load dump filter if the main driver is already loaded
 *			Enable device class filters and load boot arguments if the driver is set to start at system boot
 *	\param[in] PUNICODE_STRING RegistryPath
 *	\param[in] PDRIVER_OBJECT DriverObject
 *	\return NTSTATUS GSTCreateRootDeviceObject (DriverObject)
 *
 */
NTSTATUS DriverEntry (PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	PKEY_VALUE_PARTIAL_INFORMATION startKeyValue;
	LONG version;
	int i;

	Dump ("DriverEntry " GST_APP_NAME " " VERSION_STRING "\n");

	PsGetVersion (&OsMajorVersion, &OsMinorVersion, NULL, NULL);

	// Load dump filter if the main driver is already loaded
	if (NT_SUCCESS (GSTDeviceIoControl (NT_ROOT_PREFIX, GST_IOCTL_GET_DRIVER_VERSION, NULL, 0, &version, sizeof (version))))
		return DumpFilterEntry ((PFILTER_EXTENSION) DriverObject, (PFILTER_INITIALIZATION_DATA) RegistryPath);

	GSTDriverObject = DriverObject;
	memset (VirtualVolumeDeviceObjects, 0, sizeof (VirtualVolumeDeviceObjects));

	ReadRegistryConfigFlags (TRUE);
	EncryptionThreadPoolStart (EncryptionThreadPoolFreeCpuCountLimit);
	SelfTestsPassed = AutoTestAlgorithms();

	// Enable device class filters and load boot arguments if the driver is set to start at system boot
		
	if (NT_SUCCESS (GSTReadRegistryKey (RegistryPath, L"Start", &startKeyValue)))
	{
		if (startKeyValue->Type == REG_DWORD && *((uint32 *) startKeyValue->Data) == SERVICE_BOOT_START)
		{
			if (!SelfTestsPassed)
				GST_BUG_CHECK (STATUS_INVALID_PARAMETER);

			LoadBootArguments();
			VolumeClassFilterRegistered = IsVolumeClassFilterRegistered();

			DriverObject->DriverExtension->AddDevice = DriverAddDevice;
		}

		GSTfree (startKeyValue);
	}

	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = GSTDispatchQueueIRP;
	}

	DriverObject->DriverUnload = GSTUnloadDriver;
	return GSTCreateRootDeviceObject (DriverObject);
}


/**
 *
 *	\fn NTSTATUS DriverAddDevice (PDRIVER_OBJECT driverObject, PDEVICE_OBJECT pdo)
 *	\brief Add a device to the driver object
 *	\param[in] PDEVICE_OBJECT pdo
 *	\param[in] PDRIVER_OBJECT driverObject
 *	\return NTSTATUS DriveFilterAddDevice (driverObject, pdo)
 *
 */
NTSTATUS DriverAddDevice (PDRIVER_OBJECT driverObject, PDEVICE_OBJECT pdo)
{
#ifdef DEBUG
	char nameInfoBuffer[128];
	POBJECT_NAME_INFORMATION nameInfo = (POBJECT_NAME_INFORMATION) nameInfoBuffer;
	ULONG nameInfoSize;
	Dump ("AddDevice pdo=%p type=%x name=%ws\n", pdo, pdo->DeviceType, NT_SUCCESS (ObQueryNameString (pdo, nameInfo, sizeof (nameInfoBuffer), &nameInfoSize)) ? nameInfo->Name.Buffer : L"?");
#endif

	if (VolumeClassFilterRegistered && BootArgsValid && BootArgs.HiddenSystemPartitionStart != 0)
	{
		PWSTR interfaceLinks;
		if (NT_SUCCESS (IoGetDeviceInterfaces (&GUID_DEVINTERFACE_VOLUME, pdo, DEVICE_INTERFACE_INCLUDE_NONACTIVE, &interfaceLinks)))
		{
			if (interfaceLinks[0] != UNICODE_NULL)
			{
				Dump ("Volume pdo=%p interface=%ws\n", pdo, interfaceLinks);
				ExFreePool (interfaceLinks);

				return VolumeFilterAddDevice (driverObject, pdo);
			}

			ExFreePool (interfaceLinks);
		}
	}

	return DriveFilterAddDevice (driverObject, pdo);
}



/**
 *
 *	\fn void DumpMemory (void *mem, int size)
 *	\brief Dumps a memory region to debug output
 *	\param[in] int size
 *	\param[in] void * mem
 *	\return void
 *
 */
void DumpMemory (void *mem, int size)
{
	unsigned char str[20];
	unsigned char *m = mem;
	int i,j;

	for (j = 0; j < size / 8; j++)
	{
		memset (str,0,sizeof str);
		for (i = 0; i < 8; i++) 
		{
			if (m[i] > ' ' && m[i] <= '~')
				str[i]=m[i];
			else
				str[i]='.';
		}

		Dump ("0x%08p  %02x %02x %02x %02x %02x %02x %02x %02x  %s\n",
			m, m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], str);

		m+=8;
	}
}


/**
 *
 *	\fn BOOL ValidateIOBufferSize (PIRP irp, size_t requiredBufferSize, ValidateIOBufferSizeType type)
 *	\brief Validate the buffer size and return TRUE if the requiredBufferSize argument is good.
 *	\param[in] ValidateIOBufferSizeType type
 *	\param[in] size_t requiredBufferSize
 *	\param[in] PIRP irp
 *	\return BOOL FALSE
 *	TRUE
 *
 */
BOOL ValidateIOBufferSize (PIRP irp, size_t requiredBufferSize, ValidateIOBufferSizeType type)
{
	PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation (irp);
	BOOL input = (type == ValidateInput || type == ValidateInputOutput);
	BOOL output = (type == ValidateOutput || type == ValidateInputOutput);

	if ((input && irpSp->Parameters.DeviceIoControl.InputBufferLength < requiredBufferSize)
		|| (output && irpSp->Parameters.DeviceIoControl.OutputBufferLength < requiredBufferSize))
	{
		Dump ("STATUS_BUFFER_TOO_SMALL ioctl=0x%x,%d in=%d out=%d reqsize=%d insize=%d outsize=%d\n", (int) (irpSp->Parameters.DeviceIoControl.IoControlCode >> 16), (int) ((irpSp->Parameters.DeviceIoControl.IoControlCode & 0x1FFF) >> 2), input, output, requiredBufferSize, irpSp->Parameters.DeviceIoControl.InputBufferLength, irpSp->Parameters.DeviceIoControl.OutputBufferLength);

		irp->IoStatus.Status = STATUS_BUFFER_TOO_SMALL;
		irp->IoStatus.Information = 0;
		return FALSE;
	}

	if (!input && output)
		memset (irp->AssociatedIrp.SystemBuffer, 0, irpSp->Parameters.DeviceIoControl.OutputBufferLength);

	return TRUE;
}


/**
 *
 *	\fn PDEVICE_OBJECT GetVirtualVolumeDeviceObject (int driveNumber)
 *	\brief Getter for VirtualVolumeDeviceObjects[driveNumber]
 *	\param[in] int driveNumber
 *	\return PDEVICE_OBJECT VirtualVolumeDeviceObjects[driveNumber]
 *			NULL else
 *
 */
PDEVICE_OBJECT GetVirtualVolumeDeviceObject (int driveNumber)
{
	if (driveNumber < MIN_MOUNTED_VOLUME_DRIVE_NUMBER || driveNumber > MAX_MOUNTED_VOLUME_DRIVE_NUMBER)
		return NULL;

	return VirtualVolumeDeviceObjects[driveNumber];
}


/**
 *
 *	\fn NTSTATUS GSTDispatchQueueIRP (PDEVICE_OBJECT DeviceObject, PIRP Irp)
 *	\brief GSTDispatchQueueIRP queues any IRP's so that they can be processed later
 *	by the thread -- or in some cases handles them immediately!
 *	\param[in] PIRP Irp
 *	\param[in] PDEVICE_OBJECT DeviceObject
 *	\return NTSTATUS status depending on the irp major function
 *			COMPLETE_IRP (DeviceObject, Irp, STATUS_SUCCESS, 0)
 *			GSTCompleteIrp (Irp, STATUS_INVALID_DEVICE_REQUEST, 0)
 *
 */
NTSTATUS GSTDispatchQueueIRP (PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	PEXTENSION Extension = (PEXTENSION) DeviceObject->DeviceExtension;
	PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation (Irp);
	NTSTATUS ntStatus;

#ifdef _DEBUG
	if (irpSp->MajorFunction == IRP_MJ_DEVICE_CONTROL && (Extension->bRootDevice || Extension->IsVolumeDevice))
	{
		switch (irpSp->Parameters.DeviceIoControl.IoControlCode)
		{
		case GST_IOCTL_GET_MOUNTED_VOLUMES:
		case GST_IOCTL_GET_PASSWORD_CACHE_STATUS:
		case GST_IOCTL_GET_PORTABLE_MODE_STATUS:
		case GST_IOCTL_SET_PORTABLE_MODE_STATUS:
		case GST_IOCTL_OPEN_TEST:
		case GST_IOCTL_GET_RESOLVED_SYMLINK:
		case GST_IOCTL_GET_DEVICE_REFCOUNT:
		case GST_IOCTL_GET_DRIVE_PARTITION_INFO:
		case GST_IOCTL_GET_BOOT_DRIVE_VOLUME_PROPERTIES:
		case GST_IOCTL_GET_BOOT_ENCRYPTION_STATUS:
		case GST_IOCTL_GET_DECOY_SYSTEM_WIPE_STATUS:
		case GST_IOCTL_GET_WARNING_FLAGS:
		case GST_IOCTL_IS_HIDDEN_SYSTEM_RUNNING:
		case IOCTL_DISK_CHECK_VERIFY:
			break;

		default:
			Dump ("%ls (0x%x %d)\n",
				GSTTranslateCode (irpSp->Parameters.DeviceIoControl.IoControlCode),
				(int) (irpSp->Parameters.DeviceIoControl.IoControlCode >> 16),
				(int) ((irpSp->Parameters.DeviceIoControl.IoControlCode & 0x1FFF) >> 2));
		}
	}
#endif

	if (!Extension->bRootDevice)
	{
		// Drive filter IRP
		if (Extension->IsDriveFilterDevice)
			return DriveFilterDispatchIrp (DeviceObject, Irp);

		// Volume filter IRP
		if (Extension->IsVolumeFilterDevice)
			return VolumeFilterDispatchIrp (DeviceObject, Irp);
	}

	switch (irpSp->MajorFunction)
	{
	case IRP_MJ_CLOSE:
	case IRP_MJ_CREATE:
	case IRP_MJ_CLEANUP:
		return COMPLETE_IRP (DeviceObject, Irp, STATUS_SUCCESS, 0);

	case IRP_MJ_SHUTDOWN:
		if (Extension->bRootDevice)
		{
			Dump ("Driver shutting down\n");
			DriverShuttingDown = TRUE;

			if (EncryptionSetupThread)
				while (SendDeviceIoControlRequest (RootDeviceObject, GST_IOCTL_ABORT_BOOT_ENCRYPTION_SETUP, NULL, 0, NULL, 0) == STATUS_INSUFFICIENT_RESOURCES);

			if (DecoySystemWipeThread)
				while (SendDeviceIoControlRequest (RootDeviceObject, GST_IOCTL_ABORT_DECOY_SYSTEM_WIPE, NULL, 0, NULL, 0) == STATUS_INSUFFICIENT_RESOURCES);

			OnShutdownPending();
		}

		return COMPLETE_IRP (DeviceObject, Irp, STATUS_SUCCESS, 0);

	case IRP_MJ_FLUSH_BUFFERS:
	case IRP_MJ_READ:
	case IRP_MJ_WRITE:
	case IRP_MJ_DEVICE_CONTROL:

		if (Extension->bRootDevice)
		{
			if (irpSp->MajorFunction == IRP_MJ_DEVICE_CONTROL)
			{
				NTSTATUS status = KeWaitForMutexObject (&RootDeviceControlMutex, Executive, KernelMode, FALSE, NULL);
				if (!NT_SUCCESS (status))
					return status;

				status = ProcessMainDeviceControlIrp (DeviceObject, Extension, Irp);

				KeReleaseMutex (&RootDeviceControlMutex, FALSE);
				return status;
			}
			break;
		}

		if (Extension->bShuttingDown)
		{
			Dump ("Device %d shutting down: STATUS_DELETE_PENDING\n", Extension->nDosDriveNo);
			return GSTCompleteDiskIrp (Irp, STATUS_DELETE_PENDING, 0);
		}

		if (Extension->bRemovable
			&& (DeviceObject->Flags & DO_VERIFY_VOLUME)
			&& !(irpSp->Flags & SL_OVERRIDE_VERIFY_VOLUME)
			&& irpSp->MajorFunction != IRP_MJ_FLUSH_BUFFERS)
		{
			Dump ("Removable device %d has DO_VERIFY_VOLUME flag: STATUS_DEVICE_NOT_READY\n", Extension->nDosDriveNo);
			return GSTCompleteDiskIrp (Irp, STATUS_DEVICE_NOT_READY, 0);
		}

		switch (irpSp->MajorFunction)
		{
		case IRP_MJ_READ:
		case IRP_MJ_WRITE:
			ntStatus = EncryptedIoQueueAddIrp (&Extension->Queue, Irp);

			if (ntStatus != STATUS_PENDING)
				GSTCompleteDiskIrp (Irp, ntStatus, 0);

			return ntStatus;

		case IRP_MJ_DEVICE_CONTROL:
			ntStatus = IoAcquireRemoveLock (&Extension->Queue.RemoveLock, Irp);
			if (!NT_SUCCESS (ntStatus))
				return GSTCompleteIrp (Irp, ntStatus, 0);

			IoMarkIrpPending (Irp);
			
			ExInterlockedInsertTailList (&Extension->ListEntry, &Irp->Tail.Overlay.ListEntry, &Extension->ListSpinLock);
			KeReleaseSemaphore (&Extension->RequestSemaphore, IO_DISK_INCREMENT, 1, FALSE);
			
			return STATUS_PENDING;

		case IRP_MJ_FLUSH_BUFFERS:
			return GSTCompleteDiskIrp (Irp, STATUS_SUCCESS, 0);
		}

		break;

	case IRP_MJ_PNP:
		if (!Extension->bRootDevice
			&& Extension->IsVolumeDevice
			&& irpSp->MinorFunction == IRP_MN_DEVICE_USAGE_NOTIFICATION
			&& irpSp->Parameters.UsageNotification.Type == DeviceUsageTypePaging
			&& irpSp->Parameters.UsageNotification.InPath)
		{
			PagingFileCreationPrevented = TRUE;
			return GSTCompleteIrp (Irp, STATUS_UNSUCCESSFUL, 0);
		}
		break;
	}

	return GSTCompleteIrp (Irp, STATUS_INVALID_DEVICE_REQUEST, 0);
}

/**
 *
 *	\fn NTSTATUS GSTCreateRootDeviceObject (PDRIVER_OBJECT DriverObject)
 *	\brief Create the root Device Object that will contain 
 *	\param[in] PDRIVER_OBJECT DriverObject
 *	\return NTSTATUS STATUS_SUCCESS if success
 *			ntStatus otherwise
 *
 */
NTSTATUS GSTCreateRootDeviceObject (PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING Win32NameString, ntUnicodeString;
	WCHAR dosname[32], ntname[32];
	PDEVICE_OBJECT DeviceObject;
	NTSTATUS ntStatus;
	BOOL *bRootExtension;

	Dump ("GSTCreateRootDeviceObject BEGIN\n");
	ASSERT (KeGetCurrentIrql() == PASSIVE_LEVEL);

	wcscpy (dosname, (LPWSTR) DOS_ROOT_PREFIX);
	wcscpy (ntname, (LPWSTR) NT_ROOT_PREFIX);
	RtlInitUnicodeString (&ntUnicodeString, ntname);
	RtlInitUnicodeString (&Win32NameString, dosname);

	Dump ("Creating root device nt=%ls dos=%ls\n", ntname, dosname);
	
	ntStatus = IoCreateDevice (
					  DriverObject,
					  sizeof (BOOL),
					  &ntUnicodeString,
					  FILE_DEVICE_UNKNOWN,
					  FILE_DEVICE_SECURE_OPEN,
					  FALSE,
					  &DeviceObject);

	if (!NT_SUCCESS (ntStatus))
	{
		Dump ("GSTCreateRootDeviceObject NTSTATUS = 0x%08x END\n", ntStatus);
		return ntStatus;/* Failed to create DeviceObject */
	}

	DeviceObject->Flags |= DO_DIRECT_IO;
	DeviceObject->AlignmentRequirement = FILE_WORD_ALIGNMENT;

	/* Setup the device extension */
	bRootExtension = (BOOL *) DeviceObject->DeviceExtension;
	*bRootExtension = TRUE;

	KeInitializeMutex (&RootDeviceControlMutex, 0);

	ntStatus = IoCreateSymbolicLink (&Win32NameString, &ntUnicodeString);

	if (!NT_SUCCESS (ntStatus))
	{
		Dump ("GSTCreateRootDeviceObject NTSTATUS = 0x%08x END\n", ntStatus);
		IoDeleteDevice (DeviceObject);
		return ntStatus;
	}

	IoRegisterShutdownNotification (DeviceObject);
	RootDeviceObject = DeviceObject;

	Dump ("GSTCreateRootDeviceObject STATUS_SUCCESS END\n");
	return STATUS_SUCCESS;
}


/**
 *
 *	\fn NTSTATUS GSTCreateDeviceObject (PDRIVER_OBJECT DriverObject, PDEVICE_OBJECT * ppDeviceObject, MOUNT_STRUCT * mount)
 *	\brief Create device object and return success
 *	\param[in] PDRIVER_OBJECT DriverObject
 *	\param[in] PDEVICE_OBJECT * ppDeviceObject
 *	\param[in] MOUNT_STRUCT * mount
 *	\return NTSTATUS STATUS_SUCCESS if success
 *			ntStatus otherwise
 *
 */
NTSTATUS GSTCreateDeviceObject (PDRIVER_OBJECT DriverObject,
		       PDEVICE_OBJECT * ppDeviceObject,
		       MOUNT_STRUCT * mount)
{
	UNICODE_STRING Win32NameString, ntUnicodeString;
	WCHAR dosname[32], ntname[32];
	PEXTENSION Extension;
	NTSTATUS ntStatus;
	ULONG devChars = 0;

	Dump ("GSTCreateDeviceObject BEGIN\n");
	ASSERT (KeGetCurrentIrql() == PASSIVE_LEVEL);

	GSTGetDosNameFromNumber (dosname, mount->nDosDriveNo);
	GSTGetNTNameFromNumber (ntname, mount->nDosDriveNo);
	RtlInitUnicodeString (&ntUnicodeString, ntname);
	RtlInitUnicodeString (&Win32NameString, dosname);

	devChars = FILE_DEVICE_SECURE_OPEN;
	devChars |= mount->bMountReadOnly ? FILE_READ_ONLY_DEVICE : 0;
	devChars |= mount->bMountRemovable ? FILE_REMOVABLE_MEDIA : 0;

	Dump ("Creating device nt=%ls dos=%ls\n", ntname, dosname);

	ntStatus = IoCreateDevice (
					  DriverObject,			/* Our Driver Object */
					  sizeof (EXTENSION),	/* Size of state information */
					  &ntUnicodeString,		/* Device name "\Device\Name" */
					  FILE_DEVICE_DISK,		/* Device type */
					  devChars,				/* Device characteristics */
					  FALSE,				/* Exclusive device */
					  ppDeviceObject);		/* Returned ptr to Device Object */

	if (!NT_SUCCESS (ntStatus))
	{
		Dump ("GSTCreateDeviceObject NTSTATUS = 0x%08x END\n", ntStatus);
		return ntStatus;/* Failed to create DeviceObject */
	}
	/* Initialize device object and extension. */

	(*ppDeviceObject)->Flags |= DO_DIRECT_IO;
	(*ppDeviceObject)->StackSize += 6;		// Reduce occurrence of NO_MORE_IRP_STACK_LOCATIONS bug check caused by buggy drivers

	/* Setup the device extension */
	Extension = (PEXTENSION) (*ppDeviceObject)->DeviceExtension;
	memset (Extension, 0, sizeof (EXTENSION));

	Extension->IsVolumeDevice = TRUE;
	Extension->nDosDriveNo = mount->nDosDriveNo;
	Extension->bRemovable = mount->bMountRemovable;
	Extension->PartitionInInactiveSysEncScope = mount->bPartitionInInactiveSysEncScope;
	Extension->SystemFavorite = mount->SystemFavorite;

	KeInitializeEvent (&Extension->keCreateEvent, SynchronizationEvent, FALSE);
	KeInitializeSemaphore (&Extension->RequestSemaphore, 0L, MAXLONG);
	KeInitializeSpinLock (&Extension->ListSpinLock);
	InitializeListHead (&Extension->ListEntry);
	IoInitializeRemoveLock (&Extension->Queue.RemoveLock, 'LRCT', 0, 0);

	VirtualVolumeDeviceObjects[mount->nDosDriveNo] = *ppDeviceObject;

	Dump ("GSTCreateDeviceObject STATUS_SUCCESS END\n");

	return STATUS_SUCCESS;
}


/**
 *
 *	\fn BOOL RootDeviceControlMutexAcquireNoWait ()
 *	\brief Control the mutex without waiting (timeout = 0)
 *	\return BOOL NT_SUCCESS (status) && status != STATUS_TIMEOUT
 *
 */
BOOL RootDeviceControlMutexAcquireNoWait ()
{
	NTSTATUS status;
	LARGE_INTEGER timeout;
	timeout.QuadPart = 0;

	status = KeWaitForMutexObject (&RootDeviceControlMutex, Executive, KernelMode, FALSE, &timeout);
	return NT_SUCCESS (status) && status != STATUS_TIMEOUT;
}


/**
 *
 *	\fn void RootDeviceControlMutexRelease ()
 *	\brief Release the control on the mutex
 *	\return void
 *
 */
void RootDeviceControlMutexRelease ()
{
	KeReleaseMutex (&RootDeviceControlMutex, FALSE);
}


/**
 *
 *	\fn NTSTATUS ProcessVolumeDeviceControlIrp (PDEVICE_OBJECT DeviceObject, PEXTENSION Extension, PIRP Irp)
 *	\brief Depending on the control code of the irp process the associated actions.
 *	\param[in] PIRP Irp
 *	\param[in] PEXTENSION Extension
 *	\param[in] PDEVICE_OBJECT DeviceObject
 *	\return NTSTATUS GSTCompleteDiskIrp (Irp, Irp->IoStatus.Status, Irp->IoStatus.Information)
 *
 */
NTSTATUS ProcessVolumeDeviceControlIrp (PDEVICE_OBJECT DeviceObject, PEXTENSION Extension, PIRP Irp)
{
	PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation (Irp);

	switch (irpSp->Parameters.DeviceIoControl.IoControlCode)
	{

	case IOCTL_MOUNTDEV_QUERY_DEVICE_NAME:
		if (!ValidateIOBufferSize (Irp, sizeof (MOUNTDEV_NAME), ValidateOutput))
		{
			Irp->IoStatus.Information = sizeof (MOUNTDEV_NAME);
			Irp->IoStatus.Status = STATUS_BUFFER_OVERFLOW;
		}
		else
		{
			ULONG outLength;
			UNICODE_STRING ntUnicodeString;
			WCHAR ntName[256];
			PMOUNTDEV_NAME outputBuffer = (PMOUNTDEV_NAME) Irp->AssociatedIrp.SystemBuffer;

			GSTGetNTNameFromNumber (ntName, Extension->nDosDriveNo);
			RtlInitUnicodeString (&ntUnicodeString, ntName);

			outputBuffer->NameLength = ntUnicodeString.Length;
			outLength = ntUnicodeString.Length + sizeof(USHORT);

			if (irpSp->Parameters.DeviceIoControl.OutputBufferLength < outLength)
			{
				Irp->IoStatus.Information = sizeof (MOUNTDEV_NAME);
				Irp->IoStatus.Status = STATUS_BUFFER_OVERFLOW;

				break;
			}

			RtlCopyMemory ((PCHAR)outputBuffer->Name,ntUnicodeString.Buffer, ntUnicodeString.Length);

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = outLength;

			Dump ("name = %ls\n",ntName);
		}
		break;

	case IOCTL_MOUNTDEV_QUERY_UNIQUE_ID:
		if (!ValidateIOBufferSize (Irp, sizeof (MOUNTDEV_UNIQUE_ID), ValidateOutput))
		{
			Irp->IoStatus.Information = sizeof (MOUNTDEV_UNIQUE_ID);
			Irp->IoStatus.Status = STATUS_BUFFER_OVERFLOW;
		}
		else
		{
			ULONG outLength;
			UCHAR volId[128], tmp[] = { 0,0 };
			PMOUNTDEV_UNIQUE_ID outputBuffer = (PMOUNTDEV_UNIQUE_ID) Irp->AssociatedIrp.SystemBuffer;

			strcpy (volId, GST_UNIQUE_ID_PREFIX); 
			tmp[0] = 'A' + (UCHAR) Extension->nDosDriveNo;
			strcat (volId, tmp);
			
			outputBuffer->UniqueIdLength = (USHORT) strlen (volId);
			outLength = (ULONG) (strlen (volId) + sizeof (USHORT));

			if (irpSp->Parameters.DeviceIoControl.OutputBufferLength < outLength)
			{
				Irp->IoStatus.Information = sizeof (MOUNTDEV_UNIQUE_ID);
				Irp->IoStatus.Status = STATUS_BUFFER_OVERFLOW;
				break;
			}

			RtlCopyMemory ((PCHAR)outputBuffer->UniqueId, volId, strlen (volId));

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = outLength;

			Dump ("id = %s\n",volId);
		}
		break;

	case IOCTL_MOUNTDEV_QUERY_SUGGESTED_LINK_NAME:
		{
			ULONG outLength;
			UNICODE_STRING ntUnicodeString;
			WCHAR ntName[256];
			PMOUNTDEV_SUGGESTED_LINK_NAME outputBuffer = (PMOUNTDEV_SUGGESTED_LINK_NAME) Irp->AssociatedIrp.SystemBuffer;

			if (!ValidateIOBufferSize (Irp, sizeof (MOUNTDEV_SUGGESTED_LINK_NAME), ValidateOutput))
			{
				Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
				Irp->IoStatus.Information = 0;
				break; 
			}

			GSTGetDosNameFromNumber (ntName, Extension->nDosDriveNo);
			RtlInitUnicodeString (&ntUnicodeString, ntName);

			outLength = FIELD_OFFSET(MOUNTDEV_SUGGESTED_LINK_NAME,Name) + ntUnicodeString.Length;

			outputBuffer->UseOnlyIfThereAreNoOtherLinks = FALSE;
			outputBuffer->NameLength = ntUnicodeString.Length;

			if(irpSp->Parameters.DeviceIoControl.OutputBufferLength < outLength)
			{
				Irp->IoStatus.Information = sizeof (MOUNTDEV_SUGGESTED_LINK_NAME);
				Irp->IoStatus.Status = STATUS_BUFFER_OVERFLOW;
				break;
			}

			RtlCopyMemory ((PCHAR)outputBuffer->Name,ntUnicodeString.Buffer, ntUnicodeString.Length);
		
			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = outLength;

			Dump ("link = %ls\n",ntName);
		}
		break;

	case IOCTL_DISK_GET_MEDIA_TYPES:
	case IOCTL_DISK_GET_DRIVE_GEOMETRY:
		/* Return the drive geometry for the disk.  Note that we
		   return values which were made up to suit the disk size.  */
		if (ValidateIOBufferSize (Irp, sizeof (DISK_GEOMETRY), ValidateOutput))
		{
			PDISK_GEOMETRY outputBuffer = (PDISK_GEOMETRY)
			Irp->AssociatedIrp.SystemBuffer;

			outputBuffer->MediaType = Extension->bRemovable ? RemovableMedia : FixedMedia;
			outputBuffer->Cylinders.QuadPart = Extension->NumberOfCylinders;
			outputBuffer->TracksPerCylinder = Extension->TracksPerCylinder;
			outputBuffer->SectorsPerTrack = Extension->SectorsPerTrack;
			outputBuffer->BytesPerSector = Extension->BytesPerSector;
			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = sizeof (DISK_GEOMETRY);
		}
		break;

	case IOCTL_DISK_GET_PARTITION_INFO:
		if (ValidateIOBufferSize (Irp, sizeof (PARTITION_INFORMATION), ValidateOutput))
		{
			PPARTITION_INFORMATION outputBuffer = (PPARTITION_INFORMATION)
			Irp->AssociatedIrp.SystemBuffer;

			outputBuffer->PartitionType = Extension->PartitionType;
			outputBuffer->BootIndicator = FALSE;
			outputBuffer->RecognizedPartition = TRUE;
			outputBuffer->RewritePartition = FALSE;
			outputBuffer->StartingOffset.QuadPart = Extension->BytesPerSector;
			outputBuffer->PartitionLength.QuadPart= Extension->DiskLength;
			outputBuffer->HiddenSectors = 0;
			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = sizeof (PARTITION_INFORMATION);
		}
		break;

	case IOCTL_DISK_GET_PARTITION_INFO_EX:
		if (ValidateIOBufferSize (Irp, sizeof (PARTITION_INFORMATION_EX), ValidateOutput))
		{
			PPARTITION_INFORMATION_EX outputBuffer = (PPARTITION_INFORMATION_EX) Irp->AssociatedIrp.SystemBuffer;

			outputBuffer->PartitionStyle = PARTITION_STYLE_MBR;
			outputBuffer->RewritePartition = FALSE;
			outputBuffer->StartingOffset.QuadPart = Extension->BytesPerSector;
			outputBuffer->PartitionLength.QuadPart= Extension->DiskLength;
			outputBuffer->Mbr.PartitionType = Extension->PartitionType;
			outputBuffer->Mbr.BootIndicator = FALSE;
			outputBuffer->Mbr.RecognizedPartition = TRUE;
			outputBuffer->Mbr.HiddenSectors = 0;
			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = sizeof (PARTITION_INFORMATION_EX);
		}
		break;

	case IOCTL_DISK_GET_DRIVE_LAYOUT:
		if (ValidateIOBufferSize (Irp, sizeof (DRIVE_LAYOUT_INFORMATION), ValidateOutput))
		{
			PDRIVE_LAYOUT_INFORMATION outputBuffer = (PDRIVE_LAYOUT_INFORMATION)
			Irp->AssociatedIrp.SystemBuffer;

			outputBuffer->PartitionCount = 1;
			outputBuffer->Signature = 0;

			outputBuffer->PartitionEntry->PartitionType = Extension->PartitionType;
			outputBuffer->PartitionEntry->BootIndicator = FALSE;
			outputBuffer->PartitionEntry->RecognizedPartition = TRUE;
			outputBuffer->PartitionEntry->RewritePartition = FALSE;
			outputBuffer->PartitionEntry->StartingOffset.QuadPart = Extension->BytesPerSector;
			outputBuffer->PartitionEntry->PartitionLength.QuadPart = Extension->DiskLength;
			outputBuffer->PartitionEntry->HiddenSectors = 0;

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = sizeof (PARTITION_INFORMATION);
		}
		break;

	case IOCTL_DISK_GET_LENGTH_INFO:
		if (!ValidateIOBufferSize (Irp, sizeof (GET_LENGTH_INFORMATION), ValidateOutput))
		{
			Irp->IoStatus.Status = STATUS_BUFFER_OVERFLOW;
			Irp->IoStatus.Information = sizeof (GET_LENGTH_INFORMATION);
		}
		else
		{
			PGET_LENGTH_INFORMATION outputBuffer = (PGET_LENGTH_INFORMATION) Irp->AssociatedIrp.SystemBuffer;

			outputBuffer->Length.QuadPart = Extension->DiskLength;
			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = sizeof (GET_LENGTH_INFORMATION);
		}
		break;

	case IOCTL_DISK_VERIFY:
		if (ValidateIOBufferSize (Irp, sizeof (VERIFY_INFORMATION), ValidateInput))
		{
			PVERIFY_INFORMATION pVerifyInformation;
			pVerifyInformation = (PVERIFY_INFORMATION) Irp->AssociatedIrp.SystemBuffer;

			if (pVerifyInformation->StartingOffset.QuadPart + pVerifyInformation->Length > Extension->DiskLength)
				Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			else
			{
				IO_STATUS_BLOCK ioStatus;
				PVOID buffer = GSTalloc (max (pVerifyInformation->Length, PAGE_SIZE));
				
				if (!buffer)
				{
					Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
				}
				else
				{
					LARGE_INTEGER offset = pVerifyInformation->StartingOffset;
					offset.QuadPart += Extension->cryptoInfo->hiddenVolume ? Extension->cryptoInfo->hiddenVolumeOffset : Extension->cryptoInfo->volDataAreaOffset;

					Irp->IoStatus.Status = ZwReadFile (Extension->hDeviceFile, NULL, NULL, NULL, &ioStatus, buffer, pVerifyInformation->Length, &offset, NULL);
					GSTfree (buffer);

					if (NT_SUCCESS (Irp->IoStatus.Status) && ioStatus.Information != pVerifyInformation->Length)
						Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
				}
			}

			Irp->IoStatus.Information = 0;
		}
		break;

	case IOCTL_DISK_CHECK_VERIFY:
	case IOCTL_STORAGE_CHECK_VERIFY:
		{
			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = 0;

			if (irpSp->Parameters.DeviceIoControl.OutputBufferLength >= sizeof (ULONG))
			{
				*((ULONG *) Irp->AssociatedIrp.SystemBuffer) = 0;
				Irp->IoStatus.Information = sizeof (ULONG);
			}
		}
		break;

	case IOCTL_DISK_IS_WRITABLE:
		{
			if (Extension->bReadOnly)
				Irp->IoStatus.Status = STATUS_MEDIA_WRITE_PROTECTED;
			else
				Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = 0;

		}
		break;
		
	case IOCTL_VOLUME_ONLINE:
		Irp->IoStatus.Status = STATUS_SUCCESS;
		Irp->IoStatus.Information = 0;
		break;

	case IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS:

		// Vista's filesystem defragmenter fails if IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS does not succeed.
		if (!(OsMajorVersion == 6 && OsMinorVersion == 0))
		{
			Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
			Irp->IoStatus.Information = 0;
		}
		else if (ValidateIOBufferSize (Irp, sizeof (VOLUME_DISK_EXTENTS), ValidateOutput))
		{
			VOLUME_DISK_EXTENTS *extents = (VOLUME_DISK_EXTENTS *) Irp->AssociatedIrp.SystemBuffer;

			// No extent data can be returned as this is not a physical drive.
			memset (extents, 0, sizeof (*extents));
			extents->NumberOfDiskExtents = 0;

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = sizeof (*extents);
		}
		break;

	default:
		return GSTCompleteIrp (Irp, STATUS_INVALID_DEVICE_REQUEST, 0);
	}

#ifdef DEBUG
	if (!NT_SUCCESS (Irp->IoStatus.Status))
	{
		Dump ("IOCTL error 0x%08x (0x%x %d)\n",
			Irp->IoStatus.Status,
			(int) (irpSp->Parameters.DeviceIoControl.IoControlCode >> 16),
			(int) ((irpSp->Parameters.DeviceIoControl.IoControlCode & 0x1FFF) >> 2));
	}
#endif

	return GSTCompleteDiskIrp (Irp, Irp->IoStatus.Status, Irp->IoStatus.Information);
}


/**
 *
 *	\fn NTSTATUS ProcessMainDeviceControlIrp (PDEVICE_OBJECT DeviceObject, PEXTENSION Extension, PIRP Irp)
 *	\brief Process the main actions for the control irp code.
 *	\param[in] PIRP Irp
 *	\param[in] PEXTENSION Extension
 *	\param[in] PDEVICE_OBJECT DeviceObject
 *	\return NTSTATUS GSTCompleteIrp (Irp, Irp->IoStatus.Status, Irp->IoStatus.Information)
 *
 */
NTSTATUS ProcessMainDeviceControlIrp (PDEVICE_OBJECT DeviceObject, PEXTENSION Extension, PIRP Irp)
{
	PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation (Irp);
	NTSTATUS ntStatus;

	switch (irpSp->Parameters.DeviceIoControl.IoControlCode)
	{
	case GST_IOCTL_GET_DRIVER_VERSION:
	case GST_IOCTL_LEGACY_GET_DRIVER_VERSION:
		if (ValidateIOBufferSize (Irp, sizeof (LONG), ValidateOutput))
		{
			LONG tmp = VERSION_NUM;
			memcpy (Irp->AssociatedIrp.SystemBuffer, &tmp, 4);
			Irp->IoStatus.Information = sizeof (LONG);
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		break;

	case GST_IOCTL_GET_DEVICE_REFCOUNT:
		if (ValidateIOBufferSize (Irp, sizeof (int), ValidateOutput))
		{
			*(int *) Irp->AssociatedIrp.SystemBuffer = DeviceObject->ReferenceCount;
			Irp->IoStatus.Information = sizeof (int);
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		break;

	case GST_IOCTL_IS_DRIVER_UNLOAD_DISABLED:
		if (ValidateIOBufferSize (Irp, sizeof (int), ValidateOutput))
		{
			LONG deviceObjectCount = 0;

			*(int *) Irp->AssociatedIrp.SystemBuffer = DriverUnloadDisabled;

			if (IoEnumerateDeviceObjectList (GSTDriverObject, NULL, 0, &deviceObjectCount) == STATUS_BUFFER_TOO_SMALL && deviceObjectCount > 1)
				*(int *) Irp->AssociatedIrp.SystemBuffer = TRUE;

			Irp->IoStatus.Information = sizeof (int);
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		break;

	case GST_IOCTL_IS_ANY_VOLUME_MOUNTED:
		if (ValidateIOBufferSize (Irp, sizeof (int), ValidateOutput))
		{
			int drive;
			*(int *) Irp->AssociatedIrp.SystemBuffer = 0;

			for (drive = MIN_MOUNTED_VOLUME_DRIVE_NUMBER; drive <= MAX_MOUNTED_VOLUME_DRIVE_NUMBER; ++drive)
			{
				if (GetVirtualVolumeDeviceObject (drive))
				{
					*(int *) Irp->AssociatedIrp.SystemBuffer = 1;
					break;
				}
			}

			if (IsBootDriveMounted())
				*(int *) Irp->AssociatedIrp.SystemBuffer = 1;

			Irp->IoStatus.Information = sizeof (int);
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		break;

	case GST_IOCTL_OPEN_TEST:
		{
			OPEN_TEST_STRUCT *opentest = (OPEN_TEST_STRUCT *) Irp->AssociatedIrp.SystemBuffer;
			OBJECT_ATTRIBUTES ObjectAttributes;
			HANDLE NtFileHandle;
			UNICODE_STRING FullFileName;
			IO_STATUS_BLOCK IoStatus;
			LARGE_INTEGER offset;
			ACCESS_MASK access = FILE_READ_ATTRIBUTES;

			if (!ValidateIOBufferSize (Irp, sizeof (OPEN_TEST_STRUCT), ValidateInputOutput))
				break;

			EnsureNullTerminatedString (opentest->wszFileName, sizeof (opentest->wszFileName));
			RtlInitUnicodeString (&FullFileName, opentest->wszFileName);

			InitializeObjectAttributes (&ObjectAttributes, &FullFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

			if (opentest->bDetectGSTBootLoader || opentest->DetectFilesystem)
				access |= FILE_READ_DATA;

			ntStatus = ZwCreateFile (&NtFileHandle,
						 SYNCHRONIZE | access, &ObjectAttributes, &IoStatus, NULL,
						 0, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

			if (NT_SUCCESS (ntStatus))
			{
				opentest->GSTBootLoaderDetected = FALSE;
				opentest->FilesystemDetected = FALSE;

				if (opentest->bDetectGSTBootLoader || opentest->DetectFilesystem)
				{
					byte *readBuffer = GSTalloc (GST_MAX_VOLUME_SECTOR_SIZE);
					if (!readBuffer)
					{
						ntStatus = STATUS_INSUFFICIENT_RESOURCES;
					}
					else
					{
						// Determine if the first sector contains a portion of the GostCrypt Boot Loader

						offset.QuadPart = 0;

						ntStatus = ZwReadFile (NtFileHandle,
							NULL,
							NULL,
							NULL,
							&IoStatus,
							readBuffer,
							GST_MAX_VOLUME_SECTOR_SIZE,
							&offset,
							NULL);

						if (NT_SUCCESS (ntStatus))
						{
							size_t i;

							if (opentest->bDetectGSTBootLoader && IoStatus.Information >= GST_SECTOR_SIZE_BIOS)
							{
								// Search for the string "GostCrypt"
								for (i = 0; i < GST_SECTOR_SIZE_BIOS - strlen (GST_APP_NAME); ++i)
								{
									if (memcmp (readBuffer + i, GST_APP_NAME, strlen (GST_APP_NAME)) == 0)
									{
										opentest->GSTBootLoaderDetected = TRUE;
										break;
									}
								}
							}

							if (opentest->DetectFilesystem && IoStatus.Information >= sizeof (int64))
							{
								switch (BE64 (*(uint64 *) readBuffer))
								{
								case 0xEB52904E54465320: // NTFS
								case 0xEB3C904D53444F53: // FAT16
								case 0xEB58904D53444F53: // FAT32
								case 0xEB76904558464154: // exFAT

									opentest->FilesystemDetected = TRUE;
									break;
								}
							}
						}

						GSTfree (readBuffer);
					}
				}

				ZwClose (NtFileHandle);
				Dump ("Open test on file %ls success.\n", opentest->wszFileName);
			}
			else
			{
#if 0
				Dump ("Open test on file %ls failed NTSTATUS 0x%08x\n", opentest->wszFileName, ntStatus);
#endif
			}

			Irp->IoStatus.Information = NT_SUCCESS (ntStatus) ? sizeof (OPEN_TEST_STRUCT) : 0;
			Irp->IoStatus.Status = ntStatus;
		}
		break;

	case GST_IOCTL_GET_SYSTEM_DRIVE_CONFIG:
		{
			GetSystemDriveConfigurationRequest *request = (GetSystemDriveConfigurationRequest *) Irp->AssociatedIrp.SystemBuffer;
			OBJECT_ATTRIBUTES ObjectAttributes;
			HANDLE NtFileHandle;
			UNICODE_STRING FullFileName;
			IO_STATUS_BLOCK IoStatus;
			LARGE_INTEGER offset;
			byte readBuffer [GST_SECTOR_SIZE_BIOS];

			if (!ValidateIOBufferSize (Irp, sizeof (GetSystemDriveConfigurationRequest), ValidateInputOutput))
				break;

			EnsureNullTerminatedString (request->DevicePath, sizeof (request->DevicePath));
			RtlInitUnicodeString (&FullFileName, request->DevicePath);

			InitializeObjectAttributes (&ObjectAttributes, &FullFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

			ntStatus = ZwCreateFile (&NtFileHandle,
				SYNCHRONIZE | GENERIC_READ, &ObjectAttributes, &IoStatus, NULL,
				FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT | FILE_RANDOM_ACCESS, NULL, 0);

			if (NT_SUCCESS (ntStatus))
			{
				// Determine if the first sector contains a portion of the GostCrypt Boot Loader
				offset.QuadPart = 0;	// MBR

				ntStatus = ZwReadFile (NtFileHandle,
					NULL,
					NULL,
					NULL,
					&IoStatus,
					readBuffer,
					sizeof(readBuffer),
					&offset,
					NULL);

				if (NT_SUCCESS (ntStatus))
				{
					size_t i;

					// Check for dynamic drive
					request->DriveIsDynamic = FALSE;

					if (readBuffer[510] == 0x55 && readBuffer[511] == 0xaa)
					{
						int i;
						for (i = 0; i < 4; ++i)
						{
							if (readBuffer[446 + i * 16 + 4] == PARTITION_LDM)
							{
								request->DriveIsDynamic = TRUE;
								break;
							}
						}
					}

					request->BootLoaderVersion = 0;
					request->Configuration = 0;
					request->UserConfiguration = 0;
					request->CustomUserMessage[0] = 0;

					// Search for the string "GostCrypt"
					for (i = 0; i < sizeof (readBuffer) - strlen (GST_APP_NAME); ++i)
					{
						if (memcmp (readBuffer + i, GST_APP_NAME, strlen (GST_APP_NAME)) == 0)
						{
							request->BootLoaderVersion = BE16 (*(uint16 *) (readBuffer + GST_BOOT_SECTOR_VERSION_OFFSET));
							request->Configuration = readBuffer[GST_BOOT_SECTOR_CONFIG_OFFSET];

							if (request->BootLoaderVersion != 0 && request->BootLoaderVersion <= VERSION_NUM)
							{
								request->UserConfiguration = readBuffer[GST_BOOT_SECTOR_USER_CONFIG_OFFSET];
								memcpy (request->CustomUserMessage, readBuffer + GST_BOOT_SECTOR_USER_MESSAGE_OFFSET, GST_BOOT_SECTOR_USER_MESSAGE_MAX_LENGTH);
							}
							break;
						}
					}

					Irp->IoStatus.Status = STATUS_SUCCESS;
					Irp->IoStatus.Information = sizeof (*request);
				}
				else
				{
					Irp->IoStatus.Status = ntStatus;
					Irp->IoStatus.Information = 0;
				}

				ZwClose (NtFileHandle);

			}
			else
			{
				Irp->IoStatus.Status = ntStatus;
				Irp->IoStatus.Information = 0;
			}
		}
		break;

	case GST_IOCTL_WIPE_PASSWORD_CACHE:
		WipeCache ();

		Irp->IoStatus.Status = STATUS_SUCCESS;
		Irp->IoStatus.Information = 0;
		break;

	case GST_IOCTL_GET_PASSWORD_CACHE_STATUS:
		Irp->IoStatus.Status = cacheEmpty ? STATUS_PIPE_EMPTY : STATUS_SUCCESS;
		Irp->IoStatus.Information = 0;
		break;

	case GST_IOCTL_SET_PORTABLE_MODE_STATUS:
		if (!UserCanAccessDriveDevice())
		{
			Irp->IoStatus.Status = STATUS_ACCESS_DENIED;
			Irp->IoStatus.Information = 0;
		}
		else
		{
			PortableMode = TRUE;
			Dump ("Setting portable mode\n");
		}
		break;

	case GST_IOCTL_GET_PORTABLE_MODE_STATUS:
		Irp->IoStatus.Status = PortableMode ? STATUS_SUCCESS : STATUS_PIPE_EMPTY;
		Irp->IoStatus.Information = 0;
		break;

	case GST_IOCTL_GET_MOUNTED_VOLUMES:
		Dump("IOCTL GetMountedVolume received");
		if (ValidateIOBufferSize (Irp, sizeof (MOUNT_LIST_STRUCT), ValidateOutput))
		{
			MOUNT_LIST_STRUCT *list = (MOUNT_LIST_STRUCT *) Irp->AssociatedIrp.SystemBuffer;
			PDEVICE_OBJECT ListDevice;
			int drive;

			list->ulMountedDrives = 0;

			for (drive = MIN_MOUNTED_VOLUME_DRIVE_NUMBER; drive <= MAX_MOUNTED_VOLUME_DRIVE_NUMBER; ++drive)
			{
				PEXTENSION ListExtension;
				Dump("Getting device object for drive %d .", drive );
				ListDevice = GetVirtualVolumeDeviceObject (drive);
				if (!ListDevice)
					continue;
				
				ListExtension = (PEXTENSION) ListDevice->DeviceExtension;
				Dump("Checking if VolumeIsAccessibleForUSer.");
				if (IsVolumeAccessibleByCurrentUser (ListExtension))
				{
					list->ulMountedDrives |= (1 << ListExtension->nDosDriveNo);
					Dump("Check 1..");
					wcscpy (list->wszVolume[ListExtension->nDosDriveNo], ListExtension->wszVolume);
					Dump("2..");
					list->diskLength[ListExtension->nDosDriveNo] = ListExtension->DiskLength;
					Dump("3..");
					list->ea[ListExtension->nDosDriveNo] = ListExtension->cryptoInfo->ea;
					Dump("4..");
					if (ListExtension->cryptoInfo->hiddenVolume)
						list->volumeType[ListExtension->nDosDriveNo] = PROP_VOL_TYPE_HIDDEN;	// Hidden volume
					else if (ListExtension->cryptoInfo->bHiddenVolProtectionAction)
						list->volumeType[ListExtension->nDosDriveNo] = PROP_VOL_TYPE_OUTER_VOL_WRITE_PREVENTED;	// Normal/outer volume (hidden volume protected AND write already prevented)
					else if (ListExtension->cryptoInfo->bProtectHiddenVolume)
						list->volumeType[ListExtension->nDosDriveNo] = PROP_VOL_TYPE_OUTER;	// Normal/outer volume (hidden volume protected)
					else
						list->volumeType[ListExtension->nDosDriveNo] = PROP_VOL_TYPE_NORMAL;	// Normal volume
					Dump("5. ok.\n");
				}
			}

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = sizeof (MOUNT_LIST_STRUCT);
		}
		break;

	case GST_IOCTL_LEGACY_GET_MOUNTED_VOLUMES:
		if (ValidateIOBufferSize (Irp, sizeof (uint32), ValidateOutput))
		{
			// Prevent the user from downgrading to versions lower than 5.0 by faking mounted volumes.
			// The user could render the system unbootable by downgrading when boot encryption
			// is active or being set up.

			memset (Irp->AssociatedIrp.SystemBuffer, 0, irpSp->Parameters.DeviceIoControl.OutputBufferLength);
			*(uint32 *) Irp->AssociatedIrp.SystemBuffer = 0xffffFFFF;

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = irpSp->Parameters.DeviceIoControl.OutputBufferLength;
		}
		break;

	case GST_IOCTL_GET_VOLUME_PROPERTIES:
		if (ValidateIOBufferSize (Irp, sizeof (VOLUME_PROPERTIES_STRUCT), ValidateInputOutput))
		{
			VOLUME_PROPERTIES_STRUCT *prop = (VOLUME_PROPERTIES_STRUCT *) Irp->AssociatedIrp.SystemBuffer;
			PDEVICE_OBJECT ListDevice = GetVirtualVolumeDeviceObject (prop->driveNo);

			Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			Irp->IoStatus.Information = 0;

			if (ListDevice)
			{
				PEXTENSION ListExtension = (PEXTENSION) ListDevice->DeviceExtension;
				if (IsVolumeAccessibleByCurrentUser (ListExtension))
				{
					prop->uniqueId = ListExtension->UniqueVolumeId;
					wcscpy (prop->wszVolume, ListExtension->wszVolume);
					prop->diskLength = ListExtension->DiskLength;
					prop->ea = ListExtension->cryptoInfo->ea;
					prop->mode = ListExtension->cryptoInfo->mode;
					prop->pkcs5 = ListExtension->cryptoInfo->pkcs5;
					prop->pkcs5Iterations = ListExtension->cryptoInfo->noIterations;
#if 0
					prop->volumeCreationTime = ListExtension->cryptoInfo->volume_creation_time;
					prop->headerCreationTime = ListExtension->cryptoInfo->header_creation_time;
#endif
					prop->volumeHeaderFlags = ListExtension->cryptoInfo->HeaderFlags;
					prop->readOnly = ListExtension->bReadOnly;
					prop->removable = ListExtension->bRemovable;
					prop->partitionInInactiveSysEncScope = ListExtension->PartitionInInactiveSysEncScope;
					prop->hiddenVolume = ListExtension->cryptoInfo->hiddenVolume;

					if (ListExtension->cryptoInfo->bProtectHiddenVolume)
						prop->hiddenVolProtection = ListExtension->cryptoInfo->bHiddenVolProtectionAction ? HIDVOL_PROT_STATUS_ACTION_TAKEN : HIDVOL_PROT_STATUS_ACTIVE;
					else
						prop->hiddenVolProtection = HIDVOL_PROT_STATUS_NONE;

					prop->totalBytesRead = ListExtension->Queue.TotalBytesRead;
					prop->totalBytesWritten = ListExtension->Queue.TotalBytesWritten;

					prop->volFormatVersion = ListExtension->cryptoInfo->LegacyVolume ? GST_VOLUME_FORMAT_VERSION_PRE_6_0 : GST_VOLUME_FORMAT_VERSION;

					Irp->IoStatus.Status = STATUS_SUCCESS;
					Irp->IoStatus.Information = sizeof (VOLUME_PROPERTIES_STRUCT);
				}
			}
		}
		break;

	case GST_IOCTL_GET_RESOLVED_SYMLINK:
		if (ValidateIOBufferSize (Irp, sizeof (RESOLVE_SYMLINK_STRUCT), ValidateInputOutput))
		{
			RESOLVE_SYMLINK_STRUCT *resolve = (RESOLVE_SYMLINK_STRUCT *) Irp->AssociatedIrp.SystemBuffer;
			{
				NTSTATUS ntStatus;

				EnsureNullTerminatedString (resolve->symLinkName, sizeof (resolve->symLinkName));

				ntStatus = SymbolicLinkToTarget (resolve->symLinkName,
					resolve->targetName,
					sizeof (resolve->targetName));

				Irp->IoStatus.Information = sizeof (RESOLVE_SYMLINK_STRUCT);
				Irp->IoStatus.Status = ntStatus;
			}
		}
		break;

	case GST_IOCTL_GET_DRIVE_PARTITION_INFO:
		if (ValidateIOBufferSize (Irp, sizeof (DISK_PARTITION_INFO_STRUCT), ValidateInputOutput))
		{
			DISK_PARTITION_INFO_STRUCT *info = (DISK_PARTITION_INFO_STRUCT *) Irp->AssociatedIrp.SystemBuffer;
			{
				PARTITION_INFORMATION_EX pi;
				NTSTATUS ntStatus;

				EnsureNullTerminatedString (info->deviceName, sizeof (info->deviceName));

				ntStatus = GSTDeviceIoControl (info->deviceName, IOCTL_DISK_GET_PARTITION_INFO_EX, NULL, 0, &pi, sizeof (pi));
				if (NT_SUCCESS(ntStatus))
				{
					memset (&info->partInfo, 0, sizeof (info->partInfo));

					info->partInfo.PartitionLength = pi.PartitionLength;
					info->partInfo.PartitionNumber = pi.PartitionNumber;
					info->partInfo.StartingOffset = pi.StartingOffset;

					if (pi.PartitionStyle == PARTITION_STYLE_MBR)
					{
						info->partInfo.PartitionType = pi.Mbr.PartitionType;
						info->partInfo.BootIndicator = pi.Mbr.BootIndicator;
					}

					info->IsGPT = pi.PartitionStyle == PARTITION_STYLE_GPT;
				}
				else
				{
					// Windows 2000 does not support IOCTL_DISK_GET_PARTITION_INFO_EX
					ntStatus = GSTDeviceIoControl (info->deviceName, IOCTL_DISK_GET_PARTITION_INFO, NULL, 0, &info->partInfo, sizeof (info->partInfo));
					info->IsGPT = FALSE;
				}

				if (!NT_SUCCESS (ntStatus))
				{
					GET_LENGTH_INFORMATION lengthInfo;
					ntStatus = GSTDeviceIoControl (info->deviceName, IOCTL_DISK_GET_LENGTH_INFO, NULL, 0, &lengthInfo, sizeof (lengthInfo));

					if (NT_SUCCESS (ntStatus))
					{
						memset (&info->partInfo, 0, sizeof (info->partInfo));
						info->partInfo.PartitionLength = lengthInfo.Length;
					}
				}

				info->IsDynamic = FALSE;

				if (NT_SUCCESS (ntStatus) && OsMajorVersion >= 6)
				{
#					define IOCTL_VOLUME_IS_DYNAMIC CTL_CODE(IOCTL_VOLUME_BASE, 18, METHOD_BUFFERED, FILE_ANY_ACCESS)
					if (!NT_SUCCESS (GSTDeviceIoControl (info->deviceName, IOCTL_VOLUME_IS_DYNAMIC, NULL, 0, &info->IsDynamic, sizeof (info->IsDynamic))))
						info->IsDynamic = FALSE;
				}

				Irp->IoStatus.Information = sizeof (DISK_PARTITION_INFO_STRUCT);
				Irp->IoStatus.Status = ntStatus;
			}
		}
		break;

	case GST_IOCTL_GET_DRIVE_GEOMETRY:
		if (ValidateIOBufferSize (Irp, sizeof (DISK_GEOMETRY_STRUCT), ValidateInputOutput))
		{
			DISK_GEOMETRY_STRUCT *g = (DISK_GEOMETRY_STRUCT *) Irp->AssociatedIrp.SystemBuffer;
			{
				NTSTATUS ntStatus;

				EnsureNullTerminatedString (g->deviceName, sizeof (g->deviceName));

				ntStatus = GSTDeviceIoControl (g->deviceName,
					IOCTL_DISK_GET_DRIVE_GEOMETRY,
					NULL, 0, &g->diskGeometry, sizeof (g->diskGeometry));

				Irp->IoStatus.Information = sizeof (DISK_GEOMETRY_STRUCT);
				Irp->IoStatus.Status = ntStatus;
			}
		}
		break;

	case GST_IOCTL_PROBE_REAL_DRIVE_SIZE:
		if (ValidateIOBufferSize (Irp, sizeof (ProbeRealDriveSizeRequest), ValidateInputOutput))
		{
			ProbeRealDriveSizeRequest *request = (ProbeRealDriveSizeRequest *) Irp->AssociatedIrp.SystemBuffer;
			NTSTATUS status;
			UNICODE_STRING name;
			PFILE_OBJECT fileObject;
			PDEVICE_OBJECT deviceObject;

			EnsureNullTerminatedString (request->DeviceName, sizeof (request->DeviceName));

			RtlInitUnicodeString (&name, request->DeviceName);
			status = IoGetDeviceObjectPointer (&name, FILE_READ_ATTRIBUTES, &fileObject, &deviceObject);
			if (!NT_SUCCESS (status))
			{
				Irp->IoStatus.Information = 0;
				Irp->IoStatus.Status = status;
				break;
			}

			status = ProbeRealDriveSize (deviceObject, &request->RealDriveSize);
			ObDereferenceObject (fileObject);

			if (status == STATUS_TIMEOUT)
			{
				request->TimeOut = TRUE;
				Irp->IoStatus.Information = sizeof (ProbeRealDriveSizeRequest);
				Irp->IoStatus.Status = STATUS_SUCCESS;
			}
			else if (!NT_SUCCESS (status))
			{
				Irp->IoStatus.Information = 0;
				Irp->IoStatus.Status = status;
			}
			else
			{
				request->TimeOut = FALSE;
				Irp->IoStatus.Information = sizeof (ProbeRealDriveSizeRequest);
				Irp->IoStatus.Status = status;
			}
		}
		break;

	case GST_IOCTL_MOUNT_VOLUME:
		if (ValidateIOBufferSize (Irp, sizeof (MOUNT_STRUCT), ValidateInputOutput))
		{
			MOUNT_STRUCT *mount = (MOUNT_STRUCT *) Irp->AssociatedIrp.SystemBuffer;

			if (mount->VolumePassword.Length > MAX_PASSWORD || mount->ProtectedHidVolPassword.Length > MAX_PASSWORD)
			{
				Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
				Irp->IoStatus.Information = 0;
				break;
			}

			EnsureNullTerminatedString (mount->wszVolume, sizeof (mount->wszVolume));

			Irp->IoStatus.Information = sizeof (MOUNT_STRUCT);
			Irp->IoStatus.Status = MountDevice (DeviceObject, mount);

			burn (&mount->VolumePassword, sizeof (mount->VolumePassword));
			burn (&mount->ProtectedHidVolPassword, sizeof (mount->ProtectedHidVolPassword));
		}
		break;

	case GST_IOCTL_DISMOUNT_VOLUME:
		if (ValidateIOBufferSize (Irp, sizeof (UNMOUNT_STRUCT), ValidateInputOutput))
		{
			UNMOUNT_STRUCT *unmount = (UNMOUNT_STRUCT *) Irp->AssociatedIrp.SystemBuffer;
			PDEVICE_OBJECT ListDevice = GetVirtualVolumeDeviceObject (unmount->nDosDriveNo);

			unmount->nReturnCode = ERR_DRIVE_NOT_FOUND;

			if (ListDevice)
			{
				PEXTENSION ListExtension = (PEXTENSION) ListDevice->DeviceExtension;

				if (IsVolumeAccessibleByCurrentUser (ListExtension))
					unmount->nReturnCode = UnmountDevice (unmount, ListDevice, unmount->ignoreOpenFiles);
			}

			Irp->IoStatus.Information = sizeof (UNMOUNT_STRUCT);
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		break;

	case GST_IOCTL_DISMOUNT_ALL_VOLUMES:
		if (ValidateIOBufferSize (Irp, sizeof (UNMOUNT_STRUCT), ValidateInputOutput))
		{
			UNMOUNT_STRUCT *unmount = (UNMOUNT_STRUCT *) Irp->AssociatedIrp.SystemBuffer;

			unmount->nReturnCode = UnmountAllDevices (unmount, unmount->ignoreOpenFiles);

			Irp->IoStatus.Information = sizeof (UNMOUNT_STRUCT);
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		break;

	case GST_IOCTL_BOOT_ENCRYPTION_SETUP:
		Irp->IoStatus.Status = StartBootEncryptionSetup (DeviceObject, Irp, irpSp);
		Irp->IoStatus.Information = 0;
		break;

	case GST_IOCTL_ABORT_BOOT_ENCRYPTION_SETUP:
		Irp->IoStatus.Status = AbortBootEncryptionSetup();
		Irp->IoStatus.Information = 0;
		break;

	case GST_IOCTL_GET_BOOT_ENCRYPTION_STATUS:
		GetBootEncryptionStatus (Irp, irpSp);
		break;

	case GST_IOCTL_GET_BOOT_ENCRYPTION_SETUP_RESULT:
		Irp->IoStatus.Information = 0;
		Irp->IoStatus.Status = GetSetupResult();
		break;

	case GST_IOCTL_GET_BOOT_DRIVE_VOLUME_PROPERTIES:
		GetBootDriveVolumeProperties (Irp, irpSp);
		break;

	case GST_IOCTL_GET_BOOT_LOADER_VERSION:
		GetBootLoaderVersion (Irp, irpSp);
		break;

	case GST_IOCTL_REOPEN_BOOT_VOLUME_HEADER:
		ReopenBootVolumeHeader (Irp, irpSp);
		break;

	case GST_IOCTL_GET_BOOT_ENCRYPTION_ALGORITHM_NAME:
		GetBootEncryptionAlgorithmName (Irp, irpSp);
		break;

	case GST_IOCTL_IS_HIDDEN_SYSTEM_RUNNING:
		if (ValidateIOBufferSize (Irp, sizeof (int), ValidateOutput))
		{
			*(int *) Irp->AssociatedIrp.SystemBuffer = IsHiddenSystemRunning() ? 1 : 0;
			Irp->IoStatus.Information = sizeof (int);
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		break;

	case GST_IOCTL_START_DECOY_SYSTEM_WIPE:
		Irp->IoStatus.Status = StartDecoySystemWipe (DeviceObject, Irp, irpSp);
		Irp->IoStatus.Information = 0;
		break;

	case GST_IOCTL_ABORT_DECOY_SYSTEM_WIPE:
		Irp->IoStatus.Status = AbortDecoySystemWipe();
		Irp->IoStatus.Information = 0;
		break;

	case GST_IOCTL_GET_DECOY_SYSTEM_WIPE_RESULT:
		Irp->IoStatus.Status = GetDecoySystemWipeResult();
		Irp->IoStatus.Information = 0;
		break;

	case GST_IOCTL_GET_DECOY_SYSTEM_WIPE_STATUS:
		GetDecoySystemWipeStatus (Irp, irpSp);
		break;

	case GST_IOCTL_WRITE_BOOT_DRIVE_SECTOR:
		Irp->IoStatus.Status = WriteBootDriveSector (Irp, irpSp);
		Irp->IoStatus.Information = 0;
		break;

	case GST_IOCTL_GET_WARNING_FLAGS:
		if (ValidateIOBufferSize (Irp, sizeof (GetWarningFlagsRequest), ValidateOutput))
		{
			GetWarningFlagsRequest *flags = (GetWarningFlagsRequest *) Irp->AssociatedIrp.SystemBuffer;

			flags->PagingFileCreationPrevented = PagingFileCreationPrevented;
			PagingFileCreationPrevented = FALSE;
			flags->SystemFavoriteVolumeDirty = SystemFavoriteVolumeDirty;
			SystemFavoriteVolumeDirty = FALSE;

			Irp->IoStatus.Information = sizeof (GetWarningFlagsRequest);
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		break;

	case GST_IOCTL_SET_SYSTEM_FAVORITE_VOLUME_DIRTY:
		if (UserCanAccessDriveDevice())
		{
			SystemFavoriteVolumeDirty = TRUE;
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		else
			Irp->IoStatus.Status = STATUS_ACCESS_DENIED;

		Irp->IoStatus.Information = 0;
		break;

	case GST_IOCTL_REREAD_DRIVER_CONFIG:
		Irp->IoStatus.Status = ReadRegistryConfigFlags (FALSE);
		Irp->IoStatus.Information = 0;
		break;

	case GST_IOCTL_GET_SYSTEM_DRIVE_DUMP_CONFIG:
		if (ValidateIOBufferSize (Irp, sizeof (GetSystemDriveDumpConfigRequest), ValidateOutput))
		{
			GetSystemDriveDumpConfigRequest *request = (GetSystemDriveDumpConfigRequest *) Irp->AssociatedIrp.SystemBuffer;

			request->BootDriveFilterExtension = GetBootDriveFilterExtension();
			if (IsBootDriveMounted() && request->BootDriveFilterExtension)
			{
				request->HwEncryptionEnabled = IsHwEncryptionEnabled();
				Irp->IoStatus.Status = STATUS_SUCCESS;
				Irp->IoStatus.Information = sizeof (*request);
			}
			else
			{
				Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
				Irp->IoStatus.Information = 0;
			}
		}
		break;

	default:
		return GSTCompleteIrp (Irp, STATUS_INVALID_DEVICE_REQUEST, 0);
	}

	
#ifdef DEBUG
	if (!NT_SUCCESS (Irp->IoStatus.Status))
	{
		switch (irpSp->Parameters.DeviceIoControl.IoControlCode)
		{
		case GST_IOCTL_GET_MOUNTED_VOLUMES:
		case GST_IOCTL_GET_PASSWORD_CACHE_STATUS:
		case GST_IOCTL_GET_PORTABLE_MODE_STATUS:
		case GST_IOCTL_SET_PORTABLE_MODE_STATUS:
		case GST_IOCTL_OPEN_TEST:
		case GST_IOCTL_GET_RESOLVED_SYMLINK:
		case GST_IOCTL_GET_DRIVE_PARTITION_INFO:
		case GST_IOCTL_GET_BOOT_DRIVE_VOLUME_PROPERTIES:
		case GST_IOCTL_GET_BOOT_ENCRYPTION_STATUS:
		case GST_IOCTL_IS_HIDDEN_SYSTEM_RUNNING:
			break;

		default:
			Dump ("IOCTL error 0x%08x\n", Irp->IoStatus.Status);
		}
	}
#endif

	return GSTCompleteIrp (Irp, Irp->IoStatus.Status, Irp->IoStatus.Information);
}


/**
 *
 *	\fn NTSTATUS GSTStartThread (PKSTART_ROUTINE threadProc, PVOID threadArg, PKTHREAD *kThread)
 *	\brief Start a gst thread
 *	\param[in] PKTHREAD * kThread
 *	\param[in] PVOID threadArg
 *	\param[in] PKSTART_ROUTINE threadProc
 *	\return NTSTATUS GSTStartThreadInProcess (threadProc, threadArg, kThread, NULL)
 *
 */
NTSTATUS GSTStartThread (PKSTART_ROUTINE threadProc, PVOID threadArg, PKTHREAD *kThread)
{
	return GSTStartThreadInProcess (threadProc, threadArg, kThread, NULL);
}


/**
 *
 *	\fn NTSTATUS GSTStartThreadInProcess (PKSTART_ROUTINE threadProc, PVOID threadArg, PKTHREAD *kThread, PEPROCESS process)
 *	\brief Start thread in a specified process
 *	\param[in] PEPROCESS process
 *	\param[in] PKTHREAD * kThread
 *	\param[in] PVOID threadArg
 *	\param[in] PKSTART_ROUTINE threadProc
 *	\return NTSTATUS STATUS_SUCCESS if success
 *			status otherwise
 *
 */
NTSTATUS GSTStartThreadInProcess (PKSTART_ROUTINE threadProc, PVOID threadArg, PKTHREAD *kThread, PEPROCESS process)
{
	NTSTATUS status;
	HANDLE threadHandle;
	HANDLE processHandle = NULL;
	OBJECT_ATTRIBUTES threadObjAttributes;

	if (process)
	{
		status = ObOpenObjectByPointer (process, OBJ_KERNEL_HANDLE, NULL, 0, NULL, KernelMode, &processHandle);
		if (!NT_SUCCESS (status))
			return status;
	}

	InitializeObjectAttributes (&threadObjAttributes, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);
	
	status = PsCreateSystemThread (&threadHandle, THREAD_ALL_ACCESS, &threadObjAttributes, processHandle, NULL, threadProc, threadArg);
	if (!NT_SUCCESS (status))
		return status;

	status = ObReferenceObjectByHandle (threadHandle, THREAD_ALL_ACCESS, NULL, KernelMode, (PVOID *) kThread, NULL);
	if (!NT_SUCCESS (status))
	{
		ZwClose (threadHandle);
		*kThread = NULL;
		return status;
	}

	if (processHandle)
		ZwClose (processHandle);

	ZwClose (threadHandle);
	return STATUS_SUCCESS;
}


/**
 *
 *	\fn void GSTStopThread (PKTHREAD kThread, PKEVENT wakeUpEvent)
 *	\brief If wake up event : wake up the thread in argument
 *	\param[in] PKEVENT wakeUpEvent
 *	\param[in] PKTHREAD kThread
 *	\return void
 *
 */
void GSTStopThread (PKTHREAD kThread, PKEVENT wakeUpEvent)
{
	if (wakeUpEvent)
		KeSetEvent (wakeUpEvent, 0, FALSE);

	KeWaitForSingleObject (kThread, Executive, KernelMode, FALSE, NULL);
	ObDereferenceObject (kThread);
}


/**
 *
 *	\fn NTSTATUS GSTStartVolumeThread (PDEVICE_OBJECT DeviceObject, PEXTENSION Extension, MOUNT_STRUCT * mount)
 *	\brief Start a volume thread.
 *	\param[in] MOUNT_STRUCT * mount
 *	\param[in] PEXTENSION Extension
 *	\param[in] PDEVICE_OBJECT DeviceObject
 *	\return NTSTATUS ntStatus
 *
 */
NTSTATUS GSTStartVolumeThread (PDEVICE_OBJECT DeviceObject, PEXTENSION Extension, MOUNT_STRUCT * mount)
{
	PTHREAD_BLOCK pThreadBlock = GSTalloc (sizeof (THREAD_BLOCK));
	HANDLE hThread;
	NTSTATUS ntStatus;
	OBJECT_ATTRIBUTES threadObjAttributes;
	SECURITY_QUALITY_OF_SERVICE qos;

	Dump ("Starting thread...\n");

	if (pThreadBlock == NULL)
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	else
	{
		pThreadBlock->DeviceObject = DeviceObject;
		pThreadBlock->mount = mount;
	}

	qos.Length = sizeof (qos);
	qos.ContextTrackingMode = SECURITY_STATIC_TRACKING;
	qos.EffectiveOnly = TRUE;
	qos.ImpersonationLevel = SecurityImpersonation;

	ntStatus = SeCreateClientSecurity (PsGetCurrentThread(), &qos, FALSE, &Extension->SecurityClientContext);
	if (!NT_SUCCESS (ntStatus))
		goto ret;

	Extension->SecurityClientContextValid = TRUE;

	Extension->bThreadShouldQuit = FALSE;

	InitializeObjectAttributes (&threadObjAttributes, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

	ntStatus = PsCreateSystemThread (&hThread,
					 THREAD_ALL_ACCESS,
					 &threadObjAttributes,
					 NULL,
					 NULL,
					 VolumeThreadProc,
					 pThreadBlock);

	if (!NT_SUCCESS (ntStatus))
	{
		Dump ("PsCreateSystemThread Failed END\n");
		goto ret;
	}

	ntStatus = ObReferenceObjectByHandle (hThread,
				   THREAD_ALL_ACCESS,
				   NULL,
				   KernelMode,
				   &Extension->peThread,
				   NULL);

	ZwClose (hThread);

	if (!NT_SUCCESS (ntStatus))
		goto ret;

	Dump ("Waiting for thread to initialize...\n");

	KeWaitForSingleObject (&Extension->keCreateEvent,
			       Executive,
			       KernelMode,
			       FALSE,
			       NULL);

	Dump ("Waiting completed! Thread returns 0x%08x\n", pThreadBlock->ntCreateStatus);
	ntStatus = pThreadBlock->ntCreateStatus;

ret:
	GSTfree (pThreadBlock);
	return ntStatus;
}

/**
 *
 *	\fn void GSTStopVolumeThread (PDEVICE_OBJECT DeviceObject, PEXTENSION Extension)
 *	\brief Stop a voulme thread
 *	\param[in] PEXTENSION Extension
 *	\param[in] PDEVICE_OBJECT DeviceObject
 *	\return void
 *
 */
void GSTStopVolumeThread (PDEVICE_OBJECT DeviceObject, PEXTENSION Extension)
{
	NTSTATUS ntStatus;

	if (DeviceObject);	/* Remove compiler warning */

	Dump ("Signalling thread to quit...\n");

	Extension->bThreadShouldQuit = TRUE;

	KeReleaseSemaphore (&Extension->RequestSemaphore,
			    0,
			    1,
			    TRUE);

	ntStatus = KeWaitForSingleObject (Extension->peThread,
					  Executive,
					  KernelMode,
					  FALSE,
					  NULL);

	ASSERT (NT_SUCCESS (ntStatus));

	ObDereferenceObject (Extension->peThread);
	Extension->peThread = NULL;

	Dump ("Thread exited\n");
}


/**
 *
 *	\fn void GSTSleep (int milliSeconds)
 *	\brief Suspend current thread for a number of milliseconds
 *	\param[in] int milliSeconds
 *	\return void
 *
 */
void GSTSleep (int milliSeconds)
{
	PKTIMER timer = (PKTIMER) GSTalloc (sizeof (KTIMER));
	LARGE_INTEGER duetime;

	if (!timer)
		return;

	duetime.QuadPart = (__int64) milliSeconds * -10000;
	KeInitializeTimerEx(timer, NotificationTimer);
	KeSetTimerEx(timer, duetime, 0, NULL);

	KeWaitForSingleObject (timer, Executive, KernelMode, FALSE, NULL);

	GSTfree (timer);
}


/**
 *
 *	\fn VOID VolumeThreadProc (PVOID Context)
 *	\brief VolumeThreadProc does all the work of processing IRP's, and dispatching them
 *  to either the ReadWrite function or the DeviceControl function.
 *	\param[in] PVOID Context
 *	\return VOID
 *
 */
VOID VolumeThreadProc (PVOID Context)
{
	PTHREAD_BLOCK pThreadBlock = (PTHREAD_BLOCK) Context;
	PDEVICE_OBJECT DeviceObject = pThreadBlock->DeviceObject;
	PEXTENSION Extension = (PEXTENSION) DeviceObject->DeviceExtension;
	BOOL bDevice;

	/* Set thread priority to lowest realtime level. */
	KeSetPriorityThread (KeGetCurrentThread (), LOW_REALTIME_PRIORITY);

	Dump ("Mount THREAD OPENING VOLUME BEGIN\n");

	if (memcmp (pThreadBlock->mount->wszVolume, WIDE ("\\Device"), 14) != 0)
	{
		wcscpy (pThreadBlock->wszMountVolume, WIDE ("\\??\\"));
		wcsncat (pThreadBlock->wszMountVolume, pThreadBlock->mount->wszVolume,
			sizeof (pThreadBlock->wszMountVolume) / 2 - 5);
		bDevice = FALSE;
	}
	else
	{
		pThreadBlock->wszMountVolume[0] = 0;
		wcsncat (pThreadBlock->wszMountVolume, pThreadBlock->mount->wszVolume,
			sizeof (pThreadBlock->wszMountVolume) / 2 - 1);
		bDevice = TRUE;
	}

	Dump ("Mount THREAD request for File %ls DriveNumber %d Device = %d\n",
	      pThreadBlock->wszMountVolume, pThreadBlock->mount->nDosDriveNo, bDevice);

	pThreadBlock->ntCreateStatus = GSTOpenVolume (DeviceObject,
		Extension,
		pThreadBlock->mount,
		pThreadBlock->wszMountVolume,
		bDevice);

	if (!NT_SUCCESS (pThreadBlock->ntCreateStatus) || pThreadBlock->mount->nReturnCode != 0)
	{
		KeSetEvent (&Extension->keCreateEvent, 0, FALSE);
		PsTerminateSystemThread (STATUS_SUCCESS);
	}

	// Start IO queue
	Extension->Queue.IsFilterDevice = FALSE;
	Extension->Queue.DeviceObject = DeviceObject;
	Extension->Queue.CryptoInfo = Extension->cryptoInfo;
	Extension->Queue.HostFileHandle = Extension->hDeviceFile;
	Extension->Queue.VirtualDeviceLength = Extension->DiskLength;
	Extension->Queue.MaxReadAheadOffset.QuadPart = Extension->HostLength;

	if (Extension->SecurityClientContextValid)
		Extension->Queue.SecurityClientContext = &Extension->SecurityClientContext;
	else
		Extension->Queue.SecurityClientContext = NULL;

	pThreadBlock->ntCreateStatus = EncryptedIoQueueStart (&Extension->Queue);

	if (!NT_SUCCESS (pThreadBlock->ntCreateStatus))
	{
		GSTCloseVolume (DeviceObject, Extension);

		pThreadBlock->mount->nReturnCode = ERR_OS_ERROR;
		KeSetEvent (&Extension->keCreateEvent, 0, FALSE);
		PsTerminateSystemThread (STATUS_SUCCESS);
	}

	KeSetEvent (&Extension->keCreateEvent, 0, FALSE);
	/* From this point on pThreadBlock cannot be used as it will have been released! */
	pThreadBlock = NULL;

	for (;;)
	{
		/* Wait for a request from the dispatch routines. */
		KeWaitForSingleObject ((PVOID) & Extension->RequestSemaphore, Executive, KernelMode, FALSE, NULL);

		for (;;)
		{
			PIO_STACK_LOCATION irpSp;
			PLIST_ENTRY request;
			PIRP irp;

			request = ExInterlockedRemoveHeadList (&Extension->ListEntry, &Extension->ListSpinLock);
			if (request == NULL)
				break;

			irp = CONTAINING_RECORD (request, IRP, Tail.Overlay.ListEntry);
			irpSp = IoGetCurrentIrpStackLocation (irp);

			ASSERT (irpSp->MajorFunction == IRP_MJ_DEVICE_CONTROL);

			ProcessVolumeDeviceControlIrp (DeviceObject, Extension, irp);
			IoReleaseRemoveLock (&Extension->Queue.RemoveLock, irp);
		}

		if (Extension->bThreadShouldQuit)
		{
			Dump ("Closing volume\n");
			EncryptedIoQueueStop (&Extension->Queue);

			GSTCloseVolume (DeviceObject, Extension);
			PsTerminateSystemThread (STATUS_SUCCESS);
		}
	}
}

/**
 *
 *	\fn void GSTGetNTNameFromNumber (LPWSTR ntname, int nDriveNo)
 *	\brief Update ntname argument to the name of the drivenumber.
 *	\param[in] int nDriveNo
 *	\param[in] LPWSTR ntname
 *	\return void
 *
 */
void GSTGetNTNameFromNumber (LPWSTR ntname, int nDriveNo)
{
	WCHAR tmp[3] =
	{0, ':', 0};
	int j = nDriveNo + (WCHAR) 'A';

	tmp[0] = (short) j;
	wcscpy (ntname, (LPWSTR) NT_MOUNT_PREFIX);
	wcsncat (ntname, tmp, 1);
}

/**
 *
 *	\fn void GSTGetDosNameFromNumber (LPWSTR dosname, int nDriveNo)
 *	\brief Update dosname argument to the name of the drivenumber.
 *	\param[in] int nDriveNo
 *	\param[in] LPWSTR dosname
 *	\return void
 *
 */
void GSTGetDosNameFromNumber (LPWSTR dosname, int nDriveNo)
{
	WCHAR tmp[3] =
	{0, ':', 0};
	int j = nDriveNo + (WCHAR) 'A';

	tmp[0] = (short) j;
	wcscpy (dosname, (LPWSTR) DOS_MOUNT_PREFIX);
	wcscat (dosname, tmp);
}

#ifdef _DEBUG
LPWSTR GSTTranslateCode (ULONG ulCode)
{
	switch (ulCode)
	{
#define GST_CASE_RET_NAME(CODE) case CODE : return L###CODE

		GST_CASE_RET_NAME (GST_IOCTL_ABORT_BOOT_ENCRYPTION_SETUP);
		GST_CASE_RET_NAME (GST_IOCTL_ABORT_DECOY_SYSTEM_WIPE);
		GST_CASE_RET_NAME (GST_IOCTL_BOOT_ENCRYPTION_SETUP);
		GST_CASE_RET_NAME (GST_IOCTL_DISMOUNT_ALL_VOLUMES);
		GST_CASE_RET_NAME (GST_IOCTL_DISMOUNT_VOLUME);
		GST_CASE_RET_NAME (GST_IOCTL_GET_BOOT_DRIVE_VOLUME_PROPERTIES);
		GST_CASE_RET_NAME (GST_IOCTL_GET_BOOT_ENCRYPTION_ALGORITHM_NAME);
		GST_CASE_RET_NAME (GST_IOCTL_GET_BOOT_ENCRYPTION_SETUP_RESULT);
		GST_CASE_RET_NAME (GST_IOCTL_GET_BOOT_ENCRYPTION_STATUS);
		GST_CASE_RET_NAME (GST_IOCTL_GET_BOOT_LOADER_VERSION);
		GST_CASE_RET_NAME (GST_IOCTL_GET_DECOY_SYSTEM_WIPE_RESULT);
		GST_CASE_RET_NAME (GST_IOCTL_GET_DECOY_SYSTEM_WIPE_STATUS);
		GST_CASE_RET_NAME (GST_IOCTL_GET_DEVICE_REFCOUNT);
		GST_CASE_RET_NAME (GST_IOCTL_GET_DRIVE_GEOMETRY);
		GST_CASE_RET_NAME (GST_IOCTL_GET_DRIVE_PARTITION_INFO);
		GST_CASE_RET_NAME (GST_IOCTL_GET_DRIVER_VERSION);
		GST_CASE_RET_NAME (GST_IOCTL_GET_MOUNTED_VOLUMES);
		GST_CASE_RET_NAME (GST_IOCTL_GET_PASSWORD_CACHE_STATUS);
		GST_CASE_RET_NAME (GST_IOCTL_GET_SYSTEM_DRIVE_CONFIG);
		GST_CASE_RET_NAME (GST_IOCTL_GET_PORTABLE_MODE_STATUS);
		GST_CASE_RET_NAME (GST_IOCTL_SET_PORTABLE_MODE_STATUS);
		GST_CASE_RET_NAME (GST_IOCTL_GET_RESOLVED_SYMLINK);
		GST_CASE_RET_NAME (GST_IOCTL_GET_SYSTEM_DRIVE_DUMP_CONFIG);
		GST_CASE_RET_NAME (GST_IOCTL_GET_VOLUME_PROPERTIES);
		GST_CASE_RET_NAME (GST_IOCTL_GET_WARNING_FLAGS);
		GST_CASE_RET_NAME (GST_IOCTL_DISK_IS_WRITABLE);
		GST_CASE_RET_NAME (GST_IOCTL_IS_ANY_VOLUME_MOUNTED);
		GST_CASE_RET_NAME (GST_IOCTL_IS_DRIVER_UNLOAD_DISABLED);
		GST_CASE_RET_NAME (GST_IOCTL_IS_HIDDEN_SYSTEM_RUNNING);
		GST_CASE_RET_NAME (GST_IOCTL_MOUNT_VOLUME);
		GST_CASE_RET_NAME (GST_IOCTL_OPEN_TEST);
		GST_CASE_RET_NAME (GST_IOCTL_PROBE_REAL_DRIVE_SIZE);
		GST_CASE_RET_NAME (GST_IOCTL_REOPEN_BOOT_VOLUME_HEADER);
		GST_CASE_RET_NAME (GST_IOCTL_REREAD_DRIVER_CONFIG);
		GST_CASE_RET_NAME (GST_IOCTL_SET_SYSTEM_FAVORITE_VOLUME_DIRTY);
		GST_CASE_RET_NAME (GST_IOCTL_START_DECOY_SYSTEM_WIPE);
		GST_CASE_RET_NAME (GST_IOCTL_WIPE_PASSWORD_CACHE);
		GST_CASE_RET_NAME (GST_IOCTL_WRITE_BOOT_DRIVE_SECTOR);

		GST_CASE_RET_NAME (IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS);

#undef GST_CASE_RET_NAME
	}

	if (ulCode ==			 IOCTL_DISK_GET_DRIVE_GEOMETRY)
		return (LPWSTR) _T ("IOCTL_DISK_GET_DRIVE_GEOMETRY");
	else if (ulCode ==		 IOCTL_DISK_GET_DRIVE_GEOMETRY_EX)
		return (LPWSTR) _T ("IOCTL_DISK_GET_DRIVE_GEOMETRY_EX");
	else if (ulCode ==		 IOCTL_MOUNTDEV_QUERY_DEVICE_NAME)
		return (LPWSTR) _T ("IOCTL_MOUNTDEV_QUERY_DEVICE_NAME");
	else if (ulCode ==		 IOCTL_MOUNTDEV_QUERY_SUGGESTED_LINK_NAME)
		return (LPWSTR) _T ("IOCTL_MOUNTDEV_QUERY_SUGGESTED_LINK_NAME");
	else if (ulCode ==		 IOCTL_MOUNTDEV_QUERY_UNIQUE_ID)
		return (LPWSTR) _T ("IOCTL_MOUNTDEV_QUERY_UNIQUE_ID");
	else if (ulCode ==		 IOCTL_VOLUME_ONLINE)
		return (LPWSTR) _T ("IOCTL_VOLUME_ONLINE");
	else if (ulCode ==		 IOCTL_MOUNTDEV_LINK_CREATED)
		return (LPWSTR) _T ("IOCTL_MOUNTDEV_LINK_CREATED");
	else if (ulCode ==		 IOCTL_MOUNTDEV_LINK_DELETED)
		return (LPWSTR) _T ("IOCTL_MOUNTDEV_LINK_DELETED");
	else if (ulCode ==		 IOCTL_MOUNTMGR_QUERY_POINTS)
		return (LPWSTR) _T ("IOCTL_MOUNTMGR_QUERY_POINTS");
	else if (ulCode ==		 IOCTL_MOUNTMGR_VOLUME_MOUNT_POINT_CREATED)
		return (LPWSTR) _T ("IOCTL_MOUNTMGR_VOLUME_MOUNT_POINT_CREATED");
	else if (ulCode ==		 IOCTL_MOUNTMGR_VOLUME_MOUNT_POINT_DELETED)
		return (LPWSTR) _T ("IOCTL_MOUNTMGR_VOLUME_MOUNT_POINT_DELETED");
	else if (ulCode ==		 IOCTL_DISK_GET_LENGTH_INFO)
		return (LPWSTR) _T ("IOCTL_DISK_GET_LENGTH_INFO");
	else if (ulCode ==		 IOCTL_STORAGE_GET_DEVICE_NUMBER)
		return (LPWSTR) _T ("IOCTL_STORAGE_GET_DEVICE_NUMBER");
	else if (ulCode ==		 IOCTL_DISK_GET_PARTITION_INFO)
		return (LPWSTR) _T ("IOCTL_DISK_GET_PARTITION_INFO");
	else if (ulCode ==		 IOCTL_DISK_GET_PARTITION_INFO_EX)
		return (LPWSTR) _T ("IOCTL_DISK_GET_PARTITION_INFO_EX");
	else if (ulCode ==		 IOCTL_DISK_SET_PARTITION_INFO)
		return (LPWSTR) _T ("IOCTL_DISK_SET_PARTITION_INFO");
	else if (ulCode ==		 IOCTL_DISK_GET_DRIVE_LAYOUT)
		return (LPWSTR) _T ("IOCTL_DISK_GET_DRIVE_LAYOUT");
	else if (ulCode ==		 IOCTL_DISK_SET_DRIVE_LAYOUT_EX)
		return (LPWSTR) _T ("IOCTL_DISK_SET_DRIVE_LAYOUT_EX");
	else if (ulCode ==		 IOCTL_DISK_VERIFY)
		return (LPWSTR) _T ("IOCTL_DISK_VERIFY");
	else if (ulCode == IOCTL_DISK_FORMAT_TRACKS)
		return (LPWSTR) _T ("IOCTL_DISK_FORMAT_TRACKS");
	else if (ulCode == IOCTL_DISK_REASSIGN_BLOCKS)
		return (LPWSTR) _T ("IOCTL_DISK_REASSIGN_BLOCKS");
	else if (ulCode == IOCTL_DISK_PERFORMANCE)
		return (LPWSTR) _T ("IOCTL_DISK_PERFORMANCE");
	else if (ulCode == IOCTL_DISK_IS_WRITABLE)
		return (LPWSTR) _T ("IOCTL_DISK_IS_WRITABLE");
	else if (ulCode == IOCTL_DISK_LOGGING)
		return (LPWSTR) _T ("IOCTL_DISK_LOGGING");
	else if (ulCode == IOCTL_DISK_FORMAT_TRACKS_EX)
		return (LPWSTR) _T ("IOCTL_DISK_FORMAT_TRACKS_EX");
	else if (ulCode == IOCTL_DISK_HISTOGRAM_STRUCTURE)
		return (LPWSTR) _T ("IOCTL_DISK_HISTOGRAM_STRUCTURE");
	else if (ulCode == IOCTL_DISK_HISTOGRAM_DATA)
		return (LPWSTR) _T ("IOCTL_DISK_HISTOGRAM_DATA");
	else if (ulCode == IOCTL_DISK_HISTOGRAM_RESET)
		return (LPWSTR) _T ("IOCTL_DISK_HISTOGRAM_RESET");
	else if (ulCode == IOCTL_DISK_REQUEST_STRUCTURE)
		return (LPWSTR) _T ("IOCTL_DISK_REQUEST_STRUCTURE");
	else if (ulCode == IOCTL_DISK_REQUEST_DATA)
		return (LPWSTR) _T ("IOCTL_DISK_REQUEST_DATA");
	else if (ulCode == IOCTL_DISK_CONTROLLER_NUMBER)
		return (LPWSTR) _T ("IOCTL_DISK_CONTROLLER_NUMBER");
	else if (ulCode == SMART_GET_VERSION)
		return (LPWSTR) _T ("SMART_GET_VERSION");
	else if (ulCode == SMART_SEND_DRIVE_COMMAND)
		return (LPWSTR) _T ("SMART_SEND_DRIVE_COMMAND");
	else if (ulCode == SMART_RCV_DRIVE_DATA)
		return (LPWSTR) _T ("SMART_RCV_DRIVE_DATA");
	else if (ulCode == IOCTL_DISK_INTERNAL_SET_VERIFY)
		return (LPWSTR) _T ("IOCTL_DISK_INTERNAL_SET_VERIFY");
	else if (ulCode == IOCTL_DISK_INTERNAL_CLEAR_VERIFY)
		return (LPWSTR) _T ("IOCTL_DISK_INTERNAL_CLEAR_VERIFY");
	else if (ulCode == IOCTL_DISK_CHECK_VERIFY)
		return (LPWSTR) _T ("IOCTL_DISK_CHECK_VERIFY");
	else if (ulCode == IOCTL_DISK_MEDIA_REMOVAL)
		return (LPWSTR) _T ("IOCTL_DISK_MEDIA_REMOVAL");
	else if (ulCode == IOCTL_DISK_EJECT_MEDIA)
		return (LPWSTR) _T ("IOCTL_DISK_EJECT_MEDIA");
	else if (ulCode == IOCTL_DISK_LOAD_MEDIA)
		return (LPWSTR) _T ("IOCTL_DISK_LOAD_MEDIA");
	else if (ulCode == IOCTL_DISK_RESERVE)
		return (LPWSTR) _T ("IOCTL_DISK_RESERVE");
	else if (ulCode == IOCTL_DISK_RELEASE)
		return (LPWSTR) _T ("IOCTL_DISK_RELEASE");
	else if (ulCode == IOCTL_DISK_FIND_NEW_DEVICES)
		return (LPWSTR) _T ("IOCTL_DISK_FIND_NEW_DEVICES");
	else if (ulCode == IOCTL_DISK_GET_MEDIA_TYPES)
		return (LPWSTR) _T ("IOCTL_DISK_GET_MEDIA_TYPES");
	else if (ulCode == IOCTL_STORAGE_SET_HOTPLUG_INFO)
		return (LPWSTR) _T ("IOCTL_STORAGE_SET_HOTPLUG_INFO");
	else if (ulCode == IRP_MJ_READ)
		return (LPWSTR) _T ("IRP_MJ_READ");
	else if (ulCode == IRP_MJ_WRITE)
		return (LPWSTR) _T ("IRP_MJ_WRITE");
	else if (ulCode == IRP_MJ_CREATE)
		return (LPWSTR) _T ("IRP_MJ_CREATE");
	else if (ulCode == IRP_MJ_CLOSE)
		return (LPWSTR) _T ("IRP_MJ_CLOSE");
	else if (ulCode == IRP_MJ_CLEANUP)
		return (LPWSTR) _T ("IRP_MJ_CLEANUP");
	else if (ulCode == IRP_MJ_FLUSH_BUFFERS)
		return (LPWSTR) _T ("IRP_MJ_FLUSH_BUFFERS");
	else if (ulCode == IRP_MJ_SHUTDOWN)
		return (LPWSTR) _T ("IRP_MJ_SHUTDOWN");
	else if (ulCode == IRP_MJ_DEVICE_CONTROL)
		return (LPWSTR) _T ("IRP_MJ_DEVICE_CONTROL");
	else
	{
		return (LPWSTR) _T ("IOCTL");
	}
}

#endif

/**
 *
 *	\fn void GSTDeleteDeviceObject (PDEVICE_OBJECT DeviceObject, PEXTENSION Extension)
 *	\brief Delete a device object
 *	\param[in] PEXTENSION Extension
 *	\param[in] PDEVICE_OBJECT DeviceObject
 *	\return void
 *
 */
void GSTDeleteDeviceObject (PDEVICE_OBJECT DeviceObject, PEXTENSION Extension)
{
	UNICODE_STRING Win32NameString;
	NTSTATUS ntStatus;

	Dump ("GSTDeleteDeviceObject BEGIN\n");

	if (Extension->bRootDevice)
	{
		RtlInitUnicodeString (&Win32NameString, (LPWSTR) DOS_ROOT_PREFIX);
		ntStatus = IoDeleteSymbolicLink (&Win32NameString);
		if (!NT_SUCCESS (ntStatus))
			Dump ("IoDeleteSymbolicLink failed ntStatus = 0x%08x\n", ntStatus);

		RootDeviceObject = NULL;
	}
	else
	{
		if (Extension->peThread != NULL)
			GSTStopVolumeThread (DeviceObject, Extension);

		if (Extension->UserSid)
			GSTfree (Extension->UserSid);

		if (Extension->SecurityClientContextValid)
		{
			if (OsMajorVersion == 5 && OsMinorVersion == 0)
			{
				ObDereferenceObject (Extension->SecurityClientContext.ClientToken);
			}
			else
			{
				// Windows 2000 does not support PsDereferenceImpersonationToken() used by SeDeleteClientSecurity().
				// TODO: Use only SeDeleteClientSecurity() once support for Windows 2000 is dropped.

				VOID (*PsDereferenceImpersonationTokenD) (PACCESS_TOKEN ImpersonationToken);
				UNICODE_STRING name;
				RtlInitUnicodeString (&name, L"PsDereferenceImpersonationToken");

				PsDereferenceImpersonationTokenD = MmGetSystemRoutineAddress (&name);
				if (!PsDereferenceImpersonationTokenD)
					GST_BUG_CHECK (STATUS_NOT_IMPLEMENTED);
				
#				define PsDereferencePrimaryToken
#				define PsDereferenceImpersonationToken PsDereferenceImpersonationTokenD

				SeDeleteClientSecurity (&Extension->SecurityClientContext);

#				undef PsDereferencePrimaryToken
#				undef PsDereferenceImpersonationToken
			}
		}

		VirtualVolumeDeviceObjects[Extension->nDosDriveNo] = NULL;
	}

	IoDeleteDevice (DeviceObject);

	Dump ("GSTDeleteDeviceObject END\n");
}


/**
 *
 *	\fn VOID GSTUnloadDriver (PDRIVER_OBJECT DriverObject)
 *	\brief Unload a driver
 *	\param[in] PDRIVER_OBJECT DriverObject
 *	\return VOID
 *
 */
VOID GSTUnloadDriver (PDRIVER_OBJECT DriverObject)
{
	Dump ("GSTUnloadDriver BEGIN\n");

	OnShutdownPending();

	if (IsBootDriveMounted())
		GST_BUG_CHECK (STATUS_INVALID_DEVICE_STATE);

	EncryptionThreadPoolStop();
	GSTDeleteDeviceObject (RootDeviceObject, (PEXTENSION) RootDeviceObject->DeviceExtension);

	Dump ("GSTUnloadDriver END\n");
}


/**
 *
 *	\fn void OnShutdownPending ()
 *	\brief Unmount the root device object and ignore open files
 *			also unmount hidden volumes
 *	\return void
 *
 */
void OnShutdownPending ()
{
	UNMOUNT_STRUCT unmount;
	memset (&unmount, 0, sizeof (unmount));
	unmount.ignoreOpenFiles = TRUE;

	while (SendDeviceIoControlRequest (RootDeviceObject, GST_IOCTL_DISMOUNT_ALL_VOLUMES, &unmount, sizeof (unmount), &unmount, sizeof (unmount)) == STATUS_INSUFFICIENT_RESOURCES || unmount.HiddenVolumeProtectionTriggered)
		unmount.HiddenVolumeProtectionTriggered = FALSE;

	while (SendDeviceIoControlRequest (RootDeviceObject, GST_IOCTL_WIPE_PASSWORD_CACHE, NULL, 0, NULL, 0) == STATUS_INSUFFICIENT_RESOURCES);
}


/**
 *
 *	\fn NTSTATUS GSTDeviceIoControl (PWSTR deviceName, ULONG IoControlCode, void *InputBuffer, ULONG InputBufferSize, void *OutputBuffer, ULONG OutputBufferSize)
 *	\brief Build a control request for the device and send it, return the ntstatus to the specified device in argument, wake up the thread
 *			for this device
 *	\param[in] ULONG OutputBufferSize
 *	\param[in] void * OutputBuffer
 *	\param[in] ULONG InputBufferSize
 *	\param[in] void * InputBuffer
 *	\param[in] ULONG IoControlCode
 *	\param[in] PWSTR deviceName
 *	\return NTSTATUS ntStatus
 *
 */
NTSTATUS GSTDeviceIoControl (PWSTR deviceName, ULONG IoControlCode, void *InputBuffer, ULONG InputBufferSize, void *OutputBuffer, ULONG OutputBufferSize)
{
	IO_STATUS_BLOCK ioStatusBlock;
	NTSTATUS ntStatus;
	PIRP irp;
	PFILE_OBJECT fileObject;
	PDEVICE_OBJECT deviceObject;
	KEVENT event;
	UNICODE_STRING name;

	RtlInitUnicodeString(&name, deviceName);
	ntStatus = IoGetDeviceObjectPointer (&name, FILE_READ_ATTRIBUTES, &fileObject, &deviceObject);

	if (!NT_SUCCESS (ntStatus))
		return ntStatus;

	KeInitializeEvent(&event, NotificationEvent, FALSE);

	irp = IoBuildDeviceIoControlRequest (IoControlCode,
					     deviceObject,
					     InputBuffer, InputBufferSize,
					     OutputBuffer, OutputBufferSize,
					     FALSE,
					     &event,
					     &ioStatusBlock);

	if (irp == NULL)
	{
		Dump ("IRP allocation failed\n");
		ntStatus = STATUS_INSUFFICIENT_RESOURCES;
		goto ret;
	}

	IoGetNextIrpStackLocation (irp)->FileObject = fileObject;

	ntStatus = IoCallDriver (deviceObject, irp);
	if (ntStatus == STATUS_PENDING)
	{
		KeWaitForSingleObject (&event, Executive, KernelMode, FALSE, NULL);
		ntStatus = ioStatusBlock.Status;
	}

ret:
	ObDereferenceObject (fileObject);
	return ntStatus;
}


typedef struct
{
	PDEVICE_OBJECT deviceObject; ULONG ioControlCode; void *inputBuffer; int inputBufferSize; void *outputBuffer; int outputBufferSize;
	NTSTATUS Status;
	KEVENT WorkItemCompletedEvent;
} SendDeviceIoControlRequestWorkItemArgs;


/**
 *
 *	\fn static VOID SendDeviceIoControlRequestWorkItemRoutine (PDEVICE_OBJECT rootDeviceObject, SendDeviceIoControlRequestWorkItemArgs *arg)
 *	\brief Send the device control routine.
 *	\param[in] SendDeviceIoControlRequestWorkItemArgs * arg
 *	\param[in] PDEVICE_OBJECT rootDeviceObject
 *	\return static VOID
 *
 */
static VOID SendDeviceIoControlRequestWorkItemRoutine (PDEVICE_OBJECT rootDeviceObject, SendDeviceIoControlRequestWorkItemArgs *arg)
{
	arg->Status = SendDeviceIoControlRequest (arg->deviceObject, arg->ioControlCode, arg->inputBuffer, arg->inputBufferSize, arg->outputBuffer, arg->outputBufferSize);
	KeSetEvent (&arg->WorkItemCompletedEvent, IO_NO_INCREMENT, FALSE);
}


/**
 *
 *	\fn NTSTATUS SendDeviceIoControlRequest (PDEVICE_OBJECT deviceObject, ULONG ioControlCode, void *inputBuffer, int inputBufferSize, void *outputBuffer, int outputBufferSize)
 *	\brief Send the request control device
 *	\param[in] int outputBufferSize
 *	\param[in] void * outputBuffer
 *	\param[in] int inputBufferSize
 *	\param[in] void * inputBuffer
 *	\param[in] ULONG ioControlCode
 *	\param[in] PDEVICE_OBJECT deviceObject
 *	\return NTSTATUS STATUS_INSUFFICIENT_RESOURCES if cannot allocate variable
 *			status
 *
 */
NTSTATUS SendDeviceIoControlRequest (PDEVICE_OBJECT deviceObject, ULONG ioControlCode, void *inputBuffer, int inputBufferSize, void *outputBuffer, int outputBufferSize)
{
	IO_STATUS_BLOCK ioStatusBlock;
	NTSTATUS status;
	PIRP irp;
	KEVENT event;

	if (KeGetCurrentIrql() > APC_LEVEL)
	{
		SendDeviceIoControlRequestWorkItemArgs args;

		PIO_WORKITEM workItem = IoAllocateWorkItem (RootDeviceObject);
		if (!workItem)
			return STATUS_INSUFFICIENT_RESOURCES;

		args.deviceObject = deviceObject;
		args.ioControlCode = ioControlCode;
		args.inputBuffer = inputBuffer;
		args.inputBufferSize = inputBufferSize;
		args.outputBuffer = outputBuffer;
		args.outputBufferSize = outputBufferSize;

		KeInitializeEvent (&args.WorkItemCompletedEvent, SynchronizationEvent, FALSE);
		IoQueueWorkItem (workItem, SendDeviceIoControlRequestWorkItemRoutine, DelayedWorkQueue, &args); 

		KeWaitForSingleObject (&args.WorkItemCompletedEvent, Executive, KernelMode, FALSE, NULL);
		IoFreeWorkItem (workItem);
 
		return args.Status;
	}

	KeInitializeEvent (&event, NotificationEvent, FALSE);

	irp = IoBuildDeviceIoControlRequest (ioControlCode, deviceObject, inputBuffer, inputBufferSize,
		outputBuffer, outputBufferSize, FALSE, &event, &ioStatusBlock);

	if (!irp)
		return STATUS_INSUFFICIENT_RESOURCES;

	ObReferenceObject (deviceObject);

	status = IoCallDriver (deviceObject, irp);
	if (status == STATUS_PENDING)
	{
		KeWaitForSingleObject (&event, Executive, KernelMode, FALSE, NULL);
		status = ioStatusBlock.Status;
	}

	ObDereferenceObject (deviceObject);
	return status;
}


/**
 *
 *	\fn NTSTATUS ProbeRealDriveSize (PDEVICE_OBJECT driveDeviceObject, LARGE_INTEGER *driveSize)
 *	\brief Dump the real drive size. Containing the size of the hidden bytes
 *	\param[in] LARGE_INTEGER * driveSize
 *	\param[in] PDEVICE_OBJECT driveDeviceObject
 *	\return NTSTATUS STATUS_SUCCESS, 
 *			STATUS_ACCESS_DENIED,
 *			STATUS_INSUFFICIENT_RESOURCES,
 *			STATUS_TIMEOUT depending on errors,
 *			status else
 *
 */
NTSTATUS ProbeRealDriveSize (PDEVICE_OBJECT driveDeviceObject, LARGE_INTEGER *driveSize)
{
	NTSTATUS status;
	LARGE_INTEGER sysLength;
	LARGE_INTEGER offset;
	byte *sectorBuffer;
	ULONGLONG startTime;

	if (!UserCanAccessDriveDevice())
		return STATUS_ACCESS_DENIED;

	sectorBuffer = GSTalloc (GST_SECTOR_SIZE_BIOS);
	if (!sectorBuffer)
		return STATUS_INSUFFICIENT_RESOURCES;

	status = SendDeviceIoControlRequest (driveDeviceObject, IOCTL_DISK_GET_LENGTH_INFO,
		NULL, 0, &sysLength, sizeof (sysLength));

	if (!NT_SUCCESS (status))
	{
		Dump ("Failed to get drive size - error %x\n", status);
		GSTfree (sectorBuffer);
		return status;
	}

	startTime = KeQueryInterruptTime ();
	for (offset.QuadPart = sysLength.QuadPart; ; offset.QuadPart += GST_SECTOR_SIZE_BIOS)
	{
		status = GSTReadDevice (driveDeviceObject, sectorBuffer, offset, GST_SECTOR_SIZE_BIOS);
		
		if (NT_SUCCESS (status))
			status = GSTWriteDevice (driveDeviceObject, sectorBuffer, offset, GST_SECTOR_SIZE_BIOS);

		if (!NT_SUCCESS (status))
		{
			driveSize->QuadPart = offset.QuadPart;
			Dump ("Real drive size = %I64d bytes (%I64d hidden)\n", driveSize->QuadPart, driveSize->QuadPart - sysLength.QuadPart);
			GSTfree (sectorBuffer);
			return STATUS_SUCCESS;
		}

		if (KeQueryInterruptTime() - startTime > 3ULL * 60 * 1000 * 1000 * 10)
		{
			// Abort if probing for more than 3 minutes
			driveSize->QuadPart = sysLength.QuadPart;
			GSTfree (sectorBuffer);
			return STATUS_TIMEOUT;
		}
	}
}


/**
 *
 *	\fn NTSTATUS GSTOpenFsVolume (PEXTENSION Extension, PHANDLE volumeHandle, PFILE_OBJECT * fileObject)
 *	\brief Open file volume object
 *	\param[in] PFILE_OBJECT * fileObject
 *	\param[in] PHANDLE volumeHandle
 *	\param[in] PEXTENSION Extension
 *	\return NTSTATUS ntStatus
 *
 */
NTSTATUS GSTOpenFsVolume (PEXTENSION Extension, PHANDLE volumeHandle, PFILE_OBJECT * fileObject)
{
	NTSTATUS ntStatus;
	OBJECT_ATTRIBUTES objectAttributes;
	UNICODE_STRING fullFileName;
	IO_STATUS_BLOCK ioStatus;
	WCHAR volumeName[GST_MAX_PATH];

	GSTGetNTNameFromNumber (volumeName, Extension->nDosDriveNo);
	RtlInitUnicodeString (&fullFileName, volumeName);
	InitializeObjectAttributes (&objectAttributes, &fullFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	ntStatus = ZwCreateFile (volumeHandle,
		SYNCHRONIZE | GENERIC_READ,
		&objectAttributes,
		&ioStatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0);

	Dump ("Volume %ls open NTSTATUS 0x%08x\n", volumeName, ntStatus);

	if (!NT_SUCCESS (ntStatus))
		return ntStatus;

	ntStatus = ObReferenceObjectByHandle (*volumeHandle,
		FILE_READ_DATA,
		NULL,
		KernelMode,
		fileObject,
		NULL);

	if (!NT_SUCCESS (ntStatus))
		ZwClose (*volumeHandle);

	return ntStatus;
}


/**
 *
 *	\fn void GSTCloseFsVolume (HANDLE volumeHandle, PFILE_OBJECT fileObject)
 *	\brief Close file volume object
 *	\param[in] PFILE_OBJECT fileObject
 *	\param[in] HANDLE volumeHandle
 *	\return void
 *
 */
void GSTCloseFsVolume (HANDLE volumeHandle, PFILE_OBJECT fileObject)
{
	ObDereferenceObject (fileObject);
	ZwClose (volumeHandle);
}


/**
 *
 *	\fn static NTSTATUS GSTReadWriteDevice (BOOL write, PDEVICE_OBJECT deviceObject, PVOID buffer, LARGE_INTEGER offset, ULONG length)
 *	\brief Build a synchronous request that can be write or Read
 *	\param[in] ULONG length
 *	\param[in] LARGE_INTEGER offset
 *	\param[in] PVOID buffer
 *	\param[in] PDEVICE_OBJECT deviceObject
 *	\param[in] BOOL write
 *	\return static NTSTATUS STATUS_INSUFFICIENT_RESOURCES if cannot allocate variable
 *			status otherwise 
 *
 */
static NTSTATUS GSTReadWriteDevice (BOOL write, PDEVICE_OBJECT deviceObject, PVOID buffer, LARGE_INTEGER offset, ULONG length)
{
	NTSTATUS status;
	IO_STATUS_BLOCK ioStatusBlock;
	PIRP irp;
	KEVENT completionEvent;

	ASSERT (KeGetCurrentIrql() <= APC_LEVEL);

	KeInitializeEvent (&completionEvent, NotificationEvent, FALSE);
	irp = IoBuildSynchronousFsdRequest (write ? IRP_MJ_WRITE : IRP_MJ_READ, deviceObject, buffer, length, &offset, &completionEvent, &ioStatusBlock);
	if (!irp)
		return STATUS_INSUFFICIENT_RESOURCES;

	ObReferenceObject (deviceObject);
	status = IoCallDriver (deviceObject, irp);

	if (status == STATUS_PENDING)
	{
		status = KeWaitForSingleObject (&completionEvent, Executive, KernelMode, FALSE, NULL);
		if (NT_SUCCESS (status))
			status = ioStatusBlock.Status;
	}

	ObDereferenceObject (deviceObject);
	return status;
}


/**
 *
 *	\fn NTSTATUS GSTReadDevice (PDEVICE_OBJECT deviceObject, PVOID buffer, LARGE_INTEGER offset, ULONG length)
 *	\brief Read device
 *	\param[in] ULONG length
 *	\param[in] LARGE_INTEGER offset
 *	\param[in] PVOID buffer
 *	\param[in] PDEVICE_OBJECT deviceObject
 *	\return NTSTATUS GSTReadWriteDevice (FALSE, deviceObject, buffer, offset, length)
 *
 */
NTSTATUS GSTReadDevice (PDEVICE_OBJECT deviceObject, PVOID buffer, LARGE_INTEGER offset, ULONG length)
{
	return GSTReadWriteDevice (FALSE, deviceObject, buffer, offset, length);
}


/**
 *
 *	\fn NTSTATUS GSTWriteDevice (PDEVICE_OBJECT deviceObject, PVOID buffer, LARGE_INTEGER offset, ULONG length)
 *	\brief Write device
 *	\param[in] ULONG length
 *	\param[in] LARGE_INTEGER offset
 *	\param[in] PVOID buffer
 *	\param[in] PDEVICE_OBJECT deviceObject
 *	\return NTSTATUS GSTReadWriteDevice (TRUE, deviceObject, buffer, offset, length)
 *
 */
NTSTATUS GSTWriteDevice (PDEVICE_OBJECT deviceObject, PVOID buffer, LARGE_INTEGER offset, ULONG length)
{
	return GSTReadWriteDevice (TRUE, deviceObject, buffer, offset, length);
}

/**
 *
 *	\fn NTSTATUS GSTFsctlCall (PFILE_OBJECT fileObject, LONG IoControlCode,	void *InputBuffer, int InputBufferSize, void *OutputBuffer, int OutputBufferSize)
 *	\brief call the driver with a fileObject
 *	\param[in] PFILE_OBJECT fileObject
 *	\param[in] LONG IoControlCode
 *	\param[in] void * InputBuffer
 *	\param[in] int InputBufferSize
 *	\param[in] void * OutputBuffer
 *	\param[in] int OutputBufferSize
 *	\return NTSTATUS STATUS_INSUFFICIENT_RESOURCES if irp == NULL,
 *			ntStatus else
 *
 */
NTSTATUS GSTFsctlCall (PFILE_OBJECT fileObject, LONG IoControlCode,
	void *InputBuffer, int InputBufferSize, void *OutputBuffer, int OutputBufferSize)
{
	IO_STATUS_BLOCK ioStatusBlock;
	NTSTATUS ntStatus;
	PIRP irp;
	KEVENT event;
	PIO_STACK_LOCATION stack;
	PDEVICE_OBJECT deviceObject = IoGetRelatedDeviceObject (fileObject);

	KeInitializeEvent(&event, NotificationEvent, FALSE);

	irp = IoBuildDeviceIoControlRequest (IoControlCode,
					     deviceObject,
					     InputBuffer, InputBufferSize,
					     OutputBuffer, OutputBufferSize,
					     FALSE,
					     &event,
					     &ioStatusBlock);

	if (irp == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;

	stack = IoGetNextIrpStackLocation(irp);
	
	stack->MajorFunction = IRP_MJ_FILE_SYSTEM_CONTROL;
	stack->MinorFunction = IRP_MN_USER_FS_REQUEST;
	stack->FileObject = fileObject;

	ntStatus = IoCallDriver (deviceObject, irp);
	if (ntStatus == STATUS_PENDING)
	{
		KeWaitForSingleObject (&event, Executive, KernelMode, FALSE, NULL);
		ntStatus = ioStatusBlock.Status;
	}

	return ntStatus;
}


/**
 *
 *	\fn NTSTATUS CreateDriveLink (int nDosDriveNo)
 *	\brief Create a link to the drive
 *	\param[in] int nDosDriveNo
 *	\return NTSTATUS ntStatus
 *
 */
NTSTATUS CreateDriveLink (int nDosDriveNo)
{
	WCHAR dev[128], link[128];
	UNICODE_STRING deviceName, symLink;
	NTSTATUS ntStatus;

	GSTGetNTNameFromNumber (dev, nDosDriveNo);
	GSTGetDosNameFromNumber (link, nDosDriveNo);

	RtlInitUnicodeString (&deviceName, dev);
	RtlInitUnicodeString (&symLink, link);

	ntStatus = IoCreateSymbolicLink (&symLink, &deviceName);
	Dump ("IoCreateSymbolicLink returned %X\n", ntStatus);
	return ntStatus;
}


/**
 *
 *	\fn NTSTATUS RemoveDriveLink (int nDosDriveNo)
 *	\brief Remove the link from the drive
 *	\param[in] int nDosDriveNo
 *	\return NTSTATUS ntStatus
 *
 */
NTSTATUS RemoveDriveLink (int nDosDriveNo)
{
	WCHAR link[256];
	UNICODE_STRING symLink;
	NTSTATUS ntStatus;

	GSTGetDosNameFromNumber (link, nDosDriveNo);
	RtlInitUnicodeString (&symLink, link);

	ntStatus = IoDeleteSymbolicLink (&symLink);
	Dump ("IoDeleteSymbolicLink returned %X\n", ntStatus);
	return ntStatus;
}


/**
 *
 *	\fn NTSTATUS MountManagerMount (MOUNT_STRUCT *mount)
 *	\brief Mount the manager to mount
 *	\param[in] MOUNT_STRUCT * mount
 *	\return NTSTATUS ntStatus
 *
 */
NTSTATUS MountManagerMount (MOUNT_STRUCT *mount)
{
	NTSTATUS ntStatus; 
	WCHAR arrVolume[256];
	char buf[200];
	PMOUNTMGR_TARGET_NAME in = (PMOUNTMGR_TARGET_NAME) buf;
	PMOUNTMGR_CREATE_POINT_INPUT point = (PMOUNTMGR_CREATE_POINT_INPUT) buf;
	UNICODE_STRING symName, devName;

	GSTGetNTNameFromNumber (arrVolume, mount->nDosDriveNo);
	in->DeviceNameLength = (USHORT) wcslen (arrVolume) * 2;
	wcscpy(in->DeviceName, arrVolume);

	ntStatus = GSTDeviceIoControl (MOUNTMGR_DEVICE_NAME, IOCTL_MOUNTMGR_VOLUME_ARRIVAL_NOTIFICATION,
		in, (ULONG) (sizeof (in->DeviceNameLength) + wcslen (arrVolume) * 2), 0, 0);

	memset (buf, 0, sizeof buf);
	GSTGetDosNameFromNumber ((PWSTR) &point[1], mount->nDosDriveNo);

	point->SymbolicLinkNameOffset = sizeof (MOUNTMGR_CREATE_POINT_INPUT);
	point->SymbolicLinkNameLength = (USHORT) wcslen ((PWSTR) &point[1]) * 2;

	RtlInitUnicodeString(&symName, (PWSTR) (buf + point->SymbolicLinkNameOffset));

	point->DeviceNameOffset = point->SymbolicLinkNameOffset + point->SymbolicLinkNameLength;
	GSTGetNTNameFromNumber ((PWSTR) (buf + point->DeviceNameOffset), mount->nDosDriveNo);
	point->DeviceNameLength = (USHORT) wcslen ((PWSTR) (buf + point->DeviceNameOffset)) * 2;

	RtlInitUnicodeString(&devName, (PWSTR) (buf + point->DeviceNameOffset));

	ntStatus = GSTDeviceIoControl (MOUNTMGR_DEVICE_NAME, IOCTL_MOUNTMGR_CREATE_POINT, point,
		point->DeviceNameOffset + point->DeviceNameLength, 0, 0);

	return ntStatus;
}


/**
 *
 *	\fn NTSTATUS MountManagerUnmount (int nDosDriveNo)
 *	\brief Mount the manager to unmount
 *	\param[in] int nDosDriveNo
 *	\return NTSTATUS ntStatus
 *
 */
NTSTATUS MountManagerUnmount (int nDosDriveNo)
{
	NTSTATUS ntStatus; 
	char buf[256], out[300];
	PMOUNTMGR_MOUNT_POINT in = (PMOUNTMGR_MOUNT_POINT) buf;

	memset (buf, 0, sizeof buf);

	GSTGetDosNameFromNumber ((PWSTR) &in[1], nDosDriveNo);

	// Only symbolic link can be deleted with IOCTL_MOUNTMGR_DELETE_POINTS. If any other entry is specified, the mount manager will ignore subsequent IOCTL_MOUNTMGR_VOLUME_ARRIVAL_NOTIFICATION for the same volume ID.
	in->SymbolicLinkNameOffset = sizeof (MOUNTMGR_MOUNT_POINT);
	in->SymbolicLinkNameLength = (USHORT) wcslen ((PWCHAR) &in[1]) * 2;

	ntStatus = GSTDeviceIoControl (MOUNTMGR_DEVICE_NAME, IOCTL_MOUNTMGR_DELETE_POINTS,
		in, sizeof(MOUNTMGR_MOUNT_POINT) + in->SymbolicLinkNameLength, out, sizeof out);

	Dump ("IOCTL_MOUNTMGR_DELETE_POINTS returned 0x%08x\n", ntStatus);

	return ntStatus;
}


/**
 *
 *	\fn NTSTATUS MountDevice (PDEVICE_OBJECT DeviceObject, MOUNT_STRUCT *mount)
 *	\brief Mount device
 *	\param[in] MOUNT_STRUCT * mount
 *	\param[in] PDEVICE_OBJECT DeviceObject
 *	\return NTSTATUS STATUS_SUCCESS if success
 *			ntStatus otherwise
 *
 */
NTSTATUS MountDevice (PDEVICE_OBJECT DeviceObject, MOUNT_STRUCT *mount)
{
	PDEVICE_OBJECT NewDeviceObject;
	NTSTATUS ntStatus;

	// Make sure the user is asking for a reasonable nDosDriveNo
	if (mount->nDosDriveNo >= 0 && mount->nDosDriveNo <= 25 && IsDriveLetterAvailable (mount->nDosDriveNo))
	{
		Dump ("Mount request looks valid\n");
	}
	else
	{
		Dump ("WARNING: MOUNT DRIVE LETTER INVALID\n");
		mount->nReturnCode = ERR_DRIVE_NOT_FOUND;
		return ERR_DRIVE_NOT_FOUND;
	}

	if (!SelfTestsPassed)
	{
		mount->nReturnCode = ERR_SELF_TESTS_FAILED;
		return ERR_SELF_TESTS_FAILED;
	}

	ntStatus = GSTCreateDeviceObject (DeviceObject->DriverObject, &NewDeviceObject, mount);

	if (!NT_SUCCESS (ntStatus))
	{
		Dump ("Mount CREATE DEVICE ERROR, ntStatus = 0x%08x\n", ntStatus);
		return ntStatus;
	}
	else
	{
		PEXTENSION NewExtension = (PEXTENSION) NewDeviceObject->DeviceExtension;
		SECURITY_SUBJECT_CONTEXT subContext;
		PACCESS_TOKEN accessToken;

		SeCaptureSubjectContext (&subContext);
		accessToken = SeQuerySubjectContextToken (&subContext);

		if (!accessToken)
		{
			ntStatus = STATUS_INVALID_PARAMETER;
		}
		else
		{
			PTOKEN_USER tokenUser;

			ntStatus = SeQueryInformationToken (accessToken, TokenUser, &tokenUser);
			if (NT_SUCCESS (ntStatus))
			{
				ULONG sidLength = RtlLengthSid (tokenUser->User.Sid);

				NewExtension->UserSid = GSTalloc (sidLength);
				if (!NewExtension->UserSid)
					ntStatus = STATUS_INSUFFICIENT_RESOURCES;
				else
					ntStatus = RtlCopySid (sidLength, NewExtension->UserSid, tokenUser->User.Sid);

				ExFreePool (tokenUser);		// Documented in newer versions of WDK
			}
		}

		SeReleaseSubjectContext (&subContext);

		if (NT_SUCCESS (ntStatus))
			ntStatus = GSTStartVolumeThread (NewDeviceObject, NewExtension, mount);

		if (!NT_SUCCESS (ntStatus))
		{
			Dump ("Mount FAILURE NT ERROR, ntStatus = 0x%08x\n", ntStatus);
			GSTDeleteDeviceObject (NewDeviceObject, NewExtension);
			return ntStatus;
		}
		else
		{
			if (mount->nReturnCode == 0)
			{
				HANDLE volumeHandle;
				PFILE_OBJECT volumeFileObject;

				Dump ("Mount SUCCESS GST code = 0x%08x READ-ONLY = %d\n", mount->nReturnCode, NewExtension->bReadOnly);

				if (NewExtension->bReadOnly)
					NewDeviceObject->Characteristics |= FILE_READ_ONLY_DEVICE;

				NewDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

				NewExtension->UniqueVolumeId = LastUniqueVolumeId++;

				if (mount->bMountManager)
					MountManagerMount (mount);

				NewExtension->bMountManager = mount->bMountManager;

				// We create symbolic link even if mount manager is notified of
				// arriving volume as it apparently sometimes fails to create the link
				CreateDriveLink (mount->nDosDriveNo);

				mount->FilesystemDirty = FALSE;

				if (NT_SUCCESS (GSTOpenFsVolume (NewExtension, &volumeHandle, &volumeFileObject)))
				{
					__try
					{
						ULONG fsStatus;

						if (NT_SUCCESS (GSTFsctlCall (volumeFileObject, FSCTL_IS_VOLUME_DIRTY, NULL, 0, &fsStatus, sizeof (fsStatus)))
							&& (fsStatus & VOLUME_IS_DIRTY))
						{
							mount->FilesystemDirty = TRUE;
						}
					}
					__except (EXCEPTION_EXECUTE_HANDLER)
					{
						mount->FilesystemDirty = TRUE;
					}


					GSTCloseFsVolume (volumeHandle, volumeFileObject);
				}
			}
			else
			{
				Dump ("Mount FAILURE GST code = 0x%08x\n", mount->nReturnCode);
				GSTDeleteDeviceObject (NewDeviceObject, NewExtension);
			}
			
			return STATUS_SUCCESS;
		}
	}
}

/**
 *
 *	\fn NTSTATUS UnmountDevice (UNMOUNT_STRUCT *unmountRequest, PDEVICE_OBJECT deviceObject, BOOL ignoreOpenFiles)
 *	\brief Unmount Device
 *	\param[in] BOOL ignoreOpenFiles
 *	\param[in] PDEVICE_OBJECT deviceObject
 *	\param[in] UNMOUNT_STRUCT * unmountRequest
 *	\return NTSTATUS ERR_FILES_OPEN if a file cannot open
 *			0 otherwise
 *
 */
NTSTATUS UnmountDevice (UNMOUNT_STRUCT *unmountRequest, PDEVICE_OBJECT deviceObject, BOOL ignoreOpenFiles)
{
	PEXTENSION extension = deviceObject->DeviceExtension;
	NTSTATUS ntStatus;
	HANDLE volumeHandle;
	PFILE_OBJECT volumeFileObject;

	Dump ("UnmountDevice %d\n", extension->nDosDriveNo);

	ntStatus = GSTOpenFsVolume (extension, &volumeHandle, &volumeFileObject);

	if (NT_SUCCESS (ntStatus))
	{
		int dismountRetry;

		// Dismounting a writable NTFS filesystem prevents the driver from being unloaded on Windows 7
		if (IsOSAtLeast (WIN_7) && !extension->bReadOnly)
		{
			NTFS_VOLUME_DATA_BUFFER ntfsData;

			if (NT_SUCCESS (GSTFsctlCall (volumeFileObject, FSCTL_GET_NTFS_VOLUME_DATA, NULL, 0, &ntfsData, sizeof (ntfsData))))
				DriverUnloadDisabled = TRUE;
		}

		// Lock volume
		ntStatus = GSTFsctlCall (volumeFileObject, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0);
		Dump ("FSCTL_LOCK_VOLUME returned %X\n", ntStatus);

		if (!NT_SUCCESS (ntStatus) && !ignoreOpenFiles)
		{
			GSTCloseFsVolume (volumeHandle, volumeFileObject);
			return ERR_FILES_OPEN;
		}

		// Dismount volume
		for (dismountRetry = 0; dismountRetry < 200; ++dismountRetry)
		{
			ntStatus = GSTFsctlCall (volumeFileObject, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0);
			Dump ("FSCTL_DISMOUNT_VOLUME returned %X\n", ntStatus);

			if (NT_SUCCESS (ntStatus) || ntStatus == STATUS_VOLUME_DISMOUNTED)
				break;

			if (!ignoreOpenFiles)
			{
				GSTCloseFsVolume (volumeHandle, volumeFileObject);
				return ERR_FILES_OPEN;
			}

			GSTSleep (100);
		}
	}
	else 
	{
		// Volume cannot be opened => force dismount if allowed
		if (!ignoreOpenFiles)
			return ERR_FILES_OPEN;
		else
			volumeHandle = NULL;
	}

	if (extension->bMountManager)
		MountManagerUnmount (extension->nDosDriveNo);

	// We always remove symbolic link as mount manager might fail to do so
	RemoveDriveLink (extension->nDosDriveNo);

	extension->bShuttingDown = TRUE;

	ntStatus = IoAcquireRemoveLock (&extension->Queue.RemoveLock, NULL);
	ASSERT (NT_SUCCESS (ntStatus));
	IoReleaseRemoveLockAndWait (&extension->Queue.RemoveLock, NULL);

	if (volumeHandle != NULL)
		GSTCloseFsVolume (volumeHandle, volumeFileObject);

	if (unmountRequest)
	{
		PCRYPTO_INFO cryptoInfo = ((PEXTENSION) deviceObject->DeviceExtension)->cryptoInfo;
		unmountRequest->HiddenVolumeProtectionTriggered = (cryptoInfo->bProtectHiddenVolume && cryptoInfo->bHiddenVolProtectionAction);
	}

	GSTDeleteDeviceObject (deviceObject, (PEXTENSION) deviceObject->DeviceExtension);
	return 0;
}


/**
 *
 *	\fn static PDEVICE_OBJECT FindVolumeWithHighestUniqueId (int maxUniqueId)
 *	\brief Getter for the highest id volume
 *	\param[in] int maxUniqueId
 *	\return static PDEVICE_OBJECT highestIdDevice
 *
 */
static PDEVICE_OBJECT FindVolumeWithHighestUniqueId (int maxUniqueId)
{
	PDEVICE_OBJECT highestIdDevice = NULL;
	int highestId = -1;
	int drive;

	for (drive = MIN_MOUNTED_VOLUME_DRIVE_NUMBER; drive <= MAX_MOUNTED_VOLUME_DRIVE_NUMBER; ++drive)
	{
		PDEVICE_OBJECT device = GetVirtualVolumeDeviceObject (drive);
		if (device)
		{
			PEXTENSION extension = (PEXTENSION) device->DeviceExtension;
			if (extension->UniqueVolumeId > highestId && extension->UniqueVolumeId <= maxUniqueId)
			{
				highestId = extension->UniqueVolumeId;
				highestIdDevice = device;
			}
		}
	}

	return highestIdDevice;
}


/**
 *
 *	\fn NTSTATUS UnmountAllDevices (UNMOUNT_STRUCT *unmountRequest, BOOL ignoreOpenFiles)
 *	\brief Unmount all devices
 *	\param[in] BOOL ignoreOpenFiles
 *	\param[in] UNMOUNT_STRUCT * unmountRequest
 *	\return NTSTATUS status
 *
 */
NTSTATUS UnmountAllDevices (UNMOUNT_STRUCT *unmountRequest, BOOL ignoreOpenFiles)
{
	NTSTATUS status = 0;
	PDEVICE_OBJECT ListDevice;
	int maxUniqueId = LastUniqueVolumeId;

	Dump ("Unmounting all volumes\n");

	if (unmountRequest)
		unmountRequest->HiddenVolumeProtectionTriggered = FALSE;

	// Dismount volumes in the reverse order they were mounted to properly dismount nested volumes
	while ((ListDevice = FindVolumeWithHighestUniqueId (maxUniqueId)) != NULL)
	{
		PEXTENSION ListExtension = (PEXTENSION) ListDevice->DeviceExtension;
		maxUniqueId = ListExtension->UniqueVolumeId - 1;

		if (IsVolumeAccessibleByCurrentUser (ListExtension))
		{
			NTSTATUS ntStatus;

			if (unmountRequest)
				unmountRequest->nDosDriveNo = ListExtension->nDosDriveNo;

			ntStatus = UnmountDevice (unmountRequest, ListDevice, ignoreOpenFiles);
			status = ntStatus == 0 ? status : ntStatus;

			if (unmountRequest && unmountRequest->HiddenVolumeProtectionTriggered)
				break;
		}
	}

	return status;
}


/**
 *
 *	\fn NTSTATUS SymbolicLinkToTarget (PWSTR symlinkName, PWSTR targetName, USHORT maxTargetNameLength)
 *	\brief Resolves symbolic link name to its target name
 *	\param[in] USHORT maxTargetNameLength
 *	\param[in] PWSTR targetName
 *	\param[in] PWSTR symlinkName
 *	\return NTSTATUS ntStatus
 *
 */
NTSTATUS SymbolicLinkToTarget (PWSTR symlinkName, PWSTR targetName, USHORT maxTargetNameLength)
{
	NTSTATUS ntStatus;
	OBJECT_ATTRIBUTES objectAttributes;
	UNICODE_STRING fullFileName;
	HANDLE handle;

	RtlInitUnicodeString (&fullFileName, symlinkName);
	InitializeObjectAttributes (&objectAttributes, &fullFileName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

	ntStatus = ZwOpenSymbolicLinkObject (&handle, GENERIC_READ, &objectAttributes);

	if (NT_SUCCESS (ntStatus))
	{
		UNICODE_STRING target;
		target.Buffer = targetName;
		target.Length = 0;
		target.MaximumLength = maxTargetNameLength;
		memset (targetName, 0, maxTargetNameLength);

		ntStatus = ZwQuerySymbolicLinkObject (handle, &target, NULL);

		ZwClose (handle);
	}

	return ntStatus;
}



/**
 *
 *	\fn BOOL RegionsOverlap (unsigned __int64 start1, unsigned __int64 end1, unsigned __int64 start2, unsigned __int64 end2)
 *	\brief Checks if two regions overlap (borders are parts of regions)
 *	\param[in] unsigned __int64 end2
 *	\param[in] unsigned __int64 start2
 *	\param[in] unsigned __int64 end1
 *	\param[in] unsigned __int64 start1
 *	\return BOOL (start1 < start2) ? (end1 >= start2) : (start1 <= end2)
 *
 */
BOOL RegionsOverlap (unsigned __int64 start1, unsigned __int64 end1, unsigned __int64 start2, unsigned __int64 end2)
{
	return (start1 < start2) ? (end1 >= start2) : (start1 <= end2);
}


/**
 *
 *	\fn void GetIntersection (uint64 start1, uint32 length1, uint64 start2, uint64 end2, uint64 *intersectStart, uint32 *intersectLength)
 *	\brief Getter for the intersection start address and length
 *	\param[in] uint32 * intersectLength
 *	\param[in] uint64 * intersectStart
 *	\param[in] uint64 end2
 *	\param[in] uint64 start2
 *	\param[in] uint32 length1
 *	\param[in] uint64 start1
 *	\return void
 *
 */
void GetIntersection (uint64 start1, uint32 length1, uint64 start2, uint64 end2, uint64 *intersectStart, uint32 *intersectLength)
{
	uint64 end1 = start1 + length1 - 1;
	uint64 intersectEnd = (end1 <= end2) ? end1 : end2;
	
	*intersectStart = (start1 >= start2) ? start1 : start2;
	*intersectLength = (uint32) ((*intersectStart > intersectEnd) ? 0 : intersectEnd + 1 - *intersectStart);
	
	if (*intersectLength == 0)
		*intersectStart = start1;
}


/**
 *
 *	\fn BOOL IsAccessibleByUser (PUNICODE_STRING objectFileName, BOOL readOnly)
 *	\brief Getter to know if the user can access the object
 *	\param[in] BOOL readOnly
 *	\param[in] PUNICODE_STRING objectFileName
 *	\return BOOL TRUE if accessible
 *			FALSE if not
 *
 */
BOOL IsAccessibleByUser (PUNICODE_STRING objectFileName, BOOL readOnly)
{
	OBJECT_ATTRIBUTES fileObjAttributes;
	IO_STATUS_BLOCK ioStatusBlock;
	HANDLE fileHandle;
	NTSTATUS status;

	ASSERT (!IoIsSystemThread (PsGetCurrentThread()));

	InitializeObjectAttributes (&fileObjAttributes, objectFileName, OBJ_CASE_INSENSITIVE | OBJ_FORCE_ACCESS_CHECK | OBJ_KERNEL_HANDLE, NULL, NULL);
	
	status = ZwCreateFile (&fileHandle,
		readOnly ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE,
		&fileObjAttributes,
		&ioStatusBlock,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_OPEN,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0);

	if (NT_SUCCESS (status))
	{
		ZwClose (fileHandle);
		return TRUE;
	}

	return FALSE;
}


/**
 *
 *	\fn BOOL UserCanAccessDriveDevice ()
 *	\brief Getter to know if the drive is accessible
 *	\return BOOL IsAccessibleByUser (&name, FALSE)
 *
 */
BOOL UserCanAccessDriveDevice ()
{
	UNICODE_STRING name;
	RtlInitUnicodeString (&name, L"\\Device\\MountPointManager");

	return IsAccessibleByUser (&name, FALSE);
}


/**
 *
 *	\fn BOOL IsDriveLetterAvailable (int nDosDriveNo)
 *	\brief Getter to know if the letter can be taken (for the drive number)
 *	\param[in] int nDosDriveNo
 *	\return BOOL FALSE if not succeed
 *			TRUE otherwise
 *
 */
BOOL IsDriveLetterAvailable (int nDosDriveNo)
{
	OBJECT_ATTRIBUTES objectAttributes;
	UNICODE_STRING objectName;
	WCHAR link[128];
	HANDLE handle;

	GSTGetDosNameFromNumber (link, nDosDriveNo);
	RtlInitUnicodeString (&objectName, link);
	InitializeObjectAttributes (&objectAttributes, &objectName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

	if (NT_SUCCESS (ZwOpenSymbolicLinkObject (&handle, GENERIC_READ, &objectAttributes)))
	{
		ZwClose (handle);
		return FALSE;
	}

	return TRUE;
}


/**
 *
 *	\fn NTSTATUS GSTCompleteIrp (PIRP irp, NTSTATUS status, ULONG_PTR information)
 *	\brief Complete the irp packet
 *	\param[in] ULONG_PTR information
 *	\param[in] NTSTATUS status
 *	\param[in] PIRP irp
 *	\return NTSTATUS status
 *
 */
NTSTATUS GSTCompleteIrp (PIRP irp, NTSTATUS status, ULONG_PTR information)
{
	irp->IoStatus.Status = status;
	irp->IoStatus.Information = information;
	IoCompleteRequest (irp, IO_NO_INCREMENT);
	return status;
}


/**
 *
 *	\fn NTSTATUS GSTCompleteDiskIrp (PIRP irp, NTSTATUS status, ULONG_PTR information)
 *	\brief Complete a drisk irp packet
 *	\param[in] ULONG_PTR information
 *	\param[in] NTSTATUS status
 *	\param[in] PIRP irp
 *	\return NTSTATUS status
 *
 */
NTSTATUS GSTCompleteDiskIrp (PIRP irp, NTSTATUS status, ULONG_PTR information)
{
	irp->IoStatus.Status = status;
	irp->IoStatus.Information = information;
	IoCompleteRequest (irp, NT_SUCCESS (status) ? IO_DISK_INCREMENT : IO_NO_INCREMENT);
	return status;
}


/**
 *
 *	\fn size_t GetCpuCount ()
 *	\brief Get the cpu count
 *	\return 1 if cpuCount == 0,
 *			cpuCount else
 *
 */
size_t GetCpuCount ()
{
	KAFFINITY activeCpuMap = KeQueryActiveProcessors();
	size_t mapSize = sizeof (activeCpuMap) * 8;
	size_t cpuCount = 0;

	while (mapSize--)
	{
		if (activeCpuMap & 1)
			++cpuCount;

		activeCpuMap >>= 1;
	}

	if (cpuCount == 0)
		return 1;

	return cpuCount;
}


/**
 *
 *	\fn void EnsureNullTerminatedString (wchar_t *str, size_t maxSizeInBytes)
 *	\brief Complete a string with 0
 *	\param[in] size_t maxSizeInBytes
 *	\param[in] wchar_t * str
 *	\return void
 *
 */
void EnsureNullTerminatedString (wchar_t *str, size_t maxSizeInBytes)
{
	ASSERT ((maxSizeInBytes & 1) == 0);
	str[maxSizeInBytes / sizeof (wchar_t) - 1] = 0;
}


/**
 *
 *	\fn void *AllocateMemoryWithTimeout (size_t size, int retryDelay, int timeout)
 *	\brief allocate memory with a life time "timeout" specififed
 *	\param[in] int timeout
 *	\param[in] int retryDelay
 *	\param[in] size_t size
 *	\return void *
 *
 */
void *AllocateMemoryWithTimeout (size_t size, int retryDelay, int timeout)
{
	LARGE_INTEGER waitInterval;
	waitInterval.QuadPart = retryDelay * -10000;

	ASSERT (KeGetCurrentIrql() <= APC_LEVEL);
	ASSERT (retryDelay > 0 && retryDelay <= timeout);

	while (TRUE)
	{
		void *memory = GSTalloc (size);
		if (memory)
			return memory;

		timeout -= retryDelay;
		if (timeout <= 0)
			break;

		KeDelayExecutionThread (KernelMode, FALSE, &waitInterval);
	}

	return NULL;
}


/**
 *
 *	\fn NTSTATUS GSTReadRegistryKey (PUNICODE_STRING keyPath, wchar_t *keyValueName, PKEY_VALUE_PARTIAL_INFORMATION *keyData)
 *	\brief Read a windows registry key
 *	\param[in] PKEY_VALUE_PARTIAL_INFORMATION * keyData
 *	\param[in] wchar_t * keyValueName
 *	\param[in] PUNICODE_STRING keyPath
 *	\return NTSTATUS STATUS_NO_DATA_DETECTED if no data detected
 *			STATUS_INSUFFICIENT_RESOURCES if can't allocate variable
 *			status otherwise
 *
 */
NTSTATUS GSTReadRegistryKey (PUNICODE_STRING keyPath, wchar_t *keyValueName, PKEY_VALUE_PARTIAL_INFORMATION *keyData)
{
	OBJECT_ATTRIBUTES regObjAttribs;
	HANDLE regKeyHandle;
	NTSTATUS status;
	UNICODE_STRING valName;
	ULONG size = 0;
	ULONG resultSize;

	InitializeObjectAttributes (&regObjAttribs, keyPath, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);
	status = ZwOpenKey (&regKeyHandle, KEY_READ, &regObjAttribs);
	if (!NT_SUCCESS (status))
		return status;

	RtlInitUnicodeString (&valName, keyValueName);
	status = ZwQueryValueKey (regKeyHandle, &valName, KeyValuePartialInformation, NULL, 0, &size);
		
	if (!NT_SUCCESS (status) && status != STATUS_BUFFER_OVERFLOW && status != STATUS_BUFFER_TOO_SMALL)
	{
		ZwClose (regKeyHandle);
		return status;
	}

	if (size == 0)
	{
		ZwClose (regKeyHandle);
		return STATUS_NO_DATA_DETECTED;
	}

	*keyData = (PKEY_VALUE_PARTIAL_INFORMATION) GSTalloc (size);
	if (!*keyData)
	{
		ZwClose (regKeyHandle);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	status = ZwQueryValueKey (regKeyHandle, &valName, KeyValuePartialInformation, *keyData, size, &resultSize);

	ZwClose (regKeyHandle);
	return status;
}


/**
 *
 *	\fn NTSTATUS GSTWriteRegistryKey (PUNICODE_STRING keyPath, wchar_t *keyValueName, ULONG keyValueType, void *valueData, ULONG valueSize)
 *	\brief Write (for) a windows registry key
 *	\param[in] ULONG valueSize
 *	\param[in] void * valueData
 *	\param[in] ULONG keyValueType
 *	\param[in] wchar_t * keyValueName
 *	\param[in] PUNICODE_STRING keyPath
 *	\return NTSTATUS status
 *
 */
NTSTATUS GSTWriteRegistryKey (PUNICODE_STRING keyPath, wchar_t *keyValueName, ULONG keyValueType, void *valueData, ULONG valueSize)
{
	OBJECT_ATTRIBUTES regObjAttribs;
	HANDLE regKeyHandle;
	NTSTATUS status;
	UNICODE_STRING valName;

	InitializeObjectAttributes (&regObjAttribs, keyPath, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);
	status = ZwOpenKey (&regKeyHandle, KEY_READ | KEY_WRITE, &regObjAttribs);
	if (!NT_SUCCESS (status))
		return status;

	RtlInitUnicodeString (&valName, keyValueName);

	status = ZwSetValueKey (regKeyHandle, &valName, 0, keyValueType, valueData, valueSize);

	ZwClose (regKeyHandle);
	return status;
}


/**
 *
 *	\fn BOOL IsVolumeClassFilterRegistered ()
 *	\brief Check if the volume class exists
 *	\return BOOL TRUE if active
 *			FALSE otherwise
 *
 */
BOOL IsVolumeClassFilterRegistered ()
{
	UNICODE_STRING name;
	NTSTATUS status;
	BOOL registered = FALSE;

	PKEY_VALUE_PARTIAL_INFORMATION data;

	RtlInitUnicodeString (&name, L"\\REGISTRY\\MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Class\\{71A27CDD-812A-11D0-BEC7-08002BE2092F}");
	status = GSTReadRegistryKey (&name, L"UpperFilters", &data);

	if (NT_SUCCESS (status))
	{
		if (data->Type == REG_MULTI_SZ && data->DataLength >= 9 * sizeof (wchar_t))
		{
			// Search for the string "gostcrypt"
			ULONG i;
			for (i = 0; i <= data->DataLength - 9 * sizeof (wchar_t); ++i)
			{
				if (memcmp (data->Data + i, L"gostcrypt", 9 * sizeof (wchar_t)) == 0)
				{
					Dump ("Volume class filter active\n");
					registered = TRUE;
					break;
				}
			}
		}

		GSTfree (data);
	}

	return registered;
}


/**
 *
 *	\fn NTSTATUS ReadRegistryConfigFlags (BOOL driverEntry)
 *	\brief Read registry configuration flags
 *	\param[in] BOOL driverEntry
 *	\return NTSTATUS status
 *
 */
NTSTATUS ReadRegistryConfigFlags (BOOL driverEntry)
{
	PKEY_VALUE_PARTIAL_INFORMATION data;
	UNICODE_STRING name;
	NTSTATUS status;
	uint32 flags = 0;

	RtlInitUnicodeString (&name, L"\\REGISTRY\\MACHINE\\SYSTEM\\CurrentControlSet\\Services\\gostcrypt");
	status = GSTReadRegistryKey (&name, GST_DRIVER_CONFIG_REG_VALUE_NAME, &data);

	if (NT_SUCCESS (status))
	{
		if (data->Type == REG_DWORD)
		{
			flags = *(uint32 *) data->Data;
			Dump ("Configuration flags = 0x%x\n", flags);

			if (driverEntry)
			{
				if (flags & (GST_DRIVER_CONFIG_CACHE_BOOT_PASSWORD | GST_DRIVER_CONFIG_CACHE_BOOT_PASSWORD_FOR_SYS_FAVORITES))
					CacheBootPassword = TRUE;

				if (flags & GST_DRIVER_CONFIG_DISABLE_NONADMIN_SYS_FAVORITES_ACCESS)
					NonAdminSystemFavoritesAccessDisabled = TRUE;
			}

			EnableHwEncryption ((flags & GST_DRIVER_CONFIG_DISABLE_HARDWARE_ENCRYPTION) ? FALSE : TRUE);
		}
		else
			status = STATUS_INVALID_PARAMETER;

		GSTfree (data);
	}

	if (driverEntry && NT_SUCCESS (GSTReadRegistryKey (&name, GST_ENCRYPTION_FREE_CPU_COUNT_REG_VALUE_NAME, &data)))
	{
		if (data->Type == REG_DWORD)
			EncryptionThreadPoolFreeCpuCountLimit = *(uint32 *) data->Data;

		GSTfree (data);
	}

	return status;
}


/**
 *
 *	\fn NTSTATUS WriteRegistryConfigFlags (uint32 flags)
 *	\brief Write the registry conig flags.
 *	\param[in] uint32 flags
 *	\return NTSTATUS GSTWriteRegistryKey (&name, GST_DRIVER_CONFIG_REG_VALUE_NAME, REG_DWORD, &flags, sizeof (flags))
 *
 */
NTSTATUS WriteRegistryConfigFlags (uint32 flags)
{
	UNICODE_STRING name;
	RtlInitUnicodeString (&name, L"\\REGISTRY\\MACHINE\\SYSTEM\\CurrentControlSet\\Services\\gostcrypt");

	return GSTWriteRegistryKey (&name, GST_DRIVER_CONFIG_REG_VALUE_NAME, REG_DWORD, &flags, sizeof (flags));
}


/**
 *
 *	\fn NTSTATUS GetDeviceSectorSize (PDEVICE_OBJECT deviceObject, ULONG *bytesPerSector)
 *	\brief Get the device sector sizes.
 *	\param[in] ULONG * bytesPerSector
 *	\param[in] PDEVICE_OBJECT deviceObject
 *	\return NTSTATUS STATUS_SUCCESS if success
 *			status otherwise
 *
 */
NTSTATUS GetDeviceSectorSize (PDEVICE_OBJECT deviceObject, ULONG *bytesPerSector)
{
	NTSTATUS status;
	DISK_GEOMETRY geometry;

	status = SendDeviceIoControlRequest (deviceObject, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &geometry, sizeof (geometry));

	if (!NT_SUCCESS (status))
		return status;

	*bytesPerSector = geometry.BytesPerSector;
	return STATUS_SUCCESS;
}


/**
 *
 *	\fn NTSTATUS ZeroUnreadableSectors (PDEVICE_OBJECT deviceObject, LARGE_INTEGER startOffset, ULONG size, uint64 *zeroedSectorCount)
 *	\brief Return the number of sectors unreadable in zeroedSectorCount
 *	\param[in] uint64 * zeroedSectorCount
 *	\param[in] ULONG size
 *	\param[in] LARGE_INTEGER startOffset
 *	\param[in] PDEVICE_OBJECT deviceObject
 *	\return NTSTATUS STATUS_INSUFFICIENT_RESOURCES if cannot allocate variable
 *			status otherwise
 *
 */
NTSTATUS ZeroUnreadableSectors (PDEVICE_OBJECT deviceObject, LARGE_INTEGER startOffset, ULONG size, uint64 *zeroedSectorCount)
{
	NTSTATUS status;
	ULONG sectorSize;
	ULONG sectorCount;
	byte *sectorBuffer = NULL;

	*zeroedSectorCount = 0;

	status = GetDeviceSectorSize (deviceObject, &sectorSize);
	if (!NT_SUCCESS (status))
		return status;

	sectorBuffer = GSTalloc (sectorSize);
	if (!sectorBuffer)
		return STATUS_INSUFFICIENT_RESOURCES;

	for (sectorCount = size / sectorSize; sectorCount > 0; --sectorCount, startOffset.QuadPart += sectorSize)
	{
		status = GSTReadDevice (deviceObject, sectorBuffer, startOffset, sectorSize);
		if (!NT_SUCCESS (status))
		{
			Dump ("Zeroing sector at %I64d\n", startOffset.QuadPart);
			memset (sectorBuffer, 0, sectorSize);

			status = GSTWriteDevice (deviceObject, sectorBuffer, startOffset, sectorSize);
			if (!NT_SUCCESS (status))
				goto err;

			++(*zeroedSectorCount);
		}
	}

	status = STATUS_SUCCESS;

err:
	if (sectorBuffer)
		GSTfree (sectorBuffer);

	return status;
}


/**
 *
 *	\fn NTSTATUS ReadDeviceSkipUnreadableSectors (PDEVICE_OBJECT deviceObject, byte *buffer, LARGE_INTEGER startOffset, ULONG size, uint64 *badSectorCount)
 *	\brief Read a device and skip unreadable sectors so the read doesn't stop at the first unreadable sector but still continue.
 *	\param[in] uint64 * badSectorCount
 *	\param[in] ULONG size
 *	\param[in] LARGE_INTEGER startOffset
 *	\param[in] byte * buffer
 *	\param[in] PDEVICE_OBJECT deviceObject
 *	\return NTSTATUS STATUS_SUCCESS if success
 *			status otherwise
 *
 */
NTSTATUS ReadDeviceSkipUnreadableSectors (PDEVICE_OBJECT deviceObject, byte *buffer, LARGE_INTEGER startOffset, ULONG size, uint64 *badSectorCount)
{
	NTSTATUS status;
	ULONG sectorSize;
	ULONG sectorCount;

	*badSectorCount = 0;

	status = GetDeviceSectorSize (deviceObject, &sectorSize);
	if (!NT_SUCCESS (status))
		return status;

	for (sectorCount = size / sectorSize; sectorCount > 0; --sectorCount, startOffset.QuadPart += sectorSize, buffer += sectorSize)
	{
		status = GSTReadDevice (deviceObject, buffer, startOffset, sectorSize);
		if (!NT_SUCCESS (status))
		{
			Dump ("Skipping bad sector at %I64d\n", startOffset.QuadPart);
			memset (buffer, 0, sectorSize);
			++(*badSectorCount);
		}
	}

	return STATUS_SUCCESS;
}


/**
 *
 *	\fn BOOL IsVolumeAccessibleByCurrentUser (PEXTENSION volumeDeviceExtension)
 *	\brief Getter to know if the user can access the volume.
 *	\param[in] PEXTENSION volumeDeviceExtension
 *	\return BOOL TRUE if accessible
 *			FALSE otherwise
 *
 */
BOOL IsVolumeAccessibleByCurrentUser (PEXTENSION volumeDeviceExtension)
{
	SECURITY_SUBJECT_CONTEXT subContext;
	PACCESS_TOKEN accessToken;
	PTOKEN_USER tokenUser;
	BOOL result = FALSE;

	if (IoIsSystemThread (PsGetCurrentThread())
		|| UserCanAccessDriveDevice()
		|| !volumeDeviceExtension->UserSid
		|| (volumeDeviceExtension->SystemFavorite && !NonAdminSystemFavoritesAccessDisabled))
	{
		return TRUE;
	}

	SeCaptureSubjectContext (&subContext);
	accessToken = SeQuerySubjectContextToken (&subContext);

	if (!accessToken)
		goto ret;
	
	if (SeTokenIsAdmin (accessToken))
	{
		result = TRUE;
		goto ret;
	}

	if (!NT_SUCCESS (SeQueryInformationToken (accessToken, TokenUser, &tokenUser)))
		goto ret;

	result = RtlEqualSid (volumeDeviceExtension->UserSid, tokenUser->User.Sid);
	ExFreePool (tokenUser);		// Documented in newer versions of WDK

ret:
	SeReleaseSubjectContext (&subContext);
	return result;
}


/**
 *
 *	\fn void GetElapsedTimeInit (LARGE_INTEGER *lastPerfCounter)
 *	\brief Getter for the time initialisation to know how many time passed.
 *	\param[in] LARGE_INTEGER * lastPerfCounter
 *	\return void
 *
 */
void GetElapsedTimeInit (LARGE_INTEGER *lastPerfCounter)
{
	*lastPerfCounter = KeQueryPerformanceCounter (NULL);
}


/**
 *
 *	\fn int64 GetElapsedTime (LARGE_INTEGER *lastPerfCounter)
 *	\brief Returns elapsed time in microseconds since last call
 *	\param[in] LARGE_INTEGER * lastPerfCounter
 *	\return int64 elapsed
 *
 */
int64 GetElapsedTime (LARGE_INTEGER *lastPerfCounter)
{
	LARGE_INTEGER freq;
	LARGE_INTEGER counter = KeQueryPerformanceCounter (&freq);

	int64 elapsed = (counter.QuadPart - lastPerfCounter->QuadPart) * 1000000LL / freq.QuadPart;
	*lastPerfCounter = counter;

	return elapsed;
}


/**
 *
 *	\fn BOOL IsOSAtLeast (OSVersionEnum reqMinOS)
 *	\brief Return the minimum version of the os in argument to run gostcrypt.
 *	\param[in] OSVersionEnum reqMinOS
 *	\return BOOL ((OsMajorVersion << 16 | OsMinorVersion << 8) >= (major << 16 | minor << 8))
 *
 */
BOOL IsOSAtLeast (OSVersionEnum reqMinOS)
{
	/* When updating this function, update IsOSVersionAtLeast() in Dlgcode.c too. */

	ULONG major = 0, minor = 0;

	ASSERT (OsMajorVersion != 0);

	switch (reqMinOS)
	{
	case WIN_2000:			major = 5; minor = 0; break;
	case WIN_XP:			major = 5; minor = 1; break;
	case WIN_SERVER_2003:	major = 5; minor = 2; break;
	case WIN_VISTA:			major = 6; minor = 0; break;
	case WIN_7:				major = 6; minor = 1; break;

	default:
		GST_THROW_FATAL_EXCEPTION;
		break;
	}

	return ((OsMajorVersion << 16 | OsMinorVersion << 8)
		>= (major << 16 | minor << 8));
}
