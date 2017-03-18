/**
 *	Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.
 *	
 *	Governed by the TrueCrypt License 3.0 the full text of which is contained in
 *	the file License.txt included in TrueCrypt binary and source code distribution
 *	packages.
 */

/**
 *
 *	\file EncryptionThreadPool.c
 *	\brief Manage the pool that gather multiple thread that will encrypt/decrypt data.
 *			the threads use small amount data to run the whole encryption/decryption process faster
 *	\version 1.3.1
 *	\date 15/12/2016
 *
 */

#include "EncryptionThreadPool.h"
#include "Pkcs5.h"
#ifdef DEVICE_DRIVER
#include "Driver/Ntdriver.h"
#endif

#define GST_ENC_THREAD_POOL_MAX_THREAD_COUNT 64
#define GST_ENC_THREAD_POOL_QUEUE_SIZE (GST_ENC_THREAD_POOL_MAX_THREAD_COUNT * 2)

#ifdef DEVICE_DRIVER

#define GST_THREAD_HANDLE PKTHREAD
#define GST_THREAD_PROC VOID

#define GST_SET_EVENT(EVENT) KeSetEvent (&EVENT, IO_DISK_INCREMENT, FALSE)
#define GST_CLEAR_EVENT(EVENT) KeClearEvent (&EVENT)

#define GST_MUTEX FAST_MUTEX
#define GST_ACQUIRE_MUTEX(MUTEX) ExAcquireFastMutex (MUTEX)
#define GST_RELEASE_MUTEX(MUTEX) ExReleaseFastMutex (MUTEX)

#else // !DEVICE_DRIVER

#define GST_THREAD_HANDLE HANDLE
#define GST_THREAD_PROC unsigned __stdcall

#define GST_SET_EVENT(EVENT) SetEvent (EVENT)
#define GST_CLEAR_EVENT(EVENT) ResetEvent (EVENT)

#define GST_MUTEX HANDLE
#define GST_ACQUIRE_MUTEX(MUTEX) WaitForSingleObject (*(MUTEX), INFINITE)
#define GST_RELEASE_MUTEX(MUTEX) ReleaseMutex (*(MUTEX))

#endif // !DEVICE_DRIVER


typedef enum
{
	WorkItemFree,
	WorkItemReady,
	WorkItemBusy
} WorkItemState;


typedef struct EncryptionThreadPoolWorkItemStruct
{
	WorkItemState State;
	EncryptionThreadPoolWorkType Type;

	GST_EVENT ItemCompletedEvent;

	struct EncryptionThreadPoolWorkItemStruct *FirstFragment;
	LONG OutstandingFragmentCount;

	union
	{
		struct
		{
			PCRYPTO_INFO CryptoInfo;
			byte *Data;
			UINT64_STRUCT StartUnitNo;
			uint32 UnitCount;

		} Encryption;

		struct
		{
			GST_EVENT *CompletionEvent;
			LONG *CompletionFlag;
			char *DerivedKey;
			int IterationCount;
			GST_EVENT *NoOutstandingWorkItemEvent;
			LONG *OutstandingWorkItemCount;
			char *Password;
			int PasswordLength;
			int Pkcs5Prf;
			char *Salt;

		} KeyDerivation;
	};

} EncryptionThreadPoolWorkItem;


static volatile BOOL ThreadPoolRunning = FALSE;
static volatile BOOL StopPending = FALSE;

static uint32 ThreadCount;
static GST_THREAD_HANDLE ThreadHandles[GST_ENC_THREAD_POOL_MAX_THREAD_COUNT];

static EncryptionThreadPoolWorkItem WorkItemQueue[GST_ENC_THREAD_POOL_QUEUE_SIZE];

static volatile int EnqueuePosition;
static volatile int DequeuePosition;

static GST_MUTEX EnqueueMutex;
static GST_MUTEX DequeueMutex;

