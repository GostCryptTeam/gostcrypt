/**
 *
 *	\file Xts.c
 *	\brief Manage the XTS encryption which consists of switching bytes in blocks before encrypting them.
 *			It covers 32 and 64 bytes platforms. there are optimized function for parallel and non parallel.
 *	\version 1.3.1
 *	\date 23/01/2017
 *
 */

/**
 *
 *	Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.
 *	
 *	Governed by the TrueCrypt License 3.0 the full text of which is contained in
 *	the file License.txt included in TrueCrypt binary and source code distribution
 *	packages.
 *
 */

/* If native 64-bit data types are not available, define GST_NO_COMPILER_INT64. 

For big-endian platforms define BYTE_ORDER as BIG_ENDIAN. */


#ifdef GST_MINIMIZE_CODE_SIZE
//	Preboot/boot version
#	ifndef GST_NO_COMPILER_INT64
#		define GST_NO_COMPILER_INT64
#	endif
#	pragma optimize ("tl", on)
#endif

#ifdef GST_NO_COMPILER_INT64
#	include <memory.h>
#endif

#include "Xts.h"

#ifndef GST_NO_COMPILER_INT64

/**
 *
 *	\fn void EncryptBufferXTS (unsigned __int8 *buffer, GST_LARGEST_COMPILER_UINT length, const UINT64_STRUCT *startDataUnitNo, unsigned int startCipherBlockNo, unsigned __int8 *ks, unsigned __int8 *ks2, int cipher)
 *	\brief length: number of bytes to encrypt; may be larger than one data unit and must be divisible by the cipher block size
 *			ks: the primary key schedule
 *			ks2: the secondary key schedule
 *			startDataUnitNo: The sequential number of the data unit with which the buffer starts.
 *			startCipherBlockNo: The sequential number of the first plaintext block to encrypt inside the data unit startDataUnitNo.
 *								When encrypting the data unit from its first block, startCipherBlockNo is 0. 
 *								The startCipherBlockNo value applies only to the first data unit in the buffer; each successive
 *								data unit is encrypted from its first block. The start of the buffer does not have to be
 *								aligned with the start of a data unit. If it is aligned, startCipherBlockNo must be 0; if it
 *								is not aligned, startCipherBlockNo must reflect the misalignment accordingly.
 *	\param[in] unsigned __int8 * buffer
 *	\param[in] GST_LARGEST_COMPILER_UINT length
 *	\param[in] const UINT64_STRUCT * startDataUnitNo
 *	\param[in] unsigned int startCipherBlockNo
 *	\param[in] unsigned __int8 * ks
 *	\param[in] unsigned __int8 * ks2
 *	\param[in] int cipher
 *	\return void
 *
 */
void EncryptBufferXTS (unsigned __int8 *buffer,
					   GST_LARGEST_COMPILER_UINT length,
					   const UINT64_STRUCT *startDataUnitNo,
					   unsigned int startCipherBlockNo,
					   unsigned __int8 *ks,
					   unsigned __int8 *ks2,
					   int cipher)
{
	if (CipherSupportsIntraDataUnitParallelization (cipher))
		EncryptBufferXTSParallel (buffer, length, startDataUnitNo, startCipherBlockNo, ks, ks2, cipher);
	else
		EncryptBufferXTSNonParallel (buffer, length, startDataUnitNo, startCipherBlockNo, ks, ks2, cipher);
}

/**
 *
 *	\fn void EncryptBufferXTS8Byte(unsigned __int8 *buffer, GST_LARGEST_COMPILER_UINT length, const UINT64_STRUCT *startDataUnitNo, unsigned int startCipherBlockNo, unsigned __int8 *ks, unsigned __int8 *ks2, int cipher)
 *	\brief The goal is to switch some byte of a encrypted data with the next encrypted data.
 *			This doesn't hurt the algorithm security and add a switching parameter to the encryption.
 *			On a 32-byte data it switches data with another block of data Then encrypt it.
 *	\param[in] unsigned __int8 * buffer
 *	\param[in] GST_LARGEST_COMPILER_UINT length
 *	\param[in] const UINT64_STRUCT * startDataUnitNo
 *	\param[in] unsigned int startCipherBlockNo
 *	\param[in] unsigned __int8 * ks
 *	\param[in] unsigned __int8 * ks2
 *	\param[in] int cipher
 *	\return void
 *
 */
void EncryptBufferXTS8Byte(unsigned __int8 *buffer,
						GST_LARGEST_COMPILER_UINT length,
						const UINT64_STRUCT *startDataUnitNo,
						unsigned int startCipherBlockNo,
						unsigned __int8 *ks,
						unsigned __int8 *ks2,
						int cipher)
{
	unsigned __int8 finalCarry;
	unsigned __int8 whiteningValue [BYTES_PER_XTS_BLOCK_SMALL];
	unsigned __int8 byteBufUnitNo [BYTES_PER_XTS_BLOCK_SMALL];
	unsigned __int32 *whiteningValuePtr32 = (unsigned __int32 *)whiteningValue;
	unsigned __int32 *bufPtr = (unsigned __int32 *)buffer;
	unsigned __int32 startBlock = startCipherBlockNo, endBlock, block;
	GST_LARGEST_COMPILER_UINT blockCount, dataUnitNo;
	unsigned __int8 xor_ks[MAX_EXPANDED_KEY];

	dataUnitNo = startDataUnitNo->Value;
	*((unsigned __int64 *) byteBufUnitNo) = (unsigned __int64) LE64(dataUnitNo);

	if (length % BYTES_PER_XTS_BLOCK_SMALL)
		GST_THROW_FATAL_EXCEPTION;

	blockCount = length / BYTES_PER_XTS_BLOCK_SMALL;

	memcpy(xor_ks, ks, CipherGetKeyScheduleSize(cipher));

	while (blockCount > 0)
	{
		if (blockCount < BLOCKS_PER_XTS_DATA_UNIT_SMALL)
			endBlock = startBlock + (unsigned __int32) blockCount;
		else
			endBlock = BLOCKS_PER_XTS_DATA_UNIT_SMALL;

		whiteningValuePtr32 = (unsigned __int32 *) whiteningValue;

		//Generate first whitening value
		*whiteningValuePtr32 = *((unsigned __int32 *) byteBufUnitNo);
		*(whiteningValuePtr32+1) = *((unsigned __int32 *) byteBufUnitNo+1);
		EncipherBlock (cipher, whiteningValue, ks2);

		//XOR ks with the current DataUnitNo
		XorKeySchedule(cipher, ks, xor_ks, byteBufUnitNo, 8);

		//Generate subsequent whitening values for blocks
		for (block = 0; block < endBlock; block++)
		{
			if (block >= startBlock)
			{
				//Pre-whitening
				*bufPtr++ ^= *whiteningValuePtr32++;
				*bufPtr-- ^= *whiteningValuePtr32--;

				//Actual encryption
				EncipherBlock(cipher, bufPtr, xor_ks);

				//Post-whitening
				*bufPtr++ ^= *whiteningValuePtr32++;
				*bufPtr++ ^= *whiteningValuePtr32;
			}
			else
				whiteningValuePtr32++;

			//Derive the next whitening value
#if BYTE_ORDER == LITTLE_ENDIAN

			//Little-endian platforms
			finalCarry = (*whiteningValuePtr32 & 0x80000000) ? 27 : 0;

			*whiteningValuePtr32-- <<= 1;

			if (*whiteningValuePtr32 & 0x80000000)
				*(whiteningValuePtr32 + 1) |= 1;

			*whiteningValuePtr32 <<= 1;
#else
			//Big-endian platforms
			finalCarry = (*whiteningValuePtr32 & 0x80) ? 27 : 0;

			*whiteningValuePtr32 = LE32 (LE32 (*whiteningValuePtr32) << 1);

			whiteningValuePtr32--;

			if (*whiteningValuePtr32 & 0x80)
				*(whiteningValuePtr32 + 1) |= 0x01000000;

			*whiteningValuePtr32 = LE32 (LE32 (*whiteningValuePtr32) << 1);
#endif
			whiteningValue[0] ^= finalCarry;
		}
		blockCount -= endBlock - startBlock;
		startBlock = 0;
		dataUnitNo++;
		*((unsigned __int64 *) byteBufUnitNo) = (unsigned __int64) LE64 (dataUnitNo);
	}
	FAST_ERASE32(whiteningValue, sizeof(whiteningValue));
}


