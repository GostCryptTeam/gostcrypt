/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#ifndef GST_HEADER_Platform_PlatformTest
#define GST_HEADER_Platform_PlatformTest

#include "PlatformBase.h"
#include "Thread.h"

namespace GostCrypt
{
	class PlatformTest
	{
	public:
		static bool TestAll ();

	protected:
		class RttiTestBase
		{
		public:
			virtual ~RttiTestBase () { };
		};

		class RttiTest : public RttiTestBase {
		public:
			virtual ~RttiTest () { };
		};

		PlatformTest ();
		static void SerializerTest ();
		static void ThreadTest ();
		static GST_THREAD_PROC ThreadTestProc (void *param);

		static bool TestFlag;
	};
}

#endif // GST_HEADER_Platform_PlatformTest
