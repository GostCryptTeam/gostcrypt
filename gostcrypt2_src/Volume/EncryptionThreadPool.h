/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Volume_EncryptionThreadPool
#define GST_HEADER_Volume_EncryptionThreadPool

#include "EncryptionMode.h"
#include "Core/GostCryptException.h"
#include <QMutex>
#include <QThread>
#include <QWaitCondition>

namespace GostCrypt
{
namespace Volume {

    class EncryptionThread;

    class EncryptionThreadPool
	{
    friend class EncryptionThread;
    public:
		struct WorkType
		{
			enum Enum
			{
				EncryptDataUnits,
				DecryptDataUnits,
				DeriveKey
			};
		};

		struct WorkItem
		{
			struct State
			{
				enum Enum
				{
					Free,
					Ready,
					Busy
				};
			};

			struct WorkItem *FirstFragment;
            QSharedPointer <GostCryptException> ItemException;
            QWaitCondition ItemCompletedEvent;
            QMutex ItemCompletedEventMutex;
            QAtomicInteger<size_t> OutstandingFragmentCount;
            QAtomicInteger<quint8> State;
			WorkType::Enum Type;

			union
			{
				struct
				{
					const EncryptionMode *Mode;
					quint8 *Data;
					quint64 StartUnitNo;
					quint64 UnitCount;
					size_t SectorSize;
				} Encryption;
			};
		};

		static void DoWork (WorkType::Enum type, const EncryptionMode *mode, quint8 *data, quint64 startUnitNo, quint64 unitCount, size_t sectorSize);
		static bool IsRunning () { return ThreadPoolRunning; }
		static void Start ();
		static void Stop ();

	protected:
        static const int MaxThreadCount = 32;
        static const int QueueSize = MaxThreadCount * 2;

        static QMutex DequeueMutex;
		static volatile size_t DequeuePosition;
		static volatile size_t EnqueuePosition;
        static QMutex EnqueueMutex;
        static QList < QSharedPointer <EncryptionThread> > RunningThreads;
		static volatile bool StopPending;
        static quint8 ThreadCount;
		static volatile bool ThreadPoolRunning;
        static QWaitCondition WorkItemCompletedEvent;
        static QMutex WorkItemCompletedEventMutex;
		static WorkItem WorkItemQueue[QueueSize];
        static QWaitCondition WorkItemReadyEvent;
        static QMutex WorkItemReadyEventMutex;
    };

    class EncryptionThread : public QThread {
        Q_OBJECT
        void run();
    };
}
}

#endif // GST_HEADER_Volume_EncryptionThreadPool