/**
 *
 *	\fn static void EncryptBufferXTSParallel (unsigned __int8 *buffer, GST_LARGEST_COMPILER_UINT length, const UINT64_STRUCT *startDataUnitNo, unsigned int startCipherBlockNo, unsigned __int8 *ks, unsigned __int8 *ks2, int cipher)
 *	\brief Optimized for encryption algorithms supporting intra-data-unit parallelization.
 *			Encrypt data and add a switching parameter when encrypting.
 *			On a 64-byte data it switches a 16bytes data with another block of data Then encrypt it.
 *	\param[in] unsigned __int8 * buffer
 *	\param[in] GST_LARGEST_COMPILER_UINT length
 *	\param[in] const UINT64_STRUCT * startDataUnitNo
 *	\param[in] unsigned int startCipherBlockNo
 *	\param[in] unsigned __int8 * ks
 *	\param[in] unsigned __int8 * ks2
 *	\param[in] int cipher
 *	\return static void
 *
 */
static void EncryptBufferXTSParallel (unsigned __int8 *buffer,
					   GST_LARGEST_COMPILER_UINT length,
					   const UINT64_STRUCT *startDataUnitNo,
					   unsigned int startCipherBlockNo,
					   unsigned __int8 *ks,
					   unsigned __int8 *ks2,
					   int cipher)
{
	unsigned __int8 finalCarry;
	unsigned __int8 whiteningValues [ENCRYPTION_DATA_UNIT_SIZE];
	unsigned __int8 whiteningValue [BYTES_PER_XTS_BLOCK];
	unsigned __int8 byteBufUnitNo [BYTES_PER_XTS_BLOCK];
	unsigned __int64 *whiteningValuesPtr64 = (unsigned __int64 *) whiteningValues;
	unsigned __int64 *whiteningValuePtr64 = (unsigned __int64 *) whiteningValue;
	unsigned __int64 *bufPtr = (unsigned __int64 *) buffer;
	unsigned __int64 *dataUnitBufPtr;
	unsigned int startBlock = startCipherBlockNo, endBlock, block;
	unsigned __int64 *const finalInt64WhiteningValuesPtr = whiteningValuesPtr64 + sizeof (whiteningValues) / sizeof (*whiteningValuesPtr64) - 1;
	GST_LARGEST_COMPILER_UINT blockCount, dataUnitNo;

	/* The encrypted data unit number (i.e. the resultant ciphertext block) is to be multiplied in the
	finite field GF(2^128) by j-th power of n, where j is the sequential plaintext/ciphertext block
	number and n is 2, a primitive element of GF(2^128). This can be (and is) simplified and implemented
	as a left shift of the preceding whitening value by one bit (with carry propagating). In addition, if
	the shift of the highest byte results in a carry, 135 is XORed into the lowest byte. The value 135 is
	derived from the modulus of the Galois Field (x^128+x^7+x^2+x+1). */

	// Convert the 64-bit data unit number into a little-endian 16-byte array. 
	// Note that as we are converting a 64-bit number into a 16-byte array we can always zero the last 8 bytes.
	dataUnitNo = startDataUnitNo->Value;
	*((unsigned __int64 *) byteBufUnitNo) = LE64 (dataUnitNo);
	*((unsigned __int64 *) byteBufUnitNo + 1) = 0;

	if (length % BYTES_PER_XTS_BLOCK)
		GST_THROW_FATAL_EXCEPTION;

	blockCount = length / BYTES_PER_XTS_BLOCK;

	// Process all blocks in the buffer
	while (blockCount > 0)
	{
		if (blockCount < BLOCKS_PER_XTS_DATA_UNIT)
			endBlock = startBlock + (unsigned int) blockCount;
		else
			endBlock = BLOCKS_PER_XTS_DATA_UNIT;

		whiteningValuesPtr64 = finalInt64WhiteningValuesPtr;
		whiteningValuePtr64 = (unsigned __int64 *) whiteningValue;

		// Encrypt the data unit number using the secondary key (in order to generate the first 
		// whitening value for this data unit)
		*whiteningValuePtr64 = *((unsigned __int64 *) byteBufUnitNo);
		*(whiteningValuePtr64 + 1) = 0;
		EncipherBlock (cipher, whiteningValue, ks2);

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
				whiteningValuePtr64++;

			// Derive the next whitening value

#if BYTE_ORDER == LITTLE_ENDIAN

			// Little-endian platforms

			finalCarry = 
				(*whiteningValuePtr64 & 0x8000000000000000) ?
				135 : 0;

			*whiteningValuePtr64-- <<= 1;

			if (*whiteningValuePtr64 & 0x8000000000000000)
				*(whiteningValuePtr64 + 1) |= 1;	

			*whiteningValuePtr64 <<= 1;
#else

			// Big-endian platforms

			finalCarry = 
				(*whiteningValuePtr64 & 0x80) ?
				135 : 0;

			*whiteningValuePtr64 = LE64 (LE64 (*whiteningValuePtr64) << 1);

			whiteningValuePtr64--;

			if (*whiteningValuePtr64 & 0x80)
				*(whiteningValuePtr64 + 1) |= 0x0100000000000000;	

			*whiteningValuePtr64 = LE64 (LE64 (*whiteningValuePtr64) << 1);
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
		EncipherBlocks (cipher, dataUnitBufPtr, ks, endBlock - startBlock);
		
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
		*((unsigned __int64 *) byteBufUnitNo) = LE64 (dataUnitNo);
	}

	FAST_ERASE64 (whiteningValue, sizeof (whiteningValue));
	FAST_ERASE64 (whiteningValues, sizeof (whiteningValues));
}


