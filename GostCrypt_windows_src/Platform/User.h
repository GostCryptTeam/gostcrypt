/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#ifndef GST_HEADER_Platform_User
#define GST_HEADER_Platform_User

#include "PlatformBase.h"

#ifdef GST_UNIX
#include <unistd.h>
#include <sys/types.h>
#endif

namespace GostCrypt
{
	struct UserId
	{
		UserId () { }
#ifdef GST_UNIX
		UserId (uid_t systemId) : SystemId (systemId) { }

		uid_t SystemId;
#endif
	};
}

#endif // GST_HEADER_Platform_User
