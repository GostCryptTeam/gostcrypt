/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Volume_EncryptionThreadPool
#define GST_HEADER_Volume_EncryptionThreadPool

#include "Platform/Platform.h"
#include "EncryptionMode.h"
#include "Platform/SharedVal.h"
#include <QMutex>

namespace GostCrypt
{
namespace Volume {

	class EncryptionThreadPool
	{
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
            //QSharedPointer <Exception> ItemException;
			SyncEvent ItemCompletedEvent;
			SharedVal <size_t> OutstandingFragmentCount;
			SharedVal <State::Enum> State;
			WorkType::Enum Type;

			union
			{
				struct
				{
					const EncryptionMode *Mode;
					byte *Data;
					uint64 StartUnitNo;
					uint64 UnitCount;
					size_t SectorSize;
				} Encryption;
			};
		};

		static void DoWork (WorkType::Enum type, const EncryptionMode *mode, byte *data, uint64 startUnitNo, uint64 unitCount, size_t sectorSize);
		static bool IsRunning () { return ThreadPoolRunning; }
		static void Start ();
		static void Stop ();

	protected:
		static void WorkThreadProc ();

		static const size_t MaxThreadCount = 32;
		static const size_t QueueSize = MaxThreadCount * 2;

        static QMutex DequeueMutex;
		static volatile size_t DequeuePosition;
		static volatile size_t EnqueuePosition;
        static QMutex EnqueueMutex;
		static list < QSharedPointer <Thread> > RunningThreads;
		static volatile bool StopPending;
		static size_t ThreadCount;
		static volatile bool ThreadPoolRunning;
		static SyncEvent WorkItemCompletedEvent;
		static WorkItem WorkItemQueue[QueueSize];
		static SyncEvent WorkItemReadyEvent;
	};
}
}

#endif // GST_HEADER_Volume_EncryptionThreadPool
