/*
 Copyright (c) 2008-2009 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Core_RandomNumberGenerator
#define GST_HEADER_Core_RandomNumberGenerator


#include <QMutex>
#include <QSharedPointer>
#include "Platform/Buffer.h"
#include "Volume/Crypto/Random.h"
#include "Volume/VolumeHash.h"

namespace GostCrypt {
	namespace Core {
	class RandomNumberGenerator
	{
	public:
        static void AddToPool (const ConstBufferPtr &buffer);
		static void GetData (const BufferPtr &buffer) { GetData (buffer, false); }
        static void GetDataFast (const BufferPtr &buffer) { GetData (buffer, true); }
		static bool IsRunning () { return Running; }
        static ConstBufferPtr PeekPool () { return Pool; }
        static void SetHash(QSharedPointer<Volume::VolumeHash> hashfct);
		static void Start ();
		static void Stop ();

		static const size_t PoolSize = RNG_POOL_SIZE;

	protected:
		static void AddSystemDataToPool (bool fast);
		static void GetData (const BufferPtr &buffer, bool fast);
		static void HashMixPool ();
		static void Test ();
		RandomNumberGenerator ();

		static const size_t MaxBytesAddedBeforePoolHashMix = RANDMIX_BYTE_INTERVAL;

        static QMutex AccessMutex;
		static size_t BytesAddedSincePoolHashMix;
		static bool EnrichedByUser;
		static SecureBuffer Pool;
		static QSharedPointer <Volume::VolumeHash> PoolHash;
		static size_t ReadOffset;
		static bool Running;
		static size_t WriteOffset;
	};
}
}

#endif // GST_HEADER_Core_RandomNumberGenerator