static GST_EVENT WorkItemReadyEvent;
static GST_EVENT WorkItemCompletedEvent;


/**
 *
 *	\fn static WorkItemState GetWorkItemState (EncryptionThreadPoolWorkItem *workItem)
 *	\brief Get the state of the work item
 *	\param[in] EncryptionThreadPoolWorkItem * workItem
 *	\return static WorkItemState InterlockedExchangeAdd ((LONG *) &workItem->State, 0)
 *
 */
static WorkItemState GetWorkItemState (EncryptionThreadPoolWorkItem *workItem)
{
	return InterlockedExchangeAdd ((LONG *) &workItem->State, 0);
}


/**
 *
 *	\fn static void SetWorkItemState (EncryptionThreadPoolWorkItem *workItem, WorkItemState newState)
 *	\brief Set the work item State.
 *	\param[in] WorkItemState newState
 *	\param[in] EncryptionThreadPoolWorkItem * workItem
 *	\return static void
 *
 */
static void SetWorkItemState (EncryptionThreadPoolWorkItem *workItem, WorkItemState newState)
{
	InterlockedExchange ((LONG *) &workItem->State, (LONG) newState);
}


/**
 *
 *	\fn static GST_THREAD_PROC EncryptionThreadProc (void *threadArg)
 *	\brief Thread that start the encryption by adding item in the queue, setting the item state to busy.
 *			and starting the good thread to encrypt / decrypt set the encryption key. The pool gather all the thread
 *			that will encrypt data per small packets, the multithreading improve the speed.
 *	\param[in] void * threadArg
 *	\return static GST_THREAD_PROC GST_THROW_FATAL_EXCEPTION switch (workItem->KeyDerivation.Pkcs5Prf) case default 
 *			GST_THROW_FATAL_EXCEPTION switch (workItem->Type) case default
 *			0 #else DEVICE_DRIVER
 *
 */
static GST_THREAD_PROC EncryptionThreadProc (void *threadArg)
{
	EncryptionThreadPoolWorkItem *workItem;

	while (!StopPending)
	{
		GST_ACQUIRE_MUTEX (&DequeueMutex);

		workItem = &WorkItemQueue[DequeuePosition++];

		if (DequeuePosition >= GST_ENC_THREAD_POOL_QUEUE_SIZE)
			DequeuePosition = 0;

		while (!StopPending && GetWorkItemState (workItem) != WorkItemReady)
		{
			GST_WAIT_EVENT (WorkItemReadyEvent);
		}

		SetWorkItemState (workItem, WorkItemBusy);

		GST_RELEASE_MUTEX (&DequeueMutex);

		if (StopPending)
			break;

		switch (workItem->Type)
		{
		case DecryptDataUnitsWork:
			DecryptDataUnitsCurrentThread (workItem->Encryption.Data, &workItem->Encryption.StartUnitNo, workItem->Encryption.UnitCount, workItem->Encryption.CryptoInfo);
			break;

		case EncryptDataUnitsWork:
			EncryptDataUnitsCurrentThread (workItem->Encryption.Data, &workItem->Encryption.StartUnitNo, workItem->Encryption.UnitCount, workItem->Encryption.CryptoInfo);
			break;

		case DeriveKeyWork:
			switch (workItem->KeyDerivation.Pkcs5Prf)
			{
#ifndef GST_WINDOWS_BOOT
			case WHIRLPOOL:
				derive_key_whirlpool (workItem->KeyDerivation.Password, workItem->KeyDerivation.PasswordLength, workItem->KeyDerivation.Salt, PKCS5_SALT_SIZE,
					workItem->KeyDerivation.IterationCount, workItem->KeyDerivation.DerivedKey, GetMaxPkcs5OutSize());
				break;
			case GOSTHASH:
				derive_key_gosthash (workItem->KeyDerivation.Password, workItem->KeyDerivation.PasswordLength, workItem->KeyDerivation.Salt, PKCS5_SALT_SIZE,
					workItem->KeyDerivation.IterationCount, workItem->KeyDerivation.DerivedKey, GetMaxPkcs5OutSize());
				break;
#endif
			case STRIBOG:
				derive_key_stribog (workItem->KeyDerivation.Password, workItem->KeyDerivation.PasswordLength, workItem->KeyDerivation.Salt, PKCS5_SALT_SIZE,
					workItem->KeyDerivation.IterationCount, workItem->KeyDerivation.DerivedKey, GetMaxPkcs5OutSize());
				break;
			default:		
				GST_THROW_FATAL_EXCEPTION;
			} 

			InterlockedExchange (workItem->KeyDerivation.CompletionFlag, TRUE);
			GST_SET_EVENT (*workItem->KeyDerivation.CompletionEvent);
			
			if (InterlockedDecrement (workItem->KeyDerivation.OutstandingWorkItemCount) == 0)
				GST_SET_EVENT (*workItem->KeyDerivation.NoOutstandingWorkItemEvent);

			SetWorkItemState (workItem, WorkItemFree);
			GST_SET_EVENT (WorkItemCompletedEvent);
			continue;

		default:
			GST_THROW_FATAL_EXCEPTION;
		}

		if (workItem != workItem->FirstFragment)
		{
			SetWorkItemState (workItem, WorkItemFree);
			GST_SET_EVENT (WorkItemCompletedEvent);
		}

		if (InterlockedDecrement (&workItem->FirstFragment->OutstandingFragmentCount) == 0)
			GST_SET_EVENT (workItem->FirstFragment->ItemCompletedEvent);
	}

#ifdef DEVICE_DRIVER
	PsTerminateSystemThread (STATUS_SUCCESS);
#else
	_endthreadex (0);
    return 0;
#endif
}


