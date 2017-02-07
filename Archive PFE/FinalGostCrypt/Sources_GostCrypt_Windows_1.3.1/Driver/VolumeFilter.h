/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#ifndef GST_HEADER_DRIVER_VOLUME_FILTER
#define GST_HEADER_DRIVER_VOLUME_FILTER

#include "Gstdefs.h"

extern uint32 HiddenSysLeakProtectionCount;

NTSTATUS VolumeFilterAddDevice (PDRIVER_OBJECT driverObject, PDEVICE_OBJECT pdo);
NTSTATUS VolumeFilterDispatchIrp (PDEVICE_OBJECT DeviceObject, PIRP Irp);

#endif // GST_HEADER_DRIVER_VOLUME_FILTER