/**
 *
 *	\fn static void EncryptBufferXTSNonParallel (unsigned __int8 *buffer, GST_LARGEST_COMPILER_UINT length, const UINT64_STRUCT *startDataUnitNo, unsigned int startCipherBlockNo, unsigned __int8 *ks, unsigned __int8 *ks2, int cipher)
 *	\brief Optimized for encryption algorithms not supporting intra-data-unit parallelization.
 *			Encrypt data and add a switching parameter when encrypting.
 *			On a 64-byte data it switches a 16bytes data with another block of data Then encrypt it.
 *	\param[in] unsigned __int8 * buffer
 *	\param[in] GST_LARGEST_COMPILER_UINT length
 *	\param[in] const UINT64_STRUCT * startDataUnitNo
 *	\param[in] unsigned int startCipherBlockNo
 *	\param[in] unsigned __int8 * ks
 *	\param[in] unsigned __int8 * ks2
 *	\param[in] int cipher
 *	\return static void
 *
 */
static void EncryptBufferXTSNonParallel (unsigned __int8 *buffer,
					   GST_LARGEST_COMPILER_UINT length,
					   const UINT64_STRUCT *startDataUnitNo,
					   unsigned int startCipherBlockNo,
					   unsigned __int8 *ks,
					   unsigned __int8 *ks2,
					   int cipher)
{
	unsigned __int8 finalCarry;
	unsigned __int8 whiteningValue [BYTES_PER_XTS_BLOCK];
	unsigned __int8 byteBufUnitNo [BYTES_PER_XTS_BLOCK];
	unsigned __int64 *whiteningValuePtr64 = (unsigned __int64 *) whiteningValue;
	unsigned __int64 *bufPtr = (unsigned __int64 *) buffer;
	unsigned int startBlock = startCipherBlockNo, endBlock, block;
	GST_LARGEST_COMPILER_UINT blockCount, dataUnitNo;

	/* The encrypted data unit number (i.e. the resultant ciphertext block) is to be multiplied in the
	finite field GF(2^128) by j-th power of n, where j is the sequential plaintext/ciphertext block
	number and n is 2, a primitive element of GF(2^128). This can be (and is) simplified and implemented
	as a left shift of the preceding whitening value by one bit (with carry propagating). In addition, if
	the shift of the highest byte results in a carry, 135 is XORed into the lowest byte. The value 135 is
	derived from the modulus of the Galois Field (x^128+x^7+x^2+x+1). */

	// Convert the 64-bit data unit number into a little-endian 16-byte array. 
	// Note that as we are converting a 64-bit number into a 16-byte array we can always zero the last 8 bytes.
	dataUnitNo = startDataUnitNo->Value;
	*((unsigned __int64 *) byteBufUnitNo) = LE64 (dataUnitNo);
	*((unsigned __int64 *) byteBufUnitNo + 1) = 0;

	if (length % BYTES_PER_XTS_BLOCK)
		GST_THROW_FATAL_EXCEPTION;

	blockCount = length / BYTES_PER_XTS_BLOCK;

	// Process all blocks in the buffer
	while (blockCount > 0)
	{
		if (blockCount < BLOCKS_PER_XTS_DATA_UNIT)
			endBlock = startBlock + (unsigned int) blockCount;
		else
			endBlock = BLOCKS_PER_XTS_DATA_UNIT;

		whiteningValuePtr64 = (unsigned __int64 *) whiteningValue;

		// Encrypt the data unit number using the secondary key (in order to generate the first 
		// whitening value for this data unit)
		*whiteningValuePtr64 = *((unsigned __int64 *) byteBufUnitNo);
		*(whiteningValuePtr64 + 1) = 0;
		EncipherBlock (cipher, whiteningValue, ks2);

		// Generate (and apply) subsequent whitening values for blocks in this data unit and
		// encrypt all relevant blocks in this data unit
		for (block = 0; block < endBlock; block++)
		{
			if (block >= startBlock)
			{
				// Pre-whitening
				*bufPtr++ ^= *whiteningValuePtr64++;
				*bufPtr-- ^= *whiteningValuePtr64--;

				// Actual encryption
				EncipherBlock (cipher, bufPtr, ks);

				// Post-whitening
				*bufPtr++ ^= *whiteningValuePtr64++;
				*bufPtr++ ^= *whiteningValuePtr64;
			}
			else
				whiteningValuePtr64++;

			// Derive the next whitening value

#if BYTE_ORDER == LITTLE_ENDIAN

			// Little-endian platforms

			finalCarry = 
				(*whiteningValuePtr64 & 0x8000000000000000) ?
				135 : 0;

			*whiteningValuePtr64-- <<= 1;

			if (*whiteningValuePtr64 & 0x8000000000000000)
				*(whiteningValuePtr64 + 1) |= 1;	

			*whiteningValuePtr64 <<= 1;
#else

			// Big-endian platforms

			finalCarry = 
				(*whiteningValuePtr64 & 0x80) ?
				135 : 0;

			*whiteningValuePtr64 = LE64 (LE64 (*whiteningValuePtr64) << 1);

			whiteningValuePtr64--;

			if (*whiteningValuePtr64 & 0x80)
				*(whiteningValuePtr64 + 1) |= 0x0100000000000000;	

			*whiteningValuePtr64 = LE64 (LE64 (*whiteningValuePtr64) << 1);
#endif

			whiteningValue[0] ^= finalCarry;
		}

		blockCount -= endBlock - startBlock;
		startBlock = 0;
		dataUnitNo++;
		*((unsigned __int64 *) byteBufUnitNo) = LE64 (dataUnitNo);
	}

	FAST_ERASE64 (whiteningValue, sizeof (whiteningValue));
}


/**
 *
 *	\fn void DecryptBufferXTS (unsigned __int8 *buffer, GST_LARGEST_COMPILER_UINT length, const UINT64_STRUCT *startDataUnitNo, unsigned int startCipherBlockNo, unsigned __int8 *ks, unsigned __int8 *ks2, int cipher)
 *	\brief For descriptions of the input parameters, see EncryptBufferXTS(). Decrypt before switching back the data
 *	\param[in] unsigned __int8 * buffer
 *	\param[in] GST_LARGEST_COMPILER_UINT length
 *	\param[in] const UINT64_STRUCT * startDataUnitNo
 *	\param[in] unsigned int startCipherBlockNo
 *	\param[in] unsigned __int8 * ks
 *	\param[in] unsigned __int8 * ks2
 *	\param[in] int cipher
 *	\return void
 *
 */
void DecryptBufferXTS (unsigned __int8 *buffer,
					   GST_LARGEST_COMPILER_UINT length,
					   const UINT64_STRUCT *startDataUnitNo,
					   unsigned int startCipherBlockNo,
					   unsigned __int8 *ks,
					   unsigned __int8 *ks2,
					   int cipher)
{
	if (CipherSupportsIntraDataUnitParallelization (cipher))
		DecryptBufferXTSParallel (buffer, length, startDataUnitNo, startCipherBlockNo, ks, ks2, cipher);
	else
		DecryptBufferXTSNonParallel (buffer, length, startDataUnitNo, startCipherBlockNo, ks, ks2, cipher);
}