/**
 *
 *	\fn BOOL EncryptionThreadPoolStart (size_t encryptionFreeCpuCount)
 *	\brief Start the thread pool ofr encryption
 *	\param[in] size_t encryptionFreeCpuCount
 *	\return BOOL TRUE if (ThreadPoolRunning)
 *			TRUE if (cpuCount < 2)
 *			FALSE #else DEVICE_DRIVER if (!WorkItemReadyEvent)
 *			FALSE #else DEVICE_DRIVER if (!WorkItemCompletedEvent)
 *			FALSE #else DEVICE_DRIVER if (!DequeueMutex)
 *			FALSE #else DEVICE_DRIVER if (!EnqueueMutex)
 *			FALSE #else DEVICE_DRIVER if (!WorkItemQueue[i].ItemCompletedEvent)
 *			FALSE if (!(ThreadHandles[ThreadCount] = (HANDLE) _beginthreadex (NULL, 0, EncryptionThreadProc, NULL, 0, NULL)))
 *			TRUE otherwise
 *
 */
BOOL EncryptionThreadPoolStart (size_t encryptionFreeCpuCount)
{
	size_t cpuCount, i;

	if (ThreadPoolRunning)
		return TRUE;

#ifdef DEVICE_DRIVER
	cpuCount = GetCpuCount();
#else
	{
		SYSTEM_INFO sysInfo;
		GetSystemInfo (&sysInfo);
		cpuCount = sysInfo.dwNumberOfProcessors;
	}
#endif

	if (cpuCount > encryptionFreeCpuCount)
		cpuCount -= encryptionFreeCpuCount;

	if (cpuCount < 2)
		return TRUE;

	if (cpuCount > GST_ENC_THREAD_POOL_MAX_THREAD_COUNT)
		cpuCount = GST_ENC_THREAD_POOL_MAX_THREAD_COUNT;

	StopPending = FALSE;
	DequeuePosition = 0;
	EnqueuePosition = 0;

#ifdef DEVICE_DRIVER
	KeInitializeEvent (&WorkItemReadyEvent, SynchronizationEvent, FALSE);
	KeInitializeEvent (&WorkItemCompletedEvent, SynchronizationEvent, FALSE);
#else
	WorkItemReadyEvent = CreateEvent (NULL, FALSE, FALSE, NULL);
	if (!WorkItemReadyEvent)
		return FALSE;
	
	WorkItemCompletedEvent = CreateEvent (NULL, FALSE, FALSE, NULL);
	if (!WorkItemCompletedEvent)
		return FALSE;
#endif
	
#ifdef DEVICE_DRIVER
	ExInitializeFastMutex (&DequeueMutex);
	ExInitializeFastMutex (&EnqueueMutex);
#else
	DequeueMutex = CreateMutex (NULL, FALSE, NULL);
	if (!DequeueMutex)
		return FALSE;

	EnqueueMutex = CreateMutex (NULL, FALSE, NULL);
	if (!EnqueueMutex)
		return FALSE;
#endif

	memset (WorkItemQueue, 0, sizeof (WorkItemQueue));

	for (i = 0; i < sizeof (WorkItemQueue) / sizeof (WorkItemQueue[0]); ++i)
	{
		WorkItemQueue[i].State = WorkItemFree;

#ifdef DEVICE_DRIVER
		KeInitializeEvent (&WorkItemQueue[i].ItemCompletedEvent, SynchronizationEvent, FALSE);
#else
		WorkItemQueue[i].ItemCompletedEvent = CreateEvent (NULL, FALSE, FALSE, NULL);
		if (!WorkItemQueue[i].ItemCompletedEvent)
		{
			EncryptionThreadPoolStop();
			return FALSE;
		}
#endif
	}

	for (ThreadCount = 0; ThreadCount < cpuCount; ++ThreadCount)
	{
#ifdef DEVICE_DRIVER
		if (!NT_SUCCESS (GSTStartThread (EncryptionThreadProc, NULL, &ThreadHandles[ThreadCount])))
#else
		if (!(ThreadHandles[ThreadCount] = (HANDLE) _beginthreadex (NULL, 0, EncryptionThreadProc, NULL, 0, NULL)))
#endif
		{
			EncryptionThreadPoolStop();
			return FALSE;
		}
	}

	ThreadPoolRunning = TRUE;
	return TRUE;
}


