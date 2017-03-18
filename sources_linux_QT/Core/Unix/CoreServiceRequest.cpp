/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include <errno.h>
#include "CoreServiceRequest.h"
#include "Platform/SerializerFactory.h"

namespace GostCrypt
{
	void CoreServiceRequest::Deserialize (shared_ptr <Stream> stream)
	{
		Serializer sr (stream);
		sr.Deserialize ("AdminPassword", AdminPassword);
		ApplicationExecutablePath = sr.DeserializeWString ("ApplicationExecutablePath");
		sr.Deserialize ("ElevateUserPrivileges", ElevateUserPrivileges);
		sr.Deserialize ("FastElevation", FastElevation);
	}

	void CoreServiceRequest::Serialize (shared_ptr <Stream> stream) const
	{
		Serializable::Serialize (stream);
		Serializer sr (stream);
		sr.Serialize ("AdminPassword", AdminPassword);
		sr.Serialize ("ApplicationExecutablePath", wstring (ApplicationExecutablePath));
		sr.Serialize ("ElevateUserPrivileges", ElevateUserPrivileges);
		sr.Serialize ("FastElevation", FastElevation);
	}

	// CheckFilesystemRequest
	void CheckFilesystemRequest::Deserialize (shared_ptr <Stream> stream)
	{
		CoreServiceRequest::Deserialize (stream);
		Serializer sr (stream);
		MountedVolumeInfo = Serializable::DeserializeNew <VolumeInfo> (stream);
		sr.Deserialize ("Repair", Repair);
	}

	bool CheckFilesystemRequest::RequiresElevation () const
	{
#ifdef GST_MACOSX
		return false;
#endif
		return !Core->HasAdminPrivileges();
	}

	void CheckFilesystemRequest::Serialize (shared_ptr <Stream> stream) const
	{
		CoreServiceRequest::Serialize (stream);
		Serializer sr (stream);
		MountedVolumeInfo->Serialize (stream);
		sr.Serialize ("Repair", Repair);
	}

	// DismountFilesystemRequest
	void DismountFilesystemRequest::Deserialize (shared_ptr <Stream> stream)
	{
		CoreServiceRequest::Deserialize (stream);
		Serializer sr (stream);
		sr.Deserialize ("Force", Force);
		MountPoint = sr.DeserializeWString ("MountPoint");
	}

	bool DismountFilesystemRequest::RequiresElevation () const
	{
		return !Core->HasAdminPrivileges();
	}

	void DismountFilesystemRequest::Serialize (shared_ptr <Stream> stream) const
	{
		CoreServiceRequest::Serialize (stream);
		Serializer sr (stream);
		sr.Serialize ("Force", Force);
		sr.Serialize ("MountPoint", wstring (MountPoint));
	}

	// DismountVolumeRequest
	void DismountVolumeRequest::Deserialize (shared_ptr <Stream> stream)
	{
		CoreServiceRequest::Deserialize (stream);
		Serializer sr (stream);
		sr.Deserialize ("IgnoreOpenFiles", IgnoreOpenFiles);
		sr.Deserialize ("SyncVolumeInfo", SyncVolumeInfo);
		MountedVolumeInfo = Serializable::DeserializeNew <VolumeInfo> (stream);
	}

	bool DismountVolumeRequest::RequiresElevation () const
	{
#ifdef GST_MACOSX
		if (MountedVolumeInfo->Path.IsDevice())
		{
			try
			{
				File file;
				file.Open (MountedVolumeInfo->Path, File::OpenReadWrite);
			}
			catch (...)
			{
				return true;
			}
		}

		return false;
#endif
		return !Core->HasAdminPrivileges();
	}

	void DismountVolumeRequest::Serialize (shared_ptr <Stream> stream) const
	{
		CoreServiceRequest::Serialize (stream);
		Serializer sr (stream);
		sr.Serialize ("IgnoreOpenFiles", IgnoreOpenFiles);
		sr.Serialize ("SyncVolumeInfo", SyncVolumeInfo);
		MountedVolumeInfo->Serialize (stream);
	}

	// GetDeviceSectorSizeRequest
	void GetDeviceSectorSizeRequest::Deserialize (shared_ptr <Stream> stream)
	{
		CoreServiceRequest::Deserialize (stream);
		Serializer sr (stream);
		Path = sr.DeserializeWString ("Path");
	}

	bool GetDeviceSectorSizeRequest::RequiresElevation () const
	{
		return !Core->HasAdminPrivileges();
	}

