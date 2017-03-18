/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Core_Unix_MountedFilesystem
#define GST_HEADER_Core_Unix_MountedFilesystem

#include "Platform/Platform.h"

namespace GostCrypt
{
	struct MountedFilesystem
	{
	public:
		DevicePath Device;
		DirectoryPath MountPoint;
		string Type;
	};

	typedef list < shared_ptr <MountedFilesystem> > MountedFilesystemList;
}

#endif // GST_HEADER_Core_Unix_MountedFilesystem
