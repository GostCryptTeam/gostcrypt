/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#include <QtEndian>
#include "EncryptionModeXTS.h"
#include "Crypto/Crypto.h"
#include "VolumeHeader.h"
#include "VolumeException.h"

namespace GostCrypt
{
namespace Volume
{

void EncryptionModeXTS::Encrypt(quint8* data, quint64 length) const
{
    EncryptBuffer(data, length, 0);
}

void EncryptionModeXTS::EncryptBuffer(quint8* data, quint64 length, quint64 startDataUnitNo) const
{
    //if_debug (ValidateState());

    CipherAlgorithmList::const_iterator iSecondaryCipher = SecondaryCiphers.begin();

    for (CipherAlgorithmList::const_iterator iCipher = Ciphers.begin(); iCipher != Ciphers.end();
            ++iCipher)
    {
        if ((**iCipher).GetBlockSize() == 8)
        {
            EncryptBufferXTS8Byte(**iCipher, **iSecondaryCipher, data, length, startDataUnitNo, 0);
        }
        else
        {
            EncryptBufferXTS(**iCipher, **iSecondaryCipher, data, length, startDataUnitNo + SectorOffset, 0);
        }
        ++iSecondaryCipher;
    }

    //assert (iSecondaryCipher == SecondaryCiphers.end());
}

void EncryptionModeXTS::EncryptBufferXTS8Byte(const CipherAlgorithm& cipher,
        const CipherAlgorithm& secondaryCipher, quint8* buffer, quint64 length, quint64 startDataUnitNo,
        unsigned int startCipherBlockNo)
{
    quint8 finalCarry;
    quint8 whiteningValue [BYTES_PER_XTS_BLOCK_SMALL];
    quint8 byteBufUnitNo [BYTES_PER_XTS_BLOCK_SMALL];

    quint8 xor_ks [MAX_EXPANDED_KEY];
    quint32* whiteningValuePtr32 = (quint32*)whiteningValue;
    quint32* bufPtr = (quint32*)buffer;
    quint32 startBlock = startCipherBlockNo, endBlock, block;
    quint64 blockCount, dataUnitNo;
    quint32 modulus = 27;

    dataUnitNo = startDataUnitNo;
    *((quint64*) byteBufUnitNo) = qToLittleEndian(dataUnitNo);

    if (length % BYTES_PER_XTS_BLOCK_SMALL)
    {
        throw IncorrectCipherDataLengthException();
    }

    blockCount = length / BYTES_PER_XTS_BLOCK_SMALL;

    //Store the original key schedule
    cipher.CopyCipherKey((quint8*)xor_ks);

    while (blockCount > 0)
    {
        if (blockCount < BLOCKS_PER_XTS_DATA_UNIT_SMALL)
        {
            endBlock = startBlock + (quint32) blockCount;
        }
        else
        {
            endBlock = BLOCKS_PER_XTS_DATA_UNIT_SMALL;
        }

        whiteningValuePtr32 = (quint32*) whiteningValue;

        //Generate first whitening value
        *whiteningValuePtr32 = *((quint32*) byteBufUnitNo);
        *(whiteningValuePtr32 + 1) = *((quint32*) byteBufUnitNo + 1);
        secondaryCipher.EncryptBlock(whiteningValue);

        //XOR ks with the current DataUnitNo
        cipher.XorCipherKey((quint8*)xor_ks, byteBufUnitNo, 8);

        //Generate subsequent whitening values for blocks
        for (block = 0; block < endBlock; block++)
        {
            if (block >= startBlock)
            {
                //Pre-whitening
                *bufPtr++ ^= *whiteningValuePtr32++;
                *bufPtr-- ^= *whiteningValuePtr32--;

                //Actual encryption
                cipher.EncryptWithKS((quint8*) bufPtr, (quint8*) xor_ks);

                //Post-whitening
                *bufPtr++ ^= *whiteningValuePtr32++;
                *bufPtr++ ^= *whiteningValuePtr32;
            }
            else
            {
                whiteningValuePtr32++;
            }

            //Derive the next whitening value
#if BYTE_ORDER == LITTLE_ENDIAN

            //Little-endian platforms
            finalCarry = (*whiteningValuePtr32 & 0x80000000) ? modulus : 0;

            *whiteningValuePtr32-- <<= 1;

            if (*whiteningValuePtr32 & 0x80000000)
            {
                *(whiteningValuePtr32 + 1) |= 1;
            }

            *whiteningValuePtr32 <<= 1;
#else
            //Big-endian platforms
            finalCarry = (*whiteningValuePtr32 & 0x80) ? modulus : 0;

            *whiteningValuePtr32 = Endian::Little(Endian::Little(*whiteningValuePtr32) << 1);

            whiteningValuePtr32--;

            if (*whiteningValuePtr32 & 0x80)
            {
                *(whiteningValuePtr32 + 1) |= 0x01000000;
            }

            *whiteningValuePtr32 = Endian::Little(Endian::Little(*whiteningValuePtr32) << 1);
#endif
            whiteningValue[0] ^= finalCarry;
        }

        blockCount -= endBlock - startBlock;
        startBlock = 0;
        dataUnitNo++;
        *((quint64*) byteBufUnitNo) = qToLittleEndian(dataUnitNo);
    }
    FAST_ERASE64(whiteningValue, sizeof(whiteningValue));
}

void EncryptionModeXTS::EncryptBufferXTS(const CipherAlgorithm& cipher,
        const CipherAlgorithm& secondaryCipher, quint8* buffer, quint64 length, quint64 startDataUnitNo,
        unsigned int startCipherBlockNo)
{
    quint8 finalCarry;
    quint8 whiteningValues [ENCRYPTION_DATA_UNIT_SIZE];
    quint8 whiteningValue [BYTES_PER_XTS_BLOCK];
    quint8 byteBufUnitNo [BYTES_PER_XTS_BLOCK];
    quint64* whiteningValuesPtr64 = (quint64*) whiteningValues;
    quint64* whiteningValuePtr64 = (quint64*) whiteningValue;
    quint64* bufPtr = (quint64*) buffer;
    quint64* dataUnitBufPtr;
    unsigned int startBlock = startCipherBlockNo, endBlock, block;
    quint64* const finalInt64WhiteningValuesPtr = whiteningValuesPtr64 + sizeof(
                whiteningValues) / sizeof(*whiteningValuesPtr64) - 1;
    quint64 blockCount, dataUnitNo;

    //moved to method call to make it static
    //startDataUnitNo += SectorOffset;

    /* The encrypted data unit number (i.e. the resultant ciphertext block) is to be multiplied in the
    finite field GF(2^128) by j-th power of n, where j is the sequential plaintext/ciphertext block
    number and n is 2, a primitive element of GF(2^128). This can be (and is) simplified and implemented
    as a left shift of the preceding whitening value by one bit (with carry propagating). In addition, if
    the shift of the highest byte results in a carry, 135 is XORed into the lowest byte. The value 135 is
    derived from the modulus of the Galois Field (x^128+x^7+x^2+x+1). */

    // Convert the 64-bit data unit number into a little-endian 16-byte array.
    // Note that as we are converting a 64-bit number into a 16-byte array we can always zero the last 8 bytes.
    dataUnitNo = startDataUnitNo;
    *((quint64*) byteBufUnitNo) = qToLittleEndian(dataUnitNo);
    *((quint64*) byteBufUnitNo + 1) = 0;

    if (length % BYTES_PER_XTS_BLOCK)
    {
        throw IncorrectCipherDataLengthException();
    }

    blockCount = length / BYTES_PER_XTS_BLOCK;

    // Process all blocks in the buffer
    while (blockCount > 0)
    {
        if (blockCount < BLOCKS_PER_XTS_DATA_UNIT)
        {
            endBlock = startBlock + (unsigned int) blockCount;
        }
        else
        {
            endBlock = BLOCKS_PER_XTS_DATA_UNIT;
        }

        whiteningValuesPtr64 = finalInt64WhiteningValuesPtr;
        whiteningValuePtr64 = (quint64*) whiteningValue;

        // Encrypt the data unit number using the secondary key (in order to generate the first
        // whitening value for this data unit)
        *whiteningValuePtr64 = *((quint64*) byteBufUnitNo);
        *(whiteningValuePtr64 + 1) = 0;
        secondaryCipher.EncryptBlock(whiteningValue);

        // Generate subsequent whitening values for blocks in this data unit. Note that all generated 128-bit
        // whitening values are stored in memory as a sequence of 64-bit integers in reverse order.
        for (block = 0; block < endBlock; block++)
        {
            if (block >= startBlock)
            {
                *whiteningValuesPtr64-- = *whiteningValuePtr64++;
                *whiteningValuesPtr64-- = *whiteningValuePtr64;
            }
            else
            {
                whiteningValuePtr64++;
            }

            // Derive the next whitening value

#if BYTE_ORDER == LITTLE_ENDIAN

            // Little-endian platforms

            finalCarry =
                (*whiteningValuePtr64 & 0x8000000000000000ULL) ?
                135 : 0;

            *whiteningValuePtr64-- <<= 1;

            if (*whiteningValuePtr64 & 0x8000000000000000ULL)
            {
                *(whiteningValuePtr64 + 1) |= 1;
            }

            *whiteningValuePtr64 <<= 1;
#else

            // Big-endian platforms

            finalCarry =
                (*whiteningValuePtr64 & 0x80) ?
                135 : 0;

            *whiteningValuePtr64 = Endian::Little(Endian::Little(*whiteningValuePtr64) << 1);

            whiteningValuePtr64--;

            if (*whiteningValuePtr64 & 0x80)
            {
                *(whiteningValuePtr64 + 1) |= 0x0100000000000000ULL;
            }

            *whiteningValuePtr64 = Endian::Little(Endian::Little(*whiteningValuePtr64) << 1);
#endif

            whiteningValue[0] ^= finalCarry;
        }

        dataUnitBufPtr = bufPtr;
        whiteningValuesPtr64 = finalInt64WhiteningValuesPtr;

        // Encrypt all blocks in this data unit

        for (block = startBlock; block < endBlock; block++)
        {
            // Pre-whitening
            *bufPtr++ ^= *whiteningValuesPtr64--;
            *bufPtr++ ^= *whiteningValuesPtr64--;
        }

        // Actual encryption
        cipher.EncryptBlocks((quint8*) dataUnitBufPtr, endBlock - startBlock);

        bufPtr = dataUnitBufPtr;
        whiteningValuesPtr64 = finalInt64WhiteningValuesPtr;

        for (block = startBlock; block < endBlock; block++)
        {
            // Post-whitening
            *bufPtr++ ^= *whiteningValuesPtr64--;
            *bufPtr++ ^= *whiteningValuesPtr64--;
        }

        blockCount -= endBlock - startBlock;
        startBlock = 0;
        dataUnitNo++;
        *((quint64*) byteBufUnitNo) = qToLittleEndian(dataUnitNo);
    }

    FAST_ERASE64(whiteningValue, sizeof(whiteningValue));
    FAST_ERASE64(whiteningValues, sizeof(whiteningValues));
}

void EncryptionModeXTS::EncryptSectorsCurrentThread(quint8* data, quint64 sectorIndex,
        quint64 sectorCount, size_t sectorSize) const
{
    EncryptBuffer(data, sectorCount * sectorSize, sectorIndex * sectorSize / ENCRYPTION_DATA_UNIT_SIZE);
}

size_t EncryptionModeXTS::GetKeySize() const
{
    if (Ciphers.empty())
    {
        throw EncryptionModeNotInitializedException();
    }
    size_t keySize = 0;
    for (const QSharedPointer<CipherAlgorithm> cipher : SecondaryCiphers)
    {
        keySize += cipher->GetKeySize();
    }

    return keySize;
}

void EncryptionModeXTS::Decrypt(quint8* data, quint64 length) const
{
    DecryptBuffer(data, length, 0);
}

void EncryptionModeXTS::DecryptBuffer(quint8* data, quint64 length, quint64 startDataUnitNo) const
{
    //if_debug (ValidateState());

    CipherAlgorithmList::const_iterator iSecondaryCipher = SecondaryCiphers.end();

    for (CipherAlgorithmList::const_reverse_iterator iCipher = Ciphers.rbegin();
            iCipher != Ciphers.rend(); ++iCipher)
    {
        --iSecondaryCipher;
        if ((**iCipher).GetBlockSize() == 8)
        {
            DecryptBufferXTS8Byte(**iCipher, **iSecondaryCipher, data, length, startDataUnitNo, 0);
        }
        else
        {
            DecryptBufferXTS(**iCipher, **iSecondaryCipher, data, length, startDataUnitNo + SectorOffset,
                             0);    //AskFiliol Why SectorOffset (always = 0)
        }
    }

    //assert (iSecondaryCipher == SecondaryCiphers.begin());
}

void EncryptionModeXTS::DecryptBufferXTS8Byte(const CipherAlgorithm& cipher,
        const CipherAlgorithm& secondaryCipher, quint8* buffer, quint64 length, quint64 startDataUnitNo,
        unsigned int startCipherBlockNo)
{
    quint8 finalCarry;
    quint8 whiteningValue [BYTES_PER_XTS_BLOCK_SMALL];
    quint8 byteBufUnitNo [BYTES_PER_XTS_BLOCK_SMALL];
    quint8 xor_ks [MAX_EXPANDED_KEY];
    quint32* whiteningValuePtr32 = (quint32*)whiteningValue;
    quint32* bufPtr = (quint32*)buffer;
    quint32 startBlock = startCipherBlockNo, endBlock, block;
    quint64 blockCount, dataUnitNo;

    quint32 modulus = 27;

    dataUnitNo = startDataUnitNo;
    *((quint64*) byteBufUnitNo) = qToLittleEndian(dataUnitNo);

    if (length % BYTES_PER_XTS_BLOCK_SMALL)
    {
        throw IncorrectCipherDataLengthException();
    }

    blockCount = length / BYTES_PER_XTS_BLOCK_SMALL;

    //Store the original key schedule
    cipher.CopyCipherKey((quint8*)xor_ks);

    while (blockCount > 0)
    {
        if (blockCount < BLOCKS_PER_XTS_DATA_UNIT_SMALL)
        {
            endBlock = startBlock + (quint32) blockCount;
        }
        else
        {
            endBlock = BLOCKS_PER_XTS_DATA_UNIT_SMALL;
        }

        whiteningValuePtr32 = (quint32*) whiteningValue;

        //Generate first whitening value
        *whiteningValuePtr32 = *((quint32*) byteBufUnitNo);
        *(whiteningValuePtr32 + 1) = *((quint32*) byteBufUnitNo + 1);
        secondaryCipher.EncryptBlock(whiteningValue);

        //XOR ks with the current DataUnitNo
        cipher.XorCipherKey((quint8*)xor_ks, byteBufUnitNo, 8);

        //Generate subsequent whitening values for blocks
        for (block = 0; block < endBlock; block++)
        {
            if (block >= startBlock)
            {
                //Pre-whitening
                *bufPtr++ ^= *whiteningValuePtr32++;
                *bufPtr-- ^= *whiteningValuePtr32--;

                //Actual encryption
                cipher.DecryptWithKS((quint8*) bufPtr, (quint8*) xor_ks);

                //Post-whitening
                *bufPtr++ ^= *whiteningValuePtr32++;
                *bufPtr++ ^= *whiteningValuePtr32;
            }
            else
            {
                whiteningValuePtr32++;
            }

            //Derive the next whitening value
#if BYTE_ORDER == LITTLE_ENDIAN

            //Little-endian platforms
            finalCarry = (*whiteningValuePtr32 & 0x80000000) ? modulus : 0;

            *whiteningValuePtr32-- <<= 1;

            if (*whiteningValuePtr32 & 0x80000000)
            {
                *(whiteningValuePtr32 + 1) |= 1;
            }

            *whiteningValuePtr32 <<= 1;
#else
            //Big-endian platforms
            finalCarry = (*whiteningValuePtr32 & 0x80) ? modulus : 0;

            *whiteningValuePtr32 = Endian::Little(Endian::Little(*whiteningValuePtr32) << 1);

            whiteningValuePtr32--;

            if (*whiteningValuePtr32 & 0x80)
            {
                *(whiteningValuePtr32 + 1) |= 0x01000000;
            }

            *whiteningValuePtr32 = Endian::Little(Endian::Little(*whiteningValuePtr32) << 1);
#endif
            whiteningValue[0] ^= finalCarry;
        }

        blockCount -= endBlock - startBlock;
        startBlock = 0;
        dataUnitNo++;
        *((quint64*) byteBufUnitNo) = qToLittleEndian(dataUnitNo);
    }
    FAST_ERASE64(whiteningValue, sizeof(whiteningValue));
}

void EncryptionModeXTS::DecryptBufferXTS(const CipherAlgorithm& cipher,
        const CipherAlgorithm& secondaryCipher, quint8* buffer, quint64 length, quint64 startDataUnitNo,
        unsigned int startCipherBlockNo)
{
    quint8 finalCarry;
    quint8 whiteningValues [ENCRYPTION_DATA_UNIT_SIZE];
    quint8 whiteningValue [BYTES_PER_XTS_BLOCK];
    quint8 byteBufUnitNo [BYTES_PER_XTS_BLOCK];
    quint64* whiteningValuesPtr64 = (quint64*) whiteningValues;
    quint64* whiteningValuePtr64 = (quint64*) whiteningValue;
    quint64* bufPtr = (quint64*) buffer;
    quint64* dataUnitBufPtr;
    unsigned int startBlock = startCipherBlockNo, endBlock, block;
    quint64* const finalInt64WhiteningValuesPtr = whiteningValuesPtr64 + sizeof(
                whiteningValues) / sizeof(*whiteningValuesPtr64) - 1;
    quint64 blockCount, dataUnitNo;

    //moved to method call
    //startDataUnitNo += SectorOffset;

    // Convert the 64-bit data unit number into a little-endian 16-byte array.
    // Note that as we are converting a 64-bit number into a 16-byte array we can always zero the last 8 bytes.
    dataUnitNo = startDataUnitNo;
    *((quint64*) byteBufUnitNo) = qToLittleEndian(dataUnitNo);
    *((quint64*) byteBufUnitNo + 1) = 0;

    if (length % BYTES_PER_XTS_BLOCK)
    {
        throw IncorrectCipherDataLengthException();
    }

    blockCount = length / BYTES_PER_XTS_BLOCK;

    // Process all blocks in the buffer
    while (blockCount > 0)
    {
        if (blockCount < BLOCKS_PER_XTS_DATA_UNIT)
        {
            endBlock = startBlock + (unsigned int) blockCount;
        }
        else
        {
            endBlock = BLOCKS_PER_XTS_DATA_UNIT;
        }

        whiteningValuesPtr64 = finalInt64WhiteningValuesPtr;
        whiteningValuePtr64 = (quint64*) whiteningValue;

        // Encrypt the data unit number using the secondary key (in order to generate the first
        // whitening value for this data unit)
        *whiteningValuePtr64 = *((quint64*) byteBufUnitNo);
        *(whiteningValuePtr64 + 1) = 0;
        secondaryCipher.EncryptBlock(whiteningValue);

        // Generate subsequent whitening values for blocks in this data unit. Note that all generated 128-bit
        // whitening values are stored in memory as a sequence of 64-bit integers in reverse order.
        for (block = 0; block < endBlock; block++)
        {
            if (block >= startBlock)
            {
                *whiteningValuesPtr64-- = *whiteningValuePtr64++;
                *whiteningValuesPtr64-- = *whiteningValuePtr64;
            }
            else
            {
                whiteningValuePtr64++;
            }

            // Derive the next whitening value

#if BYTE_ORDER == LITTLE_ENDIAN

            // Little-endian platforms

            finalCarry =
                (*whiteningValuePtr64 & 0x8000000000000000ULL) ?
                135 : 0;

            *whiteningValuePtr64-- <<= 1;

            if (*whiteningValuePtr64 & 0x8000000000000000ULL)
            {
                *(whiteningValuePtr64 + 1) |= 1;
            }

            *whiteningValuePtr64 <<= 1;

#else
            // Big-endian platforms

            finalCarry =
                (*whiteningValuePtr64 & 0x80) ?
                135 : 0;

            *whiteningValuePtr64 = Endian::Little(Endian::Little(*whiteningValuePtr64) << 1);

            whiteningValuePtr64--;

            if (*whiteningValuePtr64 & 0x80)
            {
                *(whiteningValuePtr64 + 1) |= 0x0100000000000000ULL;
            }

            *whiteningValuePtr64 = Endian::Little(Endian::Little(*whiteningValuePtr64) << 1);
#endif

            whiteningValue[0] ^= finalCarry;
        }

        dataUnitBufPtr = bufPtr;
        whiteningValuesPtr64 = finalInt64WhiteningValuesPtr;

        // Decrypt blocks in this data unit

        for (block = startBlock; block < endBlock; block++)
        {
            *bufPtr++ ^= *whiteningValuesPtr64--;
            *bufPtr++ ^= *whiteningValuesPtr64--;
        }

        cipher.DecryptBlocks((quint8*) dataUnitBufPtr, endBlock - startBlock);

        bufPtr = dataUnitBufPtr;
        whiteningValuesPtr64 = finalInt64WhiteningValuesPtr;

        for (block = startBlock; block < endBlock; block++)
        {
            *bufPtr++ ^= *whiteningValuesPtr64--;
            *bufPtr++ ^= *whiteningValuesPtr64--;
        }

        blockCount -= endBlock - startBlock;
        startBlock = 0;
        dataUnitNo++;

        *((quint64*) byteBufUnitNo) = qToLittleEndian(dataUnitNo);
    }

    FAST_ERASE64(whiteningValue, sizeof(whiteningValue));
    FAST_ERASE64(whiteningValues, sizeof(whiteningValues));
}

void EncryptionModeXTS::DecryptSectorsCurrentThread(quint8* data, quint64 sectorIndex,
        quint64 sectorCount, size_t sectorSize) const
{
    DecryptBuffer(data, sectorCount * sectorSize, sectorIndex * sectorSize / ENCRYPTION_DATA_UNIT_SIZE);
}

void EncryptionModeXTS::SetCiphers(const CipherAlgorithmList& ciphers)
{
    EncryptionMode::SetCiphers(ciphers);

    SecondaryCiphers.clear();

    for (const QSharedPointer<CipherAlgorithm> cipher : ciphers)
    {
        SecondaryCiphers.push_back(cipher->GetNew());
    }

    if (SecondaryKey.size() > 0)
    {
        SetSecondaryCipherKeys();
    }
}

void EncryptionModeXTS::SetKey(const BufferPtr& key)
{
    SecondaryKey.allocate(key.size());
    SecondaryKey.copyFrom(key);

    if (!SecondaryCiphers.empty())
    {
        SetSecondaryCipherKeys();
    }
}

void EncryptionModeXTS::SetSecondaryCipherKeys()
{
    size_t keyOffset = 0;
    for (QSharedPointer<CipherAlgorithm> cipher : SecondaryCiphers)
    {
        cipher->SetKey(SecondaryKey.getRange(keyOffset, cipher->GetKeySize()));
        keyOffset += cipher->GetKeySize();
    }

    KeySet = true;
}
}
}
