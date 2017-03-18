/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Core_Unix_CoreServiceResponse
#define GST_HEADER_Core_Unix_CoreServiceResponse

#include "Platform/Serializable.h"
#include "Core/Core.h"

namespace GostCrypt
{
	struct CoreServiceResponse : public Serializable
	{
	};

	struct CheckFilesystemResponse : CoreServiceResponse
	{
		CheckFilesystemResponse () { }
		GST_SERIALIZABLE (CheckFilesystemResponse);
	};

	struct DismountFilesystemResponse : CoreServiceResponse
	{
		DismountFilesystemResponse () { }
		GST_SERIALIZABLE (DismountFilesystemResponse);
	};

	struct DismountVolumeResponse : CoreServiceResponse
	{
		DismountVolumeResponse () { }
		GST_SERIALIZABLE (DismountVolumeResponse);

		shared_ptr <VolumeInfo> DismountedVolumeInfo;
	};

	struct GetDeviceSectorSizeResponse : CoreServiceResponse
	{
		GetDeviceSectorSizeResponse () { }
		GetDeviceSectorSizeResponse (uint32 size) : Size (size) { }
		GST_SERIALIZABLE (GetDeviceSectorSizeResponse);

		uint32 Size;
	};

	struct GetDeviceSizeResponse : CoreServiceResponse
	{
		GetDeviceSizeResponse () { }
		GetDeviceSizeResponse (uint64 size) : Size (size) { }
		GST_SERIALIZABLE (GetDeviceSizeResponse);

		uint64 Size;
	};

	struct GetHostDevicesResponse : CoreServiceResponse
	{
		GetHostDevicesResponse () { }
		GetHostDevicesResponse (const HostDeviceList &hostDevices) : HostDevices (hostDevices) { }
		GST_SERIALIZABLE (GetHostDevicesResponse);

		HostDeviceList HostDevices;
	};

	struct MountVolumeResponse : CoreServiceResponse
	{
		MountVolumeResponse () { }
		MountVolumeResponse (shared_ptr <VolumeInfo> volumeInfo) : MountedVolumeInfo (volumeInfo) { }
		GST_SERIALIZABLE (MountVolumeResponse);

		shared_ptr <VolumeInfo> MountedVolumeInfo;
	};

	struct SetFileOwnerResponse : CoreServiceResponse
	{
		SetFileOwnerResponse () { }
		GST_SERIALIZABLE (SetFileOwnerResponse);
	};
}

#endif // GST_HEADER_Core_Unix_CoreServiceResponse