void DecryptBufferXTS8Byte (unsigned __int8 *buffer,
					GST_LARGEST_COMPILER_UINT length,
					const UINT64_STRUCT *startDataUnitNo,
					unsigned int startCipherBlockNo,
					unsigned __int8 *ks,
					unsigned __int8 *ks2,
					int cipher)
{
	unsigned __int8 finalCarry;
	unsigned __int8 whiteningValue [BYTES_PER_XTS_BLOCK_SMALL];
	unsigned __int8 byteBufUnitNo [BYTES_PER_XTS_BLOCK_SMALL];
	unsigned __int32 *whiteningValuePtr32 = (unsigned __int32 *) whiteningValue;
	unsigned __int32 *bufPtr = (unsigned __int32 *) buffer;
	unsigned __int32 startBlock = startCipherBlockNo, endBlock, block;
	GST_LARGEST_COMPILER_UINT blockCount, dataUnitNo;
	unsigned __int8 xor_ks[MAX_EXPANDED_KEY];

	dataUnitNo = startDataUnitNo->Value;
	*((unsigned __int64 *) byteBufUnitNo) = (unsigned __int64) LE64 (dataUnitNo);

	if (length % BYTES_PER_XTS_BLOCK_SMALL)
		GST_THROW_FATAL_EXCEPTION;

	blockCount = length / BYTES_PER_XTS_BLOCK_SMALL;

	memcpy(xor_ks, ks, CipherGetKeyScheduleSize(cipher));

	while (blockCount > 0)
	{
		if (blockCount < BLOCKS_PER_XTS_DATA_UNIT_SMALL)
			endBlock = startBlock + (unsigned __int32) blockCount;
		else
			endBlock = BLOCKS_PER_XTS_DATA_UNIT_SMALL;

		whiteningValuePtr32 = (unsigned __int32 *) whiteningValue;

		//Encrypt data unit using the second key to generate the first whitening value
		*whiteningValuePtr32 = *((unsigned __int32 *) byteBufUnitNo);
		*(whiteningValuePtr32+1) = *((unsigned __int32 *) byteBufUnitNo+1);
		EncipherBlock (cipher, whiteningValue, ks2);

		//XOR ks with the current DataUnitNo
		XorKeySchedule(cipher, ks, xor_ks, byteBufUnitNo, 8);

		for (block = 0; block < endBlock; block++)
		{
			if (block >= startBlock)
			{
				//Post-whitening
				*bufPtr++ ^= *whiteningValuePtr32++;
				*bufPtr-- ^= *whiteningValuePtr32--;

				//Actual decryption
				DecipherBlock (cipher, bufPtr, xor_ks);

				//Pre-whitening
				*bufPtr++ ^= *whiteningValuePtr32++;
				*bufPtr++ ^= *whiteningValuePtr32;
			}
			else
				whiteningValuePtr32++;

			//Derive the next whitening value

#if BYTE_ORDER == LITTLE_ENDIAN

			//Little-endian platforms

			finalCarry = (*whiteningValuePtr32 & 0x80000000) ? 27 : 0;

			*whiteningValuePtr32-- <<= 1;

			if (*whiteningValuePtr32 & 0x80000000)
				*(whiteningValuePtr32 + 1) |= 1;

			*whiteningValuePtr32 <<= 1;
#else
			//Big-endian platforms
			finalCarry = (*whiteningValuePtr32 & 0x80) ? 27 : 0;

			*whiteningValuePtr32 = LE32 (LE32 (*whiteningValuePtr32) << 1);

			whiteningValuePtr32--;

			if (*whiteningValuePtr32 & 0x80)
				*(whiteningValuePtr32 + 1) |= 0x1000000;

			*whiteningValuePtr32 = LE32 (LE32 (*whiteningValuePtr32) << 1);
#endif

			whiteningValue[0] ^= finalCarry;
		}

		blockCount -= endBlock - startBlock;
		startBlock = 0;
		dataUnitNo++;
		*((unsigned __int64 *) byteBufUnitNo) = (unsigned __int64) LE64 (dataUnitNo);
	}

	FAST_ERASE32 (whiteningValue, sizeof (whiteningValue));
}

/**
 *
 *	\fn void DecryptBufferXTSParallel (unsigned __int8 *buffer, GST_LARGEST_COMPILER_UINT length, const UINT64_STRUCT *startDataUnitNo, unsigned int startCipherBlockNo, unsigned __int8 *ks, unsigned __int8 *ks2, int cipher)
 *	\brief Optimized for encryption algorithms supporting intra-data-unit parallelization. For descriptions of the input parameters, see EncryptBufferXTS(). Decrypt before switching back the data
 *	\param[in] unsigned __int8 * buffer
 *	\param[in] GST_LARGEST_COMPILER_UINT length
 *	\param[in] const UINT64_STRUCT * startDataUnitNo
 *	\param[in] unsigned int startCipherBlockNo
 *	\param[in] unsigned __int8 * ks
 *	\param[in] unsigned __int8 * ks2
 *	\param[in] int cipher
 *	\return static void
 *
 */
