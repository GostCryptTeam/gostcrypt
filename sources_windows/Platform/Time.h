/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#ifndef GST_HEADER_Platform_Time
#define GST_HEADER_Platform_Time

#include "PlatformBase.h"

namespace GostCrypt
{
	class Time
	{
	public:
		Time () { }
		virtual ~Time () { }

		static uint64 GetCurrent (); // Returns time in hundreds of nanoseconds since 1601/01/01

	private:
		Time (const Time &);
		Time &operator= (const Time &);
	};
}

#endif // GST_HEADER_Platform_Time
