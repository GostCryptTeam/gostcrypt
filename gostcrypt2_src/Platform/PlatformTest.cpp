/*
 Copyright (c) 2008-2009 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "PlatformTest.h"
#include "FileStream.h"
#include "Finally.h"
#include "MemoryStream.h"
#include "Mutex.h"
#include "SyncEvent.h"
#include "Thread.h"
#include "Common/Gstdefs.h"

namespace GostCrypt
{
    // QSharedPointer, Mutex, ScopeLock, SyncEvent, Thread
	static struct 
	{
		QSharedPointer <int> SharedIntPtr;
		Mutex IntMutex;
		SyncEvent ExitAllowedEvent;
	} ThreadTestData;

	void PlatformTest::ThreadTest ()
	{
		Mutex mutex;
		mutex.Lock();
		mutex.Unlock();

		const int maxThreads = 3;
		ThreadTestData.SharedIntPtr.reset (new int (0));

		for (int i = 0; i < maxThreads; i++)
		{
			Thread t;
			t.Start (&ThreadTestProc, (void *) &ThreadTestData);
		}

		for (int i = 0; i < 50; i++)
		{
			{
				ScopeLock sl (ThreadTestData.IntMutex);
				if (*ThreadTestData.SharedIntPtr == maxThreads)
					break;
			}

			Thread::Sleep(100);
		}

		if (*ThreadTestData.SharedIntPtr != maxThreads)
            throw;// TestFailed (SRC_POS);

		for (int i = 0; i < 60000; i++)
		{
			ThreadTestData.ExitAllowedEvent.Signal();
			Thread::Sleep(1);

			ScopeLock sl (ThreadTestData.IntMutex);
			if (*ThreadTestData.SharedIntPtr == 0)
				break;
		}

		if (*ThreadTestData.SharedIntPtr != 0)
            throw;// TestFailed (SRC_POS);
	}

	GST_THREAD_PROC PlatformTest::ThreadTestProc (void *arg)
	{
		
		if (arg != (void *) &ThreadTestData)
			return 0;

		{
			ScopeLock sl (ThreadTestData.IntMutex);
			++(*ThreadTestData.SharedIntPtr);
		}

		ThreadTestData.ExitAllowedEvent.Wait();

		{
			ScopeLock sl (ThreadTestData.IntMutex);
			--(*ThreadTestData.SharedIntPtr);
		}

		return 0;
	}

	bool PlatformTest::TestAll ()
	{
		// Integer types
        if (sizeof (byte)   != 1 || sizeof (int8)  != 1 || sizeof (__int8)  != 1) throw;// TestFailed (SRC_POS);
        if (sizeof (uint16) != 2 || sizeof (int16) != 2 || sizeof (__int16) != 2) throw;// TestFailed (SRC_POS);
        if (sizeof (uint32) != 4 || sizeof (int32) != 4 || sizeof (__int32) != 4) throw;// TestFailed (SRC_POS);
        if (sizeof (uint64) != 8 || sizeof (int64) != 8) throw;// TestFailed (SRC_POS);

		// Exception handling
		TestFlag = false;
        //try
		{
			try
			{
                throw;// TestFailed (SRC_POS);
			}
			catch (...)
			{
				throw;
			}
			return false;
		}
        /*
        catch (Exception &)
		{
			TestFlag = true;
        }//*/
		if (!TestFlag)
			return false;

		// RTTI
		RttiTest rtti;
		RttiTestBase &rttiBaseRef = rtti;
		RttiTestBase *rttiBasePtr = &rtti;

		if (typeid (rttiBaseRef) != typeid (rtti))
            throw;// TestFailed (SRC_POS);

		if (typeid (*rttiBasePtr) != typeid (rtti))
            throw;// TestFailed (SRC_POS);

		if (dynamic_cast <RttiTest *> (rttiBasePtr) == nullptr)
            throw;// TestFailed (SRC_POS);

		try
		{
			dynamic_cast <RttiTest &> (rttiBaseRef);
		}
		catch (...)
		{
            throw;// TestFailed (SRC_POS);
		}

		// finally
		TestFlag = false;
		{
			finally_do ({ TestFlag = true; });
			if (TestFlag)
                throw; //TestFailed (SRC_POS);
		}
		if (!TestFlag)
            throw;// TestFailed (SRC_POS);

		TestFlag = false;
		{
			finally_do_arg (bool*, &TestFlag, { *finally_arg = true; });
			if (TestFlag)
                throw;// TestFailed (SRC_POS);
		}
		if (!TestFlag)
            throw;// TestFailed (SRC_POS);

		TestFlag = false;
		int tesFlag2 = 0;
		{
			finally_do_arg2 (bool*, &TestFlag, int*, &tesFlag2, { *finally_arg = true; *finally_arg2 = 2; });
			if (TestFlag || tesFlag2 != 0)
                throw;// TestFailed (SRC_POS);
		}
		if (!TestFlag || tesFlag2 != 2)
            throw;// TestFailed (SRC_POS);

		SerializerTest();
		ThreadTest();

		return true;
	}

	bool PlatformTest::TestFlag;
}