static void DecryptBufferXTSParallel (unsigned __int8 *buffer,
					   GST_LARGEST_COMPILER_UINT length,
					   const UINT64_STRUCT *startDataUnitNo,
					   unsigned int startCipherBlockNo,
					   unsigned __int8 *ks,
					   unsigned __int8 *ks2,
					   int cipher)
{
	unsigned __int8 finalCarry;
	unsigned __int8 whiteningValues [ENCRYPTION_DATA_UNIT_SIZE];
	unsigned __int8 whiteningValue [BYTES_PER_XTS_BLOCK];
	unsigned __int8 byteBufUnitNo [BYTES_PER_XTS_BLOCK];
	unsigned __int64 *whiteningValuesPtr64 = (unsigned __int64 *) whiteningValues;
	unsigned __int64 *whiteningValuePtr64 = (unsigned __int64 *) whiteningValue;
	unsigned __int64 *bufPtr = (unsigned __int64 *) buffer;
	unsigned __int64 *dataUnitBufPtr;
	unsigned int startBlock = startCipherBlockNo, endBlock, block;
	unsigned __int64 *const finalInt64WhiteningValuesPtr = whiteningValuesPtr64 + sizeof (whiteningValues) / sizeof (*whiteningValuesPtr64) - 1;
	GST_LARGEST_COMPILER_UINT blockCount, dataUnitNo;

	// Convert the 64-bit data unit number into a little-endian 16-byte array. 
	// Note that as we are converting a 64-bit number into a 16-byte array we can always zero the last 8 bytes.
	dataUnitNo = startDataUnitNo->Value;
	*((unsigned __int64 *) byteBufUnitNo) = LE64 (dataUnitNo);
	*((unsigned __int64 *) byteBufUnitNo + 1) = 0;

	if (length % BYTES_PER_XTS_BLOCK)
		GST_THROW_FATAL_EXCEPTION;

	blockCount = length / BYTES_PER_XTS_BLOCK;

	// Process all blocks in the buffer
	while (blockCount > 0)
	{
		if (blockCount < BLOCKS_PER_XTS_DATA_UNIT)
			endBlock = startBlock + (unsigned int) blockCount;
		else
			endBlock = BLOCKS_PER_XTS_DATA_UNIT;

		whiteningValuesPtr64 = finalInt64WhiteningValuesPtr;
		whiteningValuePtr64 = (unsigned __int64 *) whiteningValue;

		// Encrypt the data unit number using the secondary key (in order to generate the first 
		// whitening value for this data unit)
		*whiteningValuePtr64 = *((unsigned __int64 *) byteBufUnitNo);
		*(whiteningValuePtr64 + 1) = 0;
		EncipherBlock (cipher, whiteningValue, ks2);

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
				whiteningValuePtr64++;

			// Derive the next whitening value

#if BYTE_ORDER == LITTLE_ENDIAN

			// Little-endian platforms

			finalCarry = 
				(*whiteningValuePtr64 & 0x8000000000000000) ?
				135 : 0;

			*whiteningValuePtr64-- <<= 1;

			if (*whiteningValuePtr64 & 0x8000000000000000)
				*(whiteningValuePtr64 + 1) |= 1;	

			*whiteningValuePtr64 <<= 1;

#else
			// Big-endian platforms

			finalCarry = 
				(*whiteningValuePtr64 & 0x80) ?
				135 : 0;

			*whiteningValuePtr64 = LE64 (LE64 (*whiteningValuePtr64) << 1);

			whiteningValuePtr64--;

			if (*whiteningValuePtr64 & 0x80)
				*(whiteningValuePtr64 + 1) |= 0x0100000000000000;	

			*whiteningValuePtr64 = LE64 (LE64 (*whiteningValuePtr64) << 1);
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

		DecipherBlocks (cipher, dataUnitBufPtr, ks, endBlock - startBlock);

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

		*((unsigned __int64 *) byteBufUnitNo) = LE64 (dataUnitNo);
	}

	FAST_ERASE64 (whiteningValue, sizeof (whiteningValue));
	FAST_ERASE64 (whiteningValues, sizeof (whiteningValues));
}


// Optimized for encryption algorithms not supporting intra-data-unit parallelization
/**
 *
 *	\fn void DecryptBufferXTSNonParallel (unsigned __int8 *buffer, GST_LARGEST_COMPILER_UINT length, const UINT64_STRUCT *startDataUnitNo, unsigned int startCipherBlockNo, unsigned __int8 *ks, unsigned __int8 *ks2, int cipher)
 *	\brief Optimized for encryption algorithms not supporting intra-data-unit parallelization. For descriptions of the input parameters, see EncryptBufferXTS(). Decrypt before switching back the data
 *	\param[in] unsigned __int8 * buffer
 *	\param[in] GST_LARGEST_COMPILER_UINT length
 *	\param[in] const UINT64_STRUCT * startDataUnitNo
 *	\param[in] unsigned int startCipherBlockNo
 *	\param[in] unsigned __int8 * ks
 *	\param[in] unsigned __int8 * ks2
 *	\param[in] int cipher
 *	\return static void
 *
 */
static void DecryptBufferXTSNonParallel (unsigned __int8 *buffer,
					   GST_LARGEST_COMPILER_UINT length,
					   const UINT64_STRUCT *startDataUnitNo,
					   unsigned int startCipherBlockNo,
					   unsigned __int8 *ks,
					   unsigned __int8 *ks2,
					   int cipher)
{
	unsigned __int8 finalCarry;
	unsigned __int8 whiteningValue [BYTES_PER_XTS_BLOCK];
	unsigned __int8 byteBufUnitNo [BYTES_PER_XTS_BLOCK];
	unsigned __int64 *whiteningValuePtr64 = (unsigned __int64 *) whiteningValue;
	unsigned __int64 *bufPtr = (unsigned __int64 *) buffer;
	unsigned int startBlock = startCipherBlockNo, endBlock, block;
	GST_LARGEST_COMPILER_UINT blockCount, dataUnitNo;

	// Convert the 64-bit data unit number into a little-endian 16-byte array. 
	// Note that as we are converting a 64-bit number into a 16-byte array we can always zero the last 8 bytes.
	dataUnitNo = startDataUnitNo->Value;
	*((unsigned __int64 *) byteBufUnitNo) = LE64 (dataUnitNo);
	*((unsigned __int64 *) byteBufUnitNo + 1) = 0;

	if (length % BYTES_PER_XTS_BLOCK)
		GST_THROW_FATAL_EXCEPTION;

	blockCount = length / BYTES_PER_XTS_BLOCK;

	// Process all blocks in the buffer
	while (blockCount > 0)
	{
		if (blockCount < BLOCKS_PER_XTS_DATA_UNIT)
			endBlock = startBlock + (unsigned int) blockCount;
		else
			endBlock = BLOCKS_PER_XTS_DATA_UNIT;

		whiteningValuePtr64 = (unsigned __int64 *) whiteningValue;

		// Encrypt the data unit number using the secondary key (in order to generate the first 
		// whitening value for this data unit)
		*whiteningValuePtr64 = *((unsigned __int64 *) byteBufUnitNo);
		*(whiteningValuePtr64 + 1) = 0;
		EncipherBlock (cipher, whiteningValue, ks2);

		// Generate (and apply) subsequent whitening values for blocks in this data unit and
		// decrypt all relevant blocks in this data unit
		for (block = 0; block < endBlock; block++)
		{
			if (block >= startBlock)
			{
				// Post-whitening
				*bufPtr++ ^= *whiteningValuePtr64++;
				*bufPtr-- ^= *whiteningValuePtr64--;

				// Actual decryption
				DecipherBlock (cipher, bufPtr, ks);

				// Pre-whitening
				*bufPtr++ ^= *whiteningValuePtr64++;
				*bufPtr++ ^= *whiteningValuePtr64;
			}
			else
				whiteningValuePtr64++;

			// Derive the next whitening value

#if BYTE_ORDER == LITTLE_ENDIAN

			// Little-endian platforms

			finalCarry = 
				(*whiteningValuePtr64 & 0x8000000000000000) ?
				135 : 0;

			*whiteningValuePtr64-- <<= 1;

			if (*whiteningValuePtr64 & 0x8000000000000000)
				*(whiteningValuePtr64 + 1) |= 1;	

			*whiteningValuePtr64 <<= 1;

#else
			// Big-endian platforms

			finalCarry = 
				(*whiteningValuePtr64 & 0x80) ?
				135 : 0;

			*whiteningValuePtr64 = LE64 (LE64 (*whiteningValuePtr64) << 1);

			whiteningValuePtr64--;

			if (*whiteningValuePtr64 & 0x80)
				*(whiteningValuePtr64 + 1) |= 0x0100000000000000;	

			*whiteningValuePtr64 = LE64 (LE64 (*whiteningValuePtr64) << 1);
#endif

			whiteningValue[0] ^= finalCarry;
		}

		blockCount -= endBlock - startBlock;
		startBlock = 0;
		dataUnitNo++;
		*((unsigned __int64 *) byteBufUnitNo) = LE64 (dataUnitNo);
	}

	FAST_ERASE64 (whiteningValue, sizeof (whiteningValue));
}


