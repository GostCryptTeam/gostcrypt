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
#include "Core/Buffer.h"
#include "Volume/Crypto/Random.h"
#include "Volume/VolumeHash.h"

namespace GostCrypt
{
namespace Core
{
/**
 * @brief Class in charge of generating random data using the the system random data source
 *  TODO: replace by a singleton and start/stop by constructor destructor
 */
class RandomGenerator
{
 public:
    /**
     * @brief Add the data of the given buffer to the data of the pool
     *
     * @param buffer Buffer containing the data to add to the data of the pool
     */
    static void AddToPool(const BufferPtr& buffer);
    /**
     * @brief Fill the buffer with random data (can take time if the system entropy is low)
     *
     * @param buffer Buffer to fill with random data
     */
    static void GetData(BufferPtr& buffer) { GetData(buffer, false); }
    /**
     * @brief Fill the buffer with random data (in fast mode)
     *
     * @param buffer Buffer to fill with random data
     */
    static void GetDataFast(BufferPtr& buffer) { GetData(buffer, true); }
    /**
     * @brief Return true if the RandomGenerator has been initialized and is ready to be used
     *
     * @return True if the RandomGenerator has been initialized and is ready to be used
     */
    static bool IsRunning() { return Running; }
    /**
     * @brief Change the hash algorithm to use by the random generator
     *
     * @param hashfct New hash algorithm to use
     */
    static void SetHash(QSharedPointer<Volume::VolumeHash> hashfct);
    /**
     * @brief Initialize the random generator so it can be used
     *
     */
    static void Start();
    /**
     * @brief Free memory used by the random generator so the  program can exit
     *
     */
    static void Stop();

    static const size_t PoolSize =
        RNG_POOL_SIZE; /**< Size of the data pool used by the random generator */

 protected:
    /**
     * @brief Add random data from the system to the  data in the pool
     *
     * @param fast Boolean true to use the urandom source (faster)
     */
    static void AddSystemDataToPool(bool fast);
    /**
     * @brief Fill the buffer with random data
     *
     * @param buffer Buffer to fill with random data
     * @param fast Boolean true to use the urandom source (faster)
     */
    static void GetData(BufferPtr& buffer, bool fast);
    /**
     * @brief Mix data in the pool using a hash function
     * The hash of the pool will be added to the data in the pool
     */
    static void HashMixPool();

    static const size_t MaxBytesAddedBeforePoolHashMix =
        RANDMIX_BYTE_INTERVAL; /**< Number of bytes added to the data in the pool between each hash mix */

    static QMutex AccessMutex; /**< Mutex to access the Randomgenerator static variables */
    static size_t BytesAddedSincePoolHashMix; /**< Number of bytes added to the data in the pool */
    static SecureBuffer
    Pool; /**< Pool buffer storing intermediate random data used by the random generator */
    static QSharedPointer <Volume::VolumeHash>
    PoolHash; /**< Hash algorithm used by the random generator for the HashMix */
    static size_t ReadOffset; /**< Offset in the pool where to read next */
    static bool Running; /**< Boolean true if the random generate has been initialized */
    static size_t WriteOffset; /**< Offset in the pool where to write next */
};
}
}

#endif // GST_HEADER_Core_RandomNumberGenerator
