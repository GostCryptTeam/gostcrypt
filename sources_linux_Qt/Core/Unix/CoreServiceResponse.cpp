/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "CoreServiceResponse.h"
#include "Platform/SerializerFactory.h"

namespace GostCrypt
{
	// CheckFilesystemResponse
	void CheckFilesystemResponse::Deserialize (shared_ptr <Stream> stream)
	{
		(void)stream;
	}

	void CheckFilesystemResponse::Serialize (shared_ptr <Stream> stream) const
	{
		Serializable::Serialize (stream);
	}

	// DismountFilesystemResponse
	void DismountFilesystemResponse::Deserialize (shared_ptr <Stream> stream)
	{
		(void)stream;
	}

	void DismountFilesystemResponse::Serialize (shared_ptr <Stream> stream) const
	{
		Serializable::Serialize (stream);
	}

	// DismountVolumeResponse
	void DismountVolumeResponse::Deserialize (shared_ptr <Stream> stream)
	{
		DismountedVolumeInfo = Serializable::DeserializeNew <VolumeInfo> (stream);
	}

	void DismountVolumeResponse::Serialize (shared_ptr <Stream> stream) const
	{
		Serializable::Serialize (stream);
		Serializer sr (stream);
		DismountedVolumeInfo->Serialize (stream);
	}

	// GetDeviceSectorSizeResponse
	void GetDeviceSectorSizeResponse::Deserialize (shared_ptr <Stream> stream)
	{
		Serializer sr (stream);
		sr.Deserialize ("Size", Size);
	}

	void GetDeviceSectorSizeResponse::Serialize (shared_ptr <Stream> stream) const
	{
		Serializable::Serialize (stream);
		Serializer sr (stream);
		sr.Serialize ("Size", Size);
	}

	// GetDeviceSizeResponse
	void GetDeviceSizeResponse::Deserialize (shared_ptr <Stream> stream)
	{
		Serializer sr (stream);
		sr.Deserialize ("Size", Size);
	}

	void GetDeviceSizeResponse::Serialize (shared_ptr <Stream> stream) const
	{
		Serializable::Serialize (stream);
		Serializer sr (stream);
		sr.Serialize ("Size", Size);
	}

	// GetHostDevicesResponse
	void GetHostDevicesResponse::Deserialize (shared_ptr <Stream> stream)
	{
		Serializable::DeserializeList (stream, HostDevices);
	}

	void GetHostDevicesResponse::Serialize (shared_ptr <Stream> stream) const
	{
		Serializable::Serialize (stream);
		Serializable::SerializeList (stream, HostDevices);
	}

	// MountVolumeResponse
	void MountVolumeResponse::Deserialize (shared_ptr <Stream> stream)
	{
		Serializer sr (stream);
		MountedVolumeInfo = Serializable::DeserializeNew <VolumeInfo> (stream);
	}

	void MountVolumeResponse::Serialize (shared_ptr <Stream> stream) const
	{
		Serializable::Serialize (stream);
		Serializer sr (stream);
		MountedVolumeInfo->Serialize (stream);
	}

	// SetFileOwnerResponse
	void SetFileOwnerResponse::Deserialize (shared_ptr <Stream> stream)
	{
		(void)stream;
	}

	void SetFileOwnerResponse::Serialize (shared_ptr <Stream> stream) const
	{
		Serializable::Serialize (stream);
	}

	GST_SERIALIZER_FACTORY_ADD_CLASS (CheckFilesystemResponse);
	GST_SERIALIZER_FACTORY_ADD_CLASS (DismountFilesystemResponse);
	GST_SERIALIZER_FACTORY_ADD_CLASS (DismountVolumeResponse);
	GST_SERIALIZER_FACTORY_ADD_CLASS (GetDeviceSectorSizeResponse);
	GST_SERIALIZER_FACTORY_ADD_CLASS (GetDeviceSizeResponse);
	GST_SERIALIZER_FACTORY_ADD_CLASS (GetHostDevicesResponse);
	GST_SERIALIZER_FACTORY_ADD_CLASS (MountVolumeResponse);
	GST_SERIALIZER_FACTORY_ADD_CLASS (SetFileOwnerResponse);
}