	void GetDeviceSectorSizeRequest::Serialize (shared_ptr <Stream> stream) const
	{
		CoreServiceRequest::Serialize (stream);
		Serializer sr (stream);
		sr.Serialize ("Path", wstring (Path));
	}

	// GetDeviceSizeRequest
	void GetDeviceSizeRequest::Deserialize (shared_ptr <Stream> stream)
	{
		CoreServiceRequest::Deserialize (stream);
		Serializer sr (stream);
		Path = sr.DeserializeWString ("Path");
	}

	bool GetDeviceSizeRequest::RequiresElevation () const
	{
		return !Core->HasAdminPrivileges();
	}

	void GetDeviceSizeRequest::Serialize (shared_ptr <Stream> stream) const
	{
		CoreServiceRequest::Serialize (stream);
		Serializer sr (stream);
		sr.Serialize ("Path", wstring (Path));
	}

	// GetHostDevicesRequest
	void GetHostDevicesRequest::Deserialize (shared_ptr <Stream> stream)
	{
		CoreServiceRequest::Deserialize (stream);
		Serializer sr (stream);
		sr.Deserialize ("PathListOnly", PathListOnly);
	}

	bool GetHostDevicesRequest::RequiresElevation () const
	{
		return !Core->HasAdminPrivileges();
	}

	void GetHostDevicesRequest::Serialize (shared_ptr <Stream> stream) const
	{
		CoreServiceRequest::Serialize (stream);
		Serializer sr (stream);
		sr.Serialize ("PathListOnly", PathListOnly);
	}

	// ExitRequest
	void ExitRequest::Deserialize (shared_ptr <Stream> stream)
	{
		CoreServiceRequest::Deserialize (stream);
	}

	void ExitRequest::Serialize (shared_ptr <Stream> stream) const
	{
		CoreServiceRequest::Serialize (stream);
	}

	// MountVolumeRequest
	void MountVolumeRequest::Deserialize (shared_ptr <Stream> stream)
	{
		CoreServiceRequest::Deserialize (stream);
		Serializer sr (stream);
		DeserializedOptions = Serializable::DeserializeNew <MountOptions> (stream);
		Options = DeserializedOptions.get();
	}

	bool MountVolumeRequest::RequiresElevation () const
	{
#ifdef GST_MACOSX
		if (Options->Path->IsDevice())
		{
			try
			{
				File file;
				file.Open (*Options->Path, File::OpenReadWrite);
			}
			catch (...)
			{
				return true;
			}
		}

		return false;
#endif
		return !Core->HasAdminPrivileges();
	}

	void MountVolumeRequest::Serialize (shared_ptr <Stream> stream) const
	{
		CoreServiceRequest::Serialize (stream);
		Serializer sr (stream);
		Options->Serialize (stream);
	}
	
	// SetFileOwnerRequest
	void SetFileOwnerRequest::Deserialize (shared_ptr <Stream> stream)
	{
		CoreServiceRequest::Deserialize (stream);
		Serializer sr (stream);
		
		uint64 owner;
		sr.Deserialize ("Owner", owner);
		Owner.SystemId = static_cast <uid_t> (owner);

		Path = sr.DeserializeWString ("Path");
	}

	bool SetFileOwnerRequest::RequiresElevation () const
	{
		return !Core->HasAdminPrivileges();
	}

	void SetFileOwnerRequest::Serialize (shared_ptr <Stream> stream) const
	{
		CoreServiceRequest::Serialize (stream);
		Serializer sr (stream);

		uint64 owner = Owner.SystemId;
		sr.Serialize ("Owner", owner);

		sr.Serialize ("Path", wstring (Path));
	}


	GST_SERIALIZER_FACTORY_ADD_CLASS (CoreServiceRequest);
	GST_SERIALIZER_FACTORY_ADD_CLASS (CheckFilesystemRequest);
	GST_SERIALIZER_FACTORY_ADD_CLASS (DismountFilesystemRequest);
	GST_SERIALIZER_FACTORY_ADD_CLASS (DismountVolumeRequest);
	GST_SERIALIZER_FACTORY_ADD_CLASS (ExitRequest);
	GST_SERIALIZER_FACTORY_ADD_CLASS (GetDeviceSectorSizeRequest);
	GST_SERIALIZER_FACTORY_ADD_CLASS (GetDeviceSizeRequest);
	GST_SERIALIZER_FACTORY_ADD_CLASS (GetHostDevicesRequest);
	GST_SERIALIZER_FACTORY_ADD_CLASS (MountVolumeRequest);
	GST_SERIALIZER_FACTORY_ADD_CLASS (SetFileOwnerRequest);
}