/**
 *
 *	\fn void EncryptionThreadPoolStop ()
 *	\brief Stop the encryption pool thread
 *	\return void
 *
 */
void EncryptionThreadPoolStop ()
{
	size_t i;

	if (!ThreadPoolRunning)
		return;

	StopPending = TRUE;
	GST_SET_EVENT (WorkItemReadyEvent);

	for (i = 0; i < ThreadCount; ++i)
	{
#ifdef DEVICE_DRIVER
		GSTStopThread (ThreadHandles[i], &WorkItemReadyEvent);
#else
		GST_WAIT_EVENT (ThreadHandles[i]);
#endif
	}

	ThreadCount = 0;

#ifndef DEVICE_DRIVER
	CloseHandle (DequeueMutex);
	CloseHandle (EnqueueMutex);

	CloseHandle (WorkItemReadyEvent);
	CloseHandle (WorkItemCompletedEvent);

	for (i = 0; i < sizeof (WorkItemQueue) / sizeof (WorkItemQueue[0]); ++i)
	{
		if (WorkItemQueue[i].ItemCompletedEvent)
			CloseHandle (WorkItemQueue[i].ItemCompletedEvent);
	}
#endif

	ThreadPoolRunning = FALSE;
}


/**
 *
 *	\fn void EncryptionThreadPoolBeginKeyDerivation (GST_EVENT *completionEvent, GST_EVENT *noOutstandingWorkItemEvent, LONG *completionFlag, LONG *outstandingWorkItemCount, int pkcs5Prf, char *password, int passwordLength, char *salt, int iterationCount, char *derivedKey)
 *	\brief Start the encryption key using the key chosed by the user
 *	\param[in] char * derivedKey
 *	\param[in] int iterationCount
 *	\param[in] char * salt
 *	\param[in] int passwordLength
 *	\param[in] char * password
 *	\param[in] int pkcs5Prf
 *	\param[in] LONG * outstandingWorkItemCount
 *	\param[in] LONG * completionFlag
 *	\param[in] GST_EVENT * noOutstandingWorkItemEvent
 *	\param[in] GST_EVENT * completionEvent
 *	\return void
 *
 */
