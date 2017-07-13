/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#ifndef GST_HEADER_Platform_SystemInfo
#define GST_HEADER_Platform_SystemInfo

#include "PlatformBase.h"

namespace GostCrypt
{
	class SystemInfo
	{
	public:
		static wstring GetPlatformName ();
		static vector <int> GetVersion ();
		static bool IsVersionAtLeast (int versionNumber1, int versionNumber2, int versionNumber3 = 0);

	protected:
		SystemInfo ();
	};
}

#endif // GST_HEADER_Platform_SystemInfo
