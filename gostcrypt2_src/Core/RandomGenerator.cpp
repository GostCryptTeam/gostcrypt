/*
 Copyright (c) 2008-2009 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef HAVE_GETRANDOM
#include <sys/random.h>
#endif

#include "CoreException.h"
#include "RandomGenerator.h"
#include "Volume/Crc32.h"
#include "Volume/VolumeHashStribog.h"

namespace GostCrypt
{
namespace Core
{

void RandomGenerator::AddSystemDataToPool(bool fast)
{
    SecureBuffer buffer(PoolSize);

#ifdef HAVE_GETRANDOM
    int getrandomRet;

    // Fill in the buffer with fast random data
    getrandomRet = getrandom(buffer, buffer.Size(), 0);
    if (getrandomRet == -1 || (size_t) getrandomRet < buffer.Size())
    {
        throw FailedUsingSystemRandomSourceException(getrandomRet);
    }
    AddToPool(buffer);

    // If not in fast mode, add non-fast random data too if available
    if (!fast)
    {
        getrandomRet = getrandom(buffer, buffer.Size(), GRND_RANDOM | GRND_NONBLOCK);
        if (getrandomRet == -1 && errno != EAGAIN)
        {
            throw FailedUsingSystemRandomSourceException(getrandomRet);
        }
        AddToPool(buffer);
    }

#else
    int randsource;
    int readRet;

    // Fill in the buffer with fast random data
    randsource = open("/dev/urandom", O_RDONLY);
    if (randsource == -1)
    {
        throw FailedOpenFileException(QFileInfo(QStringLiteral("/dev/urandom")));
    }

    readRet = read(randsource, buffer, buffer.size());
    close(randsource);
    if (readRet == -1 || (size_t)readRet < buffer.size())
    {
        throw FailedUsingSystemRandomSourceException(readRet);
    }
    AddToPool(buffer);

    //If not in fast mode, add non-fast random data too if available
    if (!fast)
    {
        randsource = open("/dev/random", O_RDONLY | O_NONBLOCK);
        if (randsource == -1)
        {
            throw FailedOpenFileException(QFileInfo(QStringLiteral("/dev/random")));
        }
        readRet = read(randsource, buffer, buffer.size());
        close(randsource);
        if (readRet == -1 && errno != EAGAIN)
        {
            throw FailedUsingSystemRandomSourceException(readRet);
        }
        AddToPool(buffer);
    }

#endif
}

void RandomGenerator::AddToPool(const BufferPtr& buffer)
{
    if (!Running)
    {
        throw RandomNumberGeneratorNotRunningException();
    }

    QMutexLocker lock(&AccessMutex);

    for (size_t i = 0; i < buffer.size(); ++i)
    {
        Pool[WriteOffset++] += buffer[i];

        if (WriteOffset >= PoolSize)
        {
            WriteOffset = 0;
        }

        if (++BytesAddedSincePoolHashMix >= MaxBytesAddedBeforePoolHashMix)
        {
            HashMixPool();
        }
    }
}

void RandomGenerator::SetHash(QSharedPointer<Volume::VolumeHash> hashfct)
{
    QMutexLocker lock(&RandomGenerator::AccessMutex);
    RandomGenerator::PoolHash = hashfct;
}

void RandomGenerator::GetData(BufferPtr& buffer, bool fast)
{
    if (!Running)
    {
        throw RandomNumberGeneratorNotRunningException();
    }

    if (buffer.size() > PoolSize)
    {
        throw InvalidParameterException("buffer.size()", "buffer.size() > PoolSize");
    }

    QMutexLocker lock(&AccessMutex);

    // Poll system for data
    AddSystemDataToPool(fast);
    HashMixPool();

    // Transfer bytes from pool to output buffer
    for (size_t i = 0; i < buffer.size(); ++i)
    {
        buffer[i] += Pool[ReadOffset++];

        if (ReadOffset >= PoolSize)
        {
            ReadOffset = 0;
        }
    }

    // Invert and mix the pool
    for (size_t i = 0; i < Pool.size(); ++i)
    {
        Pool[i] = ~Pool[i];
    }

    AddSystemDataToPool(true);
    HashMixPool();

    // XOR the current pool content into the output buffer to prevent pool state leaks
    for (size_t i = 0; i < buffer.size(); ++i)
    {
        buffer[i] ^= Pool[ReadOffset++];

        if (ReadOffset >= PoolSize)
        {
            ReadOffset = 0;
        }
    }
}

void RandomGenerator::HashMixPool()
{
    BytesAddedSincePoolHashMix = 0;

    for (size_t poolPos = 0; poolPos < Pool.size();)
    {
        // Compute the message digest of the entire pool using the selected hash function
        SecureBuffer digest(PoolHash->GetDigestSize());
        PoolHash->ProcessData(Pool);
        PoolHash->GetDigest(digest);

        // Add the message digest to the pool
        for (size_t digestPos = 0; digestPos < digest.size() && poolPos < Pool.size(); ++digestPos)
        {
            Pool[poolPos++] += digest[digestPos];
        }
    }
}

void RandomGenerator::Start()
{
    QMutexLocker lock(&AccessMutex);

    if (IsRunning())
    {
        return;
    }

    BytesAddedSincePoolHashMix = 0;
    ReadOffset = 0;
    WriteOffset = 0;
    Running = true;

    Pool.allocate(PoolSize);

    if (!PoolHash)
    {
        // First hash algorithm is the default one
        PoolHash = Volume::VolumeHash::GetAvailableAlgorithms().front();
    }

    AddSystemDataToPool(true);
}

void RandomGenerator::Stop()
{
    QMutexLocker lock(&AccessMutex);

    if (Pool.isAllocated())
    {
        Pool.freeData();
    }

    PoolHash.reset();

    Running = false;
}

QMutex RandomGenerator::AccessMutex(QMutex::Recursive);
size_t RandomGenerator::BytesAddedSincePoolHashMix;
SecureBuffer RandomGenerator::Pool;
QSharedPointer <Volume::VolumeHash> RandomGenerator::PoolHash;
size_t RandomGenerator::ReadOffset;
bool RandomGenerator::Running = false;
size_t RandomGenerator::WriteOffset;
}
}