void EncryptionThreadPoolBeginKeyDerivation (GST_EVENT *completionEvent, GST_EVENT *noOutstandingWorkItemEvent, LONG *completionFlag, LONG *outstandingWorkItemCount, int pkcs5Prf, char *password, int passwordLength, char *salt, int iterationCount, char *derivedKey)
{
	EncryptionThreadPoolWorkItem *workItem;

	if (!ThreadPoolRunning)
		GST_THROW_FATAL_EXCEPTION;

	GST_ACQUIRE_MUTEX (&EnqueueMutex);

	workItem = &WorkItemQueue[EnqueuePosition++];
	if (EnqueuePosition >= GST_ENC_THREAD_POOL_QUEUE_SIZE)
		EnqueuePosition = 0;

	while (GetWorkItemState (workItem) != WorkItemFree)
	{
		GST_WAIT_EVENT (WorkItemCompletedEvent);
	}

	workItem->Type = DeriveKeyWork;
	workItem->KeyDerivation.CompletionEvent = completionEvent;
	workItem->KeyDerivation.CompletionFlag = completionFlag;
	workItem->KeyDerivation.DerivedKey = derivedKey;
	workItem->KeyDerivation.IterationCount = iterationCount;
	workItem->KeyDerivation.NoOutstandingWorkItemEvent = noOutstandingWorkItemEvent;
	workItem->KeyDerivation.OutstandingWorkItemCount = outstandingWorkItemCount;
	workItem->KeyDerivation.Password = password;
	workItem->KeyDerivation.PasswordLength = passwordLength;
	workItem->KeyDerivation.Pkcs5Prf = pkcs5Prf;
	workItem->KeyDerivation.Salt = salt;

	InterlockedIncrement (outstandingWorkItemCount);
	GST_CLEAR_EVENT (*noOutstandingWorkItemEvent);

	SetWorkItemState (workItem, WorkItemReady);
	GST_SET_EVENT (WorkItemReadyEvent);
	GST_RELEASE_MUTEX (&EnqueueMutex);
}


/**
 *
 *	\fn void EncryptionThreadPoolDoWork (EncryptionThreadPoolWorkType type, byte *data, const UINT64_STRUCT *startUnitNo, uint32 unitCount, PCRYPTO_INFO cryptoInfo)
 *	\brief Start the encryption key into the thread pool. Each thread encrypt or decrypt small amount of data
 *	\param[in] PCRYPTO_INFO cryptoInfo
 *	\param[in] uint32 unitCount
 *	\param[in] const UINT64_STRUCT
 *	\param[in] byte * data
 *	\param[in] EncryptionThreadPoolWorkType type
 *	\return void
 *
 */