#else	// GST_NO_COMPILER_INT64

/* ---- The following code is to be used only when native 64-bit data types are not available. ---- */

#if BYTE_ORDER == BIG_ENDIAN
#error The GST_NO_COMPILER_INT64 version of the XTS code is not compatible with big-endian platforms
#endif 


/**
 *
 *	\fn static void Uint64ToLE16ByteArray (unsigned __int8 *byteBuf, unsigned __int32 highInt32, unsigned __int32 lowInt32)
 *	\brief When there is no 64 bytes types are not available. Build it with two 32 bytes data.
 *			Converts a 64-bit unsigned integer (passed as two 32-bit integers for compatibility with non-64-bit environments/platforms) into a little-endian 16-byte array.
 *	\param[in] unsigned __int32 lowInt32
 *	\param[in] unsigned __int32 * highInt32
 *	\param[in] unsigned __int8 * byteBuf
 *	\return static void
 *
 */
static void Uint64ToLE16ByteArray (unsigned __int8 *byteBuf, unsigned __int32 highInt32, unsigned __int32 lowInt32)
{
	unsigned __int32 *bufPtr32 = (unsigned __int32 *) byteBuf;

	*bufPtr32++ = lowInt32;
	*bufPtr32++ = highInt32;

	// We're converting a 64-bit number into a little-endian 16-byte array so we can zero the last 8 bytes
	*bufPtr32++ = 0;
	*bufPtr32 = 0;
}



/**
 *
 *	\fn static void EncryptDecryptBufferXTS32 (const unsigned __int8 *buffer, GST_LARGEST_COMPILER_UINT length, const UINT64_STRUCT *startDataUnitNo, unsigned int startBlock, unsigned __int8 *ks, unsigned __int8 *ks2, int cipher, BOOL decryption)
 *	\brief Encrypts or decrypts all blocks in the buffer in XTS mode. For descriptions of the input parameters,
 *		   see the 64-bit version of EncryptBufferXTS().
 *	\param[in] const unsigned __int8 * buffer
 *	\param[in] GST_LARGEST_COMPILER_UINT length
 *	\param[in] const UINT64_STRUCT * startDataUnitNo
 *	\param[in] unsigned int startBlock
 *	\param[in] unsigned __int8 * ks
 *	\param[in] unsigned __int8 * ks2
 *	\param[in] int cipher
 *	\param[in] BOOL decryption
 *	\return static void
 *
 */
static void EncryptDecryptBufferXTS32 (const unsigned __int8 *buffer,
							GST_LARGEST_COMPILER_UINT length,
							const UINT64_STRUCT *startDataUnitNo,
							unsigned int startBlock,
							unsigned __int8 *ks,
							unsigned __int8 *ks2,
							int cipher,
							BOOL decryption)
{
	GST_LARGEST_COMPILER_UINT blockCount;
	UINT64_STRUCT dataUnitNo;
	unsigned int block;
	unsigned int endBlock;
	unsigned __int8 byteBufUnitNo [BYTES_PER_XTS_BLOCK];
	unsigned __int8 whiteningValue [BYTES_PER_XTS_BLOCK];
	unsigned __int32 *bufPtr32 = (unsigned __int32 *) buffer;
	unsigned __int32 *whiteningValuePtr32 = (unsigned __int32 *) whiteningValue;
	unsigned __int8 finalCarry;
	unsigned __int32 *const finalDwordWhiteningValuePtr = whiteningValuePtr32 + sizeof (whiteningValue) / sizeof (*whiteningValuePtr32) - 1;

	// Store the 64-bit data unit number in a way compatible with non-64-bit environments/platforms
	dataUnitNo.HighPart = startDataUnitNo->HighPart;
	dataUnitNo.LowPart = startDataUnitNo->LowPart;

	blockCount = length / BYTES_PER_XTS_BLOCK;

	// Convert the 64-bit data unit number into a little-endian 16-byte array. 
	// (Passed as two 32-bit integers for compatibility with non-64-bit environments/platforms.)
	Uint64ToLE16ByteArray (byteBufUnitNo, dataUnitNo.HighPart, dataUnitNo.LowPart);

	// Generate whitening values for all blocks in the buffer
	while (blockCount > 0)
	{
		if (blockCount < BLOCKS_PER_XTS_DATA_UNIT)
			endBlock = startBlock + (unsigned int) blockCount;
		else
			endBlock = BLOCKS_PER_XTS_DATA_UNIT;

		// Encrypt the data unit number using the secondary key (in order to generate the first 
		// whitening value for this data unit)
		memcpy (whiteningValue, byteBufUnitNo, BYTES_PER_XTS_BLOCK);
		EncipherBlock (cipher, whiteningValue, ks2);

		// Generate (and apply) subsequent whitening values for blocks in this data unit and
		// encrypt/decrypt all relevant blocks in this data unit
		for (block = 0; block < endBlock; block++)
		{
			if (block >= startBlock)
			{
				whiteningValuePtr32 = (unsigned __int32 *) whiteningValue;

				// Whitening
				*bufPtr32++ ^= *whiteningValuePtr32++;
				*bufPtr32++ ^= *whiteningValuePtr32++;
				*bufPtr32++ ^= *whiteningValuePtr32++;
				*bufPtr32 ^= *whiteningValuePtr32;

				bufPtr32 -= BYTES_PER_XTS_BLOCK / sizeof (*bufPtr32) - 1;

				// Actual encryption/decryption
				if (decryption)
					DecipherBlock (cipher, bufPtr32, ks);
				else
					EncipherBlock (cipher, bufPtr32, ks);

				whiteningValuePtr32 = (unsigned __int32 *) whiteningValue;

				// Whitening
				*bufPtr32++ ^= *whiteningValuePtr32++;
				*bufPtr32++ ^= *whiteningValuePtr32++;
				*bufPtr32++ ^= *whiteningValuePtr32++;
				*bufPtr32++ ^= *whiteningValuePtr32;
			}

			// Derive the next whitening value

			finalCarry = 0;

			for (whiteningValuePtr32 = finalDwordWhiteningValuePtr;
				whiteningValuePtr32 >= (unsigned __int32 *) whiteningValue;
				whiteningValuePtr32--)
			{
				if (*whiteningValuePtr32 & 0x80000000)	// If the following shift results in a carry
				{
					if (whiteningValuePtr32 != finalDwordWhiteningValuePtr)	// If not processing the highest double word
					{
						// A regular carry
						*(whiteningValuePtr32 + 1) |= 1;
					}
					else 
					{
						// The highest byte shift will result in a carry
						finalCarry = 135;
					}
				}

				*whiteningValuePtr32 <<= 1;
			}

			whiteningValue[0] ^= finalCarry;
		}

		blockCount -= endBlock - startBlock;
		startBlock = 0;

		// Increase the data unit number by one
		if (!++dataUnitNo.LowPart)
		{
			dataUnitNo.HighPart++;
		}

		// Convert the 64-bit data unit number into a little-endian 16-byte array. 
		Uint64ToLE16ByteArray (byteBufUnitNo, dataUnitNo.HighPart, dataUnitNo.LowPart);
	}

	FAST_ERASE64 (whiteningValue, sizeof (whiteningValue));
}



