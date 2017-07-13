/*
 Copyright (c) 2008-2009 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#ifndef GST_HEADER_Platform_Directory
#define GST_HEADER_Platform_Directory

#include "PlatformBase.h"
#include "FilesystemPath.h"

namespace GostCrypt
{
	class Directory
	{
	public:
		static void Create (const DirectoryPath &path);
		static DirectoryPath AppendSeparator (const DirectoryPath &path);
		static FilePathList GetFilePaths (const DirectoryPath &path = L".", bool regularFilesOnly = true);

	private:
		Directory ();
	};
}

#endif // GST_HEADER_Platform_Directory
