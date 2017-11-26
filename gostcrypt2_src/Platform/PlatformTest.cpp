/*
 Copyright (c) 2008-2009 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "PlatformTest.h"
#include "Common/Gstdefs.h"
#include <QSharedPointer>
#include <QMutex>
#include <QWaitCondition>

namespace GostCrypt
{


	void PlatformTest::ThreadTest ()
	{
        QMutex mutex;
        mutex.lock();
        mutex.unlock();

		const int maxThreads = 3;
        ThreadTestData->SharedIntPtr.reset (new int (0));

        QList<QSharedPointer<TestThread>> threads;
        for (int i = 0; i < maxThreads; i++)
		{
            threads.append(QSharedPointer<TestThread>(new TestThread(ThreadTestData)));
            QObject::connect(threads.last().data(), SIGNAL(finished()), threads.last().data(), SLOT(deleteLater()));
            threads.last()->start();
		}

		for (int i = 0; i < 50; i++)
		{
			{
                QMutexLocker lock(&ThreadTestData->IntMutex);
                if (*ThreadTestData->SharedIntPtr == maxThreads)
					break;
			}

            QThread::sleep(100);
		}

        if (*ThreadTestData->SharedIntPtr != maxThreads)
            throw;// TestFailed (SRC_POS);

		for (int i = 0; i < 60000; i++)
		{
            ThreadTestData->ExitAllowedEvent.wakeOne();
            QThread::sleep(1);

            QMutexLocker lock (&ThreadTestData->IntMutex);
            if (*ThreadTestData->SharedIntPtr == 0)
				break;
		}

        if (*ThreadTestData->SharedIntPtr != 0)
            throw;// TestFailed (SRC_POS);
	}



	bool PlatformTest::TestAll ()
	{
		// Integer types
        if (sizeof (quint8)   != 1 || sizeof (int8)  != 1 || sizeof (__int8)  != 1) throw;// TestFailed (SRC_POS);
        if (sizeof (quint16) != 2 || sizeof (int16) != 2 || sizeof (__int16) != 2) throw;// TestFailed (SRC_POS);
        if (sizeof (quint32) != 4 || sizeof (int32) != 4 || sizeof (__int32) != 4) throw;// TestFailed (SRC_POS);
        if (sizeof (quint64) != 8 || sizeof (int64) != 8) throw;// TestFailed (SRC_POS);

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

		ThreadTest();

		return true;
	}

    bool PlatformTest::TestFlag;

    TestThread::TestThread(QSharedPointer<PlatformTest::ThreadTestDataStruct> threadTestData)
    {
        QMutexLocker lock(&mThreadTestDataMutex);
        mThreadTestData = threadTestData;
    }

    void TestThread::run()
    {
        mThreadTestDataMutex.lock();
        if (mThreadTestData.data() != PlatformTest::ThreadTestData.data()) {
            mThreadTestDataMutex.unlock();
            return;
        }
        mThreadTestDataMutex.unlock();


        PlatformTest::ThreadTestData->IntMutex.lock();
        ++(*PlatformTest::ThreadTestData->SharedIntPtr);

        PlatformTest::ThreadTestData->ExitAllowedEvent.wait(&PlatformTest::ThreadTestData->IntMutex);

        --(*PlatformTest::ThreadTestData->SharedIntPtr);

        PlatformTest::ThreadTestData->IntMutex.unlock();

    }

}
