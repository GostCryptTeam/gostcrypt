/*
 Copyright (c) 2008-2009 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/



#include <unistd.h>
#include "Core/GostCryptException.h"
#include "Crypto/Crypto.h"
#include "EncryptionThreadPool.h"

namespace GostCrypt
{
namespace Volume
{

void EncryptionThreadPool::DoWork(WorkType::Enum type, const EncryptionMode* encryptionMode,
                                  quint8* data, quint64 startUnitNo, quint64 unitCount, size_t sectorSize)
{
    size_t fragmentCount;
    size_t unitsPerFragment;
    size_t remainder;

    quint8* fragmentData;
    quint64 fragmentStartUnitNo;

    WorkItem* firstFragmentWorkItem;

    if (unitCount == 0)
    {
        return;
    }

    if (!ThreadPoolRunning || unitCount == 1)
    {
        switch (type)
        {
            case WorkType::DecryptDataUnits:
                encryptionMode->DecryptSectorsCurrentThread(data, startUnitNo, unitCount, sectorSize);
                break;

            case WorkType::EncryptDataUnits:
                encryptionMode->EncryptSectorsCurrentThread(data, startUnitNo, unitCount, sectorSize);
                break;

            default:
                throw InvalidParameterException("WorkType", "Unknown WorkType");
        }

        return;
    }

    if (unitCount <= ThreadCount)
    {
        fragmentCount = (size_t) unitCount;
        unitsPerFragment = 1;
        remainder = 0;
    }
    else
    {
        fragmentCount = ThreadCount;
        unitsPerFragment = (size_t) unitCount / ThreadCount;
        remainder = (size_t) unitCount % ThreadCount;

        if (remainder > 0)
        {
            ++unitsPerFragment;
        }
    }

    fragmentData = data;
    fragmentStartUnitNo = startUnitNo;

    {
        QMutexLocker lock(&EnqueueMutex);
        firstFragmentWorkItem = &WorkItemQueue[EnqueuePosition];

        while (firstFragmentWorkItem->State != WorkItem::State::Free)
        {
            WorkItemCompletedEvent.wait(&WorkItemCompletedEventMutex);
        }

        firstFragmentWorkItem->OutstandingFragmentCount = fragmentCount;
        firstFragmentWorkItem->ItemException.reset();

        while (fragmentCount-- > 0)
        {
            WorkItem* workItem = &WorkItemQueue[EnqueuePosition++];

            if (EnqueuePosition >= QueueSize)
            {
                EnqueuePosition = 0;
            }

            while (workItem->State != WorkItem::State::Free)
            {
                WorkItemCompletedEvent.wait(&WorkItemCompletedEventMutex);
            }

            workItem->Type = type;
            workItem->FirstFragment = firstFragmentWorkItem;

            workItem->Encryption.Mode = encryptionMode;
            workItem->Encryption.Data = fragmentData;
            workItem->Encryption.UnitCount = unitsPerFragment;
            workItem->Encryption.StartUnitNo = fragmentStartUnitNo;
            workItem->Encryption.SectorSize = sectorSize;

            fragmentData += unitsPerFragment * ENCRYPTION_DATA_UNIT_SIZE;
            fragmentStartUnitNo += unitsPerFragment;

            if (remainder > 0 && --remainder == 0)
            {
                --unitsPerFragment;
            }

            workItem->State = WorkItem::State::Ready;

            EncryptionThreadPool::WorkItemReadyEventMutex.unlock();
            WorkItemReadyEvent.wakeOne();
        }
        firstFragmentWorkItem->ItemCompletedEventMutex.lock();
    }

    firstFragmentWorkItem->ItemCompletedEvent.wait(&firstFragmentWorkItem->ItemCompletedEventMutex);

    QSharedPointer <GostCryptException> itemException;
    if (!firstFragmentWorkItem->ItemException.isNull())
    {
        itemException = firstFragmentWorkItem->ItemException;
    }

    firstFragmentWorkItem->State = WorkItem::State::Free;
    firstFragmentWorkItem->ItemCompletedEventMutex.unlock();

    WorkItemCompletedEvent.wakeOne();

    if (!itemException.isNull())
    {
        itemException->raise();
    }
}

void EncryptionThreadPool::Start()
{
    if (ThreadPoolRunning)
    {
        return;
    }

    int cpuCount;

    cpuCount = QThread::idealThreadCount();
    if (cpuCount == -1)
    {
        cpuCount = 1;
    }


    if (cpuCount < 2)
    {
        return;
    }

    if (cpuCount > MaxThreadCount)
    {
        cpuCount = MaxThreadCount;
    }

    StopPending = false;
    DequeuePosition = 0;
    EnqueuePosition = 0;

    for (size_t i = 0; i < sizeof(WorkItemQueue) / sizeof(WorkItemQueue[0]); ++i)
    {
        WorkItemQueue[i].State = WorkItem::State::Free;
    }

    try
    {
        for (ThreadCount = 0; ThreadCount < cpuCount; ++ThreadCount)
        {
            QSharedPointer<EncryptionThread> thread(new EncryptionThread());
            thread->start();
            RunningThreads.append(thread);
        }
    }
    catch (...)
    {
        try
        {
            ThreadPoolRunning = true;
            Stop();
        }
        catch (...) { }

        throw; //rethrow
    }
    WorkItemCompletedEventMutex.lock();
    WorkItemReadyEventMutex.lock();

    ThreadPoolRunning = true;
}

void EncryptionThreadPool::Stop()
{
    if (!ThreadPoolRunning)
    {
        return;
    }

    StopPending = true;
    WorkItemReadyEvent.wakeAll();

    for (const QSharedPointer<EncryptionThread> thread : RunningThreads)
    {
        thread->wait();
    }
    WorkItemCompletedEventMutex.unlock();
    WorkItemReadyEventMutex.unlock();
    ThreadCount = 0;
    ThreadPoolRunning = false;
}



void EncryptionThread::run()
{
    try
    {
        while (!EncryptionThreadPool::StopPending)
        {
            EncryptionThreadPool::WorkItem* workItem;
            {
                QMutexLocker lock(&EncryptionThreadPool::DequeueMutex);

                workItem = &EncryptionThreadPool::WorkItemQueue[EncryptionThreadPool::DequeuePosition++];

                if (EncryptionThreadPool::DequeuePosition >= EncryptionThreadPool::QueueSize)
                {
                    EncryptionThreadPool::DequeuePosition = 0;
                }

                // Waiting for current workItem to be Ready (there is work to do)
                while (!EncryptionThreadPool::StopPending &&
                        workItem->State != EncryptionThreadPool::WorkItem::State::Ready)
                {
                    EncryptionThreadPool::WorkItemReadyEvent.wait(
                        &EncryptionThreadPool::WorkItemReadyEventMutex);//, 100);
                }

                workItem->State = EncryptionThreadPool::WorkItem::State::Busy;
            }

            if (EncryptionThreadPool::StopPending)
            {
                break;
            }

            try
            {
                switch (workItem->Type)
                {
                    case EncryptionThreadPool::WorkType::DecryptDataUnits:
                        workItem->Encryption.Mode->DecryptSectorsCurrentThread(workItem->Encryption.Data,
                                workItem->Encryption.StartUnitNo, workItem->Encryption.UnitCount, workItem->Encryption.SectorSize);
                        break;

                    case EncryptionThreadPool::WorkType::EncryptDataUnits:
                        workItem->Encryption.Mode->EncryptSectorsCurrentThread(workItem->Encryption.Data,
                                workItem->Encryption.StartUnitNo, workItem->Encryption.UnitCount, workItem->Encryption.SectorSize);
                        break;

                    default:
                        throw InvalidParameterException("WorkType", "Unknown WorkType");
                }
            }
            catch (GostCryptException& e)
            {
                workItem->FirstFragment->ItemException.reset(e.clone());
            }
            catch (std::exception& e)
            {
                workItem->FirstFragment->ItemException.reset(new ExternalExceptionException(e));
            }
            catch (...)
            {
                workItem->FirstFragment->ItemException.reset(new UnknowExceptionException());
            }

            if (workItem != workItem->FirstFragment)
            {
                workItem->State = EncryptionThreadPool::WorkItem::State::Free;
                EncryptionThreadPool::WorkItemCompletedEvent.wakeOne();
            }

            if (!workItem->FirstFragment->OutstandingFragmentCount.deref())
            {
                workItem->FirstFragment->ItemCompletedEvent.wakeOne();
            }
        }
    }
    catch (std::exception& e)
    {
        // SystemLog::WriteException (e); TODO
    }
    catch (...)
    {
        //SystemLog::WriteException (UnknownException (SRC_POS));
    }
}

volatile bool EncryptionThreadPool::ThreadPoolRunning = false;
volatile bool EncryptionThreadPool::StopPending = false;

quint8 EncryptionThreadPool::ThreadCount;

EncryptionThreadPool::WorkItem EncryptionThreadPool::WorkItemQueue[QueueSize];

volatile size_t EncryptionThreadPool::EnqueuePosition;
volatile size_t EncryptionThreadPool::DequeuePosition;

QMutex EncryptionThreadPool::EnqueueMutex;
QMutex EncryptionThreadPool::DequeueMutex;

QWaitCondition EncryptionThreadPool::WorkItemReadyEvent;
QWaitCondition EncryptionThreadPool::WorkItemCompletedEvent;
QMutex EncryptionThreadPool::WorkItemCompletedEventMutex;
QMutex EncryptionThreadPool::WorkItemReadyEventMutex;



QList < QSharedPointer <EncryptionThread> > EncryptionThreadPool::RunningThreads;

}
}
