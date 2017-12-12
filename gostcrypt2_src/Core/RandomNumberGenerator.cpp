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
#include "RandomNumberGenerator.h"
#include "Volume/Crc32.h"
#include "Volume/VolumeHashStribog.h"

namespace GostCrypt
{
	namespace Core {
	void RandomNumberGenerator::AddSystemDataToPool (bool fast)
	{
		SecureBuffer buffer (PoolSize);

        #ifdef HAVE_GETRANDOM
        int getrandomRet = getrandom(buffer, buffer.Size(), (fast) ? 0 : GRND_RANDOM);
        if(getrandomRet < buffer.Size()) {
            throw FailedUsingSystemRandomSourceException(getrandomRet);
        }
        #else
        int randsource;
        int readRet;
        if(fast){
            randsource = open ("/dev/urandom", O_RDONLY);
        } else {
			// Read all bytes available in /dev/random up to buffer size
            randsource = open ("/dev/random", O_RDONLY); // blocking as we are not in fast mode
        }
        if (randsource == -1)
            throw FailedOpenFileException(QFileInfo(QStringLiteral("/dev/urandom")));

        readRet = read (randsource, buffer, buffer.Size()) < (int64_t)buffer.Size();
        if(readRet < buffer.Size())
            throw FailedUsingSystemRandomSourceException(readRet);
        AddToPool (buffer);
        close(randsource);
        #endif
	}

    void RandomNumberGenerator::AddToPool (const BufferPtr &buffer)
	{
		if (!Running)
            throw RandomNumberGeneratorNotRunningException();

        QMutexLocker lock (&AccessMutex);

        for (size_t i = 0; i < buffer.Size(); ++i)
		{
            Pool[WriteOffset++] += buffer[i];

			if (WriteOffset >= PoolSize)
				WriteOffset = 0;

			if (++BytesAddedSincePoolHashMix >= MaxBytesAddedBeforePoolHashMix)
				HashMixPool();
        }
    }

    void RandomNumberGenerator::SetHash(QSharedPointer<Volume::VolumeHash> hashfct)
    {
        QMutexLocker lock(&RandomNumberGenerator::AccessMutex);
        RandomNumberGenerator::PoolHash = hashfct;
    }

    void RandomNumberGenerator::GetData (BufferPtr &buffer, bool fast)
	{
		if (!Running)
            throw RandomNumberGeneratorNotRunningException();

		if (buffer.Size() > PoolSize)
            throw IncorrectParameterException("buffer.size > PoolSize");

        QMutexLocker lock (&AccessMutex);

		// Poll system for data
		AddSystemDataToPool (fast);
		HashMixPool();

		// Transfer bytes from pool to output buffer
		for (size_t i = 0; i < buffer.Size(); ++i)
		{
			buffer[i] += Pool[ReadOffset++];

			if (ReadOffset >= PoolSize)
				ReadOffset = 0;
		}

		// Invert and mix the pool
		for (size_t i = 0; i < Pool.Size(); ++i)
		{
			Pool[i] = ~Pool[i];
		}

		AddSystemDataToPool (true);
		HashMixPool();

		// XOR the current pool content into the output buffer to prevent pool state leaks
		for (size_t i = 0; i < buffer.Size(); ++i)
		{
			buffer[i] ^= Pool[ReadOffset++];

			if (ReadOffset >= PoolSize)
				ReadOffset = 0;
		}
    }

	void RandomNumberGenerator::HashMixPool ()
	{
		BytesAddedSincePoolHashMix = 0;

		for (size_t poolPos = 0; poolPos < Pool.Size(); )
		{
			// Compute the message digest of the entire pool using the selected hash function
			SecureBuffer digest (PoolHash->GetDigestSize());
			PoolHash->ProcessData (Pool);
			PoolHash->GetDigest (digest);

			// Add the message digest to the pool
			for (size_t digestPos = 0; digestPos < digest.Size() && poolPos < Pool.Size(); ++digestPos)
			{
				Pool[poolPos++] += digest[digestPos];
			}
		}
	}

	void RandomNumberGenerator::Start ()
	{
        QMutexLocker lock (&AccessMutex);

		if (IsRunning())
			return;

		BytesAddedSincePoolHashMix = 0;
		ReadOffset = 0;
		WriteOffset = 0;
		Running = true;
		EnrichedByUser = false;

		Pool.Allocate (PoolSize);
		Test();

		if (!PoolHash)
		{
			// First hash algorithm is the default one
			PoolHash = Volume::VolumeHash::GetAvailableAlgorithms().front();
		}

		AddSystemDataToPool (true);
	}

	void RandomNumberGenerator::Stop ()
	{
        QMutexLocker lock (&AccessMutex);

		if (Pool.IsAllocated())
			Pool.Free ();

		PoolHash.reset();

		EnrichedByUser = false;
		Running = false;
	}

	void RandomNumberGenerator::Test ()
	{
		QSharedPointer <Volume::VolumeHash> origPoolHash = PoolHash;
        PoolHash.reset (new Volume::VolumeHashStribog());

        Pool.Erase(); // erase will set everything to zero
		Buffer buffer (1);
		for (size_t i = 0; i < PoolSize * 10; ++i)
		{
			buffer[0] = (quint8) i;
			AddToPool (buffer);
		}

		/*if (Crc32::ProcessBuffer (Pool) != 0x2de46d17)
			throw TestFailed (SRC_POS);*/

		buffer.Allocate (PoolSize);
		buffer.CopyFrom (PeekPool());
		AddToPool (buffer);

		/*if (Crc32::ProcessBuffer (Pool) != 0xcb88e019)
			throw TestFailed (SRC_POS);*/

		PoolHash = origPoolHash;
	}

    QMutex RandomNumberGenerator::AccessMutex(QMutex::Recursive);
	size_t RandomNumberGenerator::BytesAddedSincePoolHashMix;
	bool RandomNumberGenerator::EnrichedByUser;
	SecureBuffer RandomNumberGenerator::Pool;
	QSharedPointer <Volume::VolumeHash> RandomNumberGenerator::PoolHash;
	size_t RandomNumberGenerator::ReadOffset;
	bool RandomNumberGenerator::Running = false;
	size_t RandomNumberGenerator::WriteOffset;
}
}