void EncryptionThreadPoolDoWork (EncryptionThreadPoolWorkType type, byte *data, const UINT64_STRUCT *startUnitNo, uint32 unitCount, PCRYPTO_INFO cryptoInfo)
{
	uint32 fragmentCount;
	uint32 unitsPerFragment;
	uint32 remainder;

	byte *fragmentData;
	uint64 fragmentStartUnitNo;

	EncryptionThreadPoolWorkItem *workItem;
	EncryptionThreadPoolWorkItem *firstFragmentWorkItem;
	
	if (unitCount == 0)
		return;
	
	if (!ThreadPoolRunning || unitCount == 1)
	{
		switch (type)
		{
		case DecryptDataUnitsWork:
			DecryptDataUnitsCurrentThread (data, startUnitNo, unitCount, cryptoInfo);
			break;

		case EncryptDataUnitsWork:
			EncryptDataUnitsCurrentThread (data, startUnitNo, unitCount, cryptoInfo);
			break;

		default:
			GST_THROW_FATAL_EXCEPTION;
		}

		return;
	}

	if (unitCount <= ThreadCount)
	{
		fragmentCount = unitCount;
		unitsPerFragment = 1;
		remainder = 0;
	}
	else
	{
		/* Note that it is not efficient to divide the data into fragments smaller than a few hundred bytes.
		The reason is that the overhead associated with thread handling would in most cases make a multi-threaded 
		process actually slower than a single-threaded process. */

		fragmentCount = ThreadCount;
		unitsPerFragment = unitCount / ThreadCount;
		remainder = unitCount % ThreadCount;

		if (remainder > 0)
			++unitsPerFragment;
	}
	
	fragmentData = data;
	fragmentStartUnitNo = startUnitNo->Value;

	GST_ACQUIRE_MUTEX (&EnqueueMutex);
	firstFragmentWorkItem = &WorkItemQueue[EnqueuePosition];

	while (GetWorkItemState (firstFragmentWorkItem) != WorkItemFree)
	{
		GST_WAIT_EVENT (WorkItemCompletedEvent);
	}

	firstFragmentWorkItem->OutstandingFragmentCount = fragmentCount;

	while (fragmentCount-- > 0)
	{
		workItem = &WorkItemQueue[EnqueuePosition++];
		if (EnqueuePosition >= GST_ENC_THREAD_POOL_QUEUE_SIZE)
			EnqueuePosition = 0;

		while (GetWorkItemState (workItem) != WorkItemFree)
		{
			GST_WAIT_EVENT (WorkItemCompletedEvent);
		}

		workItem->Type = type;
		workItem->FirstFragment = firstFragmentWorkItem;

		workItem->Encryption.CryptoInfo = cryptoInfo;
		workItem->Encryption.Data = fragmentData;
		workItem->Encryption.UnitCount = unitsPerFragment;
		workItem->Encryption.StartUnitNo.Value = fragmentStartUnitNo;

 		fragmentData += unitsPerFragment * ENCRYPTION_DATA_UNIT_SIZE;
		fragmentStartUnitNo += unitsPerFragment;

		if (remainder > 0 && --remainder == 0)
			--unitsPerFragment;

		SetWorkItemState (workItem, WorkItemReady);
		GST_SET_EVENT (WorkItemReadyEvent);
	}

	GST_RELEASE_MUTEX (&EnqueueMutex);

	GST_WAIT_EVENT (firstFragmentWorkItem->ItemCompletedEvent);
	SetWorkItemState (firstFragmentWorkItem, WorkItemFree);
	GST_SET_EVENT (WorkItemCompletedEvent);
}


/**
 *
 *	\fn size_t GetEncryptionThreadCount ()
 *	\brief Getter for the number of thread running
 *	\return ThreadPoolRunning ? ThreadCount : 0
 *
 */
size_t GetEncryptionThreadCount ()
{
	return ThreadPoolRunning ? ThreadCount : 0;
}


/**
 *
 *	\fn size_t GetMaxEncryptionThreadCount ()
 *	\brief Getter for the maximum Thread count define (half useless)
 *	\return GST_ENC_THREAD_POOL_MAX_THREAD_COUNT
 *
 */
size_t GetMaxEncryptionThreadCount ()
{
	return GST_ENC_THREAD_POOL_MAX_THREAD_COUNT;
}


/**
 *
 *	\fn BOOL IsEncryptionThreadPoolRunning ()
 *	\brief Getter to know if the thread pool is running.
 *	\return ThreadPoolRunning
 *
 */
BOOL IsEncryptionThreadPoolRunning ()
{
	return ThreadPoolRunning;
}