/**
 *
 *	\fn void DecryptBufferXTS8Byte (const unsigned __int8 *buffer, GST_LARGEST_COMPILER_UINT length, const UINT64_STRUCT *startDataUnitNo, unsigned int startCipherBlockNo, unsigned __int8 *ks, unsigned __int8 *ks2, int cipher)
 *	\brief Decrypts in the buffer in XTS mode.
 *	\param[in] const unsigned __int8 * buffer
 *	\param[in] GST_LARGEST_COMPILER_UINT length
 *	\param[in] const UINT64_STRUCT * startDataUnitNo
 *	\param[in] unsigned int startCipherBlockNo
 *	\param[in] unsigned __int8 * ks
 *	\param[in] unsigned __int8 * ks2
 *	\param[in] int cipher
 *	\return void
 *
 */
void DecryptBufferXTS8Byte (unsigned __int8 *buffer,
					GST_LARGEST_COMPILER_UINT length,
					const UINT64_STRUCT *startDataUnitNo,
					unsigned int startCipherBlockNo,
					unsigned __int8 *ks,
					unsigned __int8 *ks2,
					int cipher)
{
	unsigned __int8 finalCarry;
	unsigned __int8 whiteningValue [BYTES_PER_XTS_BLOCK_SMALL];
	unsigned __int8 byteBufUnitNo [BYTES_PER_XTS_BLOCK_SMALL];
	unsigned __int32 *whiteningValuePtr32 = (unsigned __int32 *) whiteningValue;
	unsigned __int32 *bufPtr = (unsigned __int32 *) buffer;
	unsigned __int32 startBlock = startCipherBlockNo, endBlock, block;
	GST_LARGEST_COMPILER_UINT blockCount;
	UINT64_STRUCT dataUnitNo;
	unsigned __int8 xor_ks[MAX_EXPANDED_KEY];

	dataUnitNo.LowPart = startDataUnitNo->LowPart;
	dataUnitNo.HighPart = startDataUnitNo->HighPart;

	*((unsigned __int32 *) byteBufUnitNo) = (unsigned __int32) LE32 (dataUnitNo.LowPart);
	*((unsigned __int32 *) byteBufUnitNo + 1) = (unsigned __int32) LE32 (dataUnitNo.HighPart);

	if (length % BYTES_PER_XTS_BLOCK_SMALL)
		GST_THROW_FATAL_EXCEPTION;

	blockCount = length / BYTES_PER_XTS_BLOCK_SMALL;

	memcpy(xor_ks, ks, CipherGetKeyScheduleSize(cipher));
	
	while (blockCount > 0)
	{
		if (blockCount < BLOCKS_PER_XTS_DATA_UNIT_SMALL)
			endBlock = startBlock + (unsigned __int32) blockCount;
		else
			endBlock = BLOCKS_PER_XTS_DATA_UNIT_SMALL;

		whiteningValuePtr32 = (unsigned __int32 *) whiteningValue;

		//Encrypt data unit using the second key to generate the first whitening value
		*whiteningValuePtr32 = *((unsigned __int32 *) byteBufUnitNo);
		*(whiteningValuePtr32+1) = *((unsigned __int32 *) byteBufUnitNo+1);
		EncipherBlock (cipher, whiteningValue, ks2);

		//XOR ks with the current DataUnitNo
		XorKeySchedule(cipher, ks, xor_ks, byteBufUnitNo, 8);

		for (block = 0; block < endBlock; block++)
		{
			if (block >= startBlock)
			{
				//Post-whitening
				*bufPtr++ ^= *whiteningValuePtr32++;
				*bufPtr-- ^= *whiteningValuePtr32--;

				//Actual decryption
				DecipherBlock (cipher, bufPtr, xor_ks);

				//Pre-whitening
				*bufPtr++ ^= *whiteningValuePtr32++;
				*bufPtr++ ^= *whiteningValuePtr32;
			}
			else
				whiteningValuePtr32++;

			//Derive the next whitening value

#if BYTE_ORDER == LITTLE_ENDIAN

			//Little-endian platforms

			finalCarry = (*whiteningValuePtr32 & 0x80000000) ? 27 : 0;

			*whiteningValuePtr32-- <<= 1;

			if (*whiteningValuePtr32 & 0x80000000)
				*(whiteningValuePtr32 + 1) |= 1;

			*whiteningValuePtr32 <<= 1;
#else
			//Big-endian platforms
			finalCarry = (*whiteningValuePtr32 & 0x80) ? 27 : 0);

			*whiteningValuePtr32 = LE32 (LE32 (*whiteningValuePtr32) << 1);

			whiteningValuePtr32--;

			if (*whiteningValuePtr32 & 0x80)
				*(whiteningValuePtr32 + 1) |= 0x1000000;

			*whiteningValuePtr32 = LE32 (LE32 (*whiteningValuePtr32) << 1);
#endif

			whiteningValue[0] ^= finalCarry;
		}

		blockCount -= endBlock - startBlock;
		startBlock = 0;
		if (!++dataUnitNo.LowPart)
			dataUnitNo.HighPart++;
		*((unsigned __int32 *) byteBufUnitNo) = (unsigned __int32) LE32 (dataUnitNo.LowPart);
		*((unsigned __int32 *) byteBufUnitNo+1) = (unsigned __int32) LE32 (dataUnitNo.HighPart);
	}

	FAST_ERASE32 (whiteningValue, sizeof (whiteningValue));
}


/**
 *
 *	\fn void EncryptBufferXTS8Byte (const unsigned __int8 *buffer, GST_LARGEST_COMPILER_UINT length, const UINT64_STRUCT *startDataUnitNo, unsigned int startCipherBlockNo, unsigned __int8 *ks, unsigned __int8 *ks2, int cipher)
 *	\brief Encrypts in the buffer in XTS mode.
 *	\param[in] const unsigned __int8 * buffer
 *	\param[in] GST_LARGEST_COMPILER_UINT length
 *	\param[in] const UINT64_STRUCT * startDataUnitNo
 *	\param[in] unsigned int startCipherBlockNo
 *	\param[in] unsigned __int8 * ks
 *	\param[in] unsigned __int8 * ks2
 *	\param[in] int cipher
 *	\return void
 *
 */
