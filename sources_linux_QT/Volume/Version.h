/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Volume_Version
#define GST_HEADER_Volume_Version

#include "Platform/PlatformBase.h"
#include "Common/Gstdefs.h"

namespace GostCrypt
{
	class Version
	{
	public:
		static const string String ()					{ return VERSION_STRING; }
		static const uint16 Number ()					{ return VERSION_NUM; }

	};
}

#endif // GST_HEADER_Volume_Version
