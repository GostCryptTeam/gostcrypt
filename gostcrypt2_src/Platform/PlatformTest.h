/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Platform_PlatformTest
#define GST_HEADER_Platform_PlatformTest

#include <QSharedPointer>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>

namespace GostCrypt
{
    class TestThread;
	class PlatformTest
	{
        friend class TestThread;
	public:
		static bool TestAll ();

	protected:
		class RttiTestBase
		{
		public:
            virtual ~RttiTestBase () { }
		};

		class RttiTest : public RttiTestBase {
		public:
            virtual ~RttiTest () { }
		};

        struct ThreadTestDataStruct
        {
            QSharedPointer <int> SharedIntPtr;
            QMutex IntMutex;
            QWaitCondition ExitAllowedEvent;
        };

        static QSharedPointer<ThreadTestDataStruct> ThreadTestData;




		PlatformTest ();
		static void SerializerTest ();
		static void ThreadTest ();

		static bool TestFlag;
	};

    class TestThread : public QThread {
        Q_OBJECT

        QSharedPointer<PlatformTest::ThreadTestDataStruct> mThreadTestData;
        QMutex mThreadTestDataMutex;
    public:
        TestThread(QSharedPointer<PlatformTest::ThreadTestDataStruct> threadTestData);
        void run();
    };
}

#endif // GST_HEADER_Platform_PlatformTest