void EncryptBufferXTS8Byte(unsigned __int8 *buffer,
						GST_LARGEST_COMPILER_UINT length,
						const UINT64_STRUCT *startDataUnitNo,
						unsigned int startCipherBlockNo,
						unsigned __int8 *ks,
						unsigned __int8 *ks2,
						int cipher)
{
	unsigned __int8 finalCarry;
	unsigned __int8 whiteningValue [BYTES_PER_XTS_BLOCK_SMALL];
	unsigned __int8 byteBufUnitNo [BYTES_PER_XTS_BLOCK_SMALL];
	unsigned __int32 *whiteningValuePtr32 = (unsigned __int32 *)whiteningValue;
	unsigned __int32 *bufPtr = (unsigned __int32 *)buffer;
	unsigned __int32 startBlock = startCipherBlockNo, endBlock, block;
	GST_LARGEST_COMPILER_UINT blockCount;
	UINT64_STRUCT dataUnitNo;
	unsigned __int8 xor_ks[MAX_EXPANDED_KEY];

	dataUnitNo.LowPart = startDataUnitNo->LowPart;
	dataUnitNo.HighPart = startDataUnitNo->HighPart;

	*((unsigned __int32 *) byteBufUnitNo) = (unsigned __int32) LE32 (dataUnitNo.LowPart);
	*((unsigned __int32 *) byteBufUnitNo + 1) = (unsigned __int32) LE32 (dataUnitNo.HighPart);

	if (length % BYTES_PER_XTS_BLOCK_SMALL)
		GST_THROW_FATAL_EXCEPTION;

	blockCount = length / BYTES_PER_XTS_BLOCK_SMALL;
	
	memcpy(xor_ks, ks, CipherGetKeyScheduleSize(cipher));

	while (blockCount > 0)
	{
		if (blockCount < BLOCKS_PER_XTS_DATA_UNIT_SMALL)
			endBlock = startBlock + (unsigned __int32) blockCount;
		else
			endBlock = BLOCKS_PER_XTS_DATA_UNIT_SMALL;

		whiteningValuePtr32 = (unsigned __int32 *) whiteningValue;

		//Generate first whitening value
		*whiteningValuePtr32 = *((unsigned __int32 *) byteBufUnitNo);
		*(whiteningValuePtr32+1) = *((unsigned __int32 *) byteBufUnitNo+1);
		EncipherBlock (cipher, whiteningValue, ks2);

		//XOR ks with the current DataUnitNo
		XorKeySchedule(cipher, ks, xor_ks, byteBufUnitNo, 8);

		//Generate subsequent whitening values for blocks
		for (block = 0; block < endBlock; block++)
		{
			if (block >= startBlock)
			{
				//Pre-whitening
				*bufPtr++ ^= *whiteningValuePtr32++;
				*bufPtr-- ^= *whiteningValuePtr32--;

				//Actual encryption
				EncipherBlock(cipher, bufPtr, xor_ks);

				//Post-whitening
				*bufPtr++ ^= *whiteningValuePtr32++;
				*bufPtr++ ^= *whiteningValuePtr32;
			}
			else
				whiteningValuePtr32++;

			//Derive the next whitening value
#if BYTE_ORDER == LITTLE_ENDIAN

			//Little-endian platforms
			finalCarry = (*whiteningValuePtr32 & 0x80000000) ? 27 : 0;

			*whiteningValuePtr32-- <<= 1;

			if (*whiteningValuePtr32 & 0x80000000)
				*(whiteningValuePtr32 + 1) |= 1;

			*whiteningValuePtr32 <<= 1;
#else
			//Big-endian platforms
			finalCarry = (*whiteningValuePtr32 & 0x80) ? 27 : 0;

			*whiteningValuePtr32 = LE32 (L32 (*whiteningValuePtr32) << 1);

			whiteningValuePtr32--;

			if (*whiteningValuePtr32 & 0x80)
				*(whiteningValuePtr32 + 1) |= 0x01000000;

			*whiteningValuePtr32 = LE32 (LE32 (*whiteningValuePtr32) << 1);
#endif
			whiteningValue[0] ^= finalCarry;
		}
		blockCount -= endBlock - startBlock;
		startBlock = 0;
		if (!++dataUnitNo.LowPart)
			dataUnitNo.HighPart++;
		*((unsigned __int32 *) byteBufUnitNo) = (unsigned __int32) LE32 (dataUnitNo.LowPart);
		*((unsigned __int32 *) byteBufUnitNo+1) = (unsigned __int32) LE32 (dataUnitNo.HighPart);
	}
	FAST_ERASE32(whiteningValue, sizeof(whiteningValue));
}


/**
 *
 *	\fn void EncryptBufferXTS (unsigned __int8 *buffer, GST_LARGEST_COMPILER_UINT length, const UINT64_STRUCT *startDataUnitNo, unsigned int startCipherBlockNo, unsigned __int8 *ks, unsigned __int8 *ks2, int cipher)
 *	\brief For descriptions of the input parameters, see the 64-bit version of EncryptBufferXTS() above.
 *	\param[in] unsigned __int8 * buffer
 *	\param[in] GST_LARGEST_COMPILER_UINT length
 *	\param[in] const UINT64_STRUCT * startDataUnitNo
 *	\param[in] unsigned int startCipherBlockNo
 *	\param[in] unsigned __int8 * ks
 *	\param[in] unsigned __int8 * ks2
 *	\param[in] int cipher
 *	\return void
 *
 */
void EncryptBufferXTS (unsigned __int8 *buffer,
					   GST_LARGEST_COMPILER_UINT length,
					   const UINT64_STRUCT *startDataUnitNo,
					   unsigned int startCipherBlockNo,
					   unsigned __int8 *ks,
					   unsigned __int8 *ks2,
					   int cipher)
{
	// Encrypt all plaintext blocks in the buffer
	EncryptDecryptBufferXTS32 (buffer, length, startDataUnitNo, startCipherBlockNo, ks, ks2, cipher, FALSE);
}


/**
 *
 *	\fn void DecryptBufferXTS (unsigned __int8 *buffer, GST_LARGEST_COMPILER_UINT length, const UINT64_STRUCT *startDataUnitNo, unsigned int startCipherBlockNo, unsigned __int8 *ks, unsigned __int8 *ks2, int cipher)
 *	\brief For descriptions of the input parameters, see the 64-bit version of EncryptBufferXTS().
 *	\param[in] unsigned __int8 * buffer
 *	\param[in] GST_LARGEST_COMPILER_UINT length
 *	\param[in] const UINT64_STRUCT * startDataUnitNo
 *	\param[in] unsigned int startCipherBlockNo
 *	\param[in] unsigned __int8 * ks
 *	\param[in] unsigned __int8 * ks2
 *	\param[in] int cipher
 *	\return void
 *
 */
void DecryptBufferXTS (unsigned __int8 *buffer,
					   GST_LARGEST_COMPILER_UINT length,
					   const UINT64_STRUCT *startDataUnitNo,
					   unsigned int startCipherBlockNo,
					   unsigned __int8 *ks,
					   unsigned __int8 *ks2,
					   int cipher)
{
	// Decrypt all ciphertext blocks in the buffer
	EncryptDecryptBufferXTS32 (buffer, length, startDataUnitNo, startCipherBlockNo, ks, ks2, cipher, TRUE);
}

#endif	// GST_NO_COMPILER_INT64
