/**
 *
 *	\file Crypto.c
 *	\brief Used for encryption and decryption (cipher, keys...)
 *	\version 1.3.1
 *	\date 15/12/2016
 *
 */

/**
 *	Legal Notice: Some portions of the source code contained in this file were
 *	derived from the source code of Encryption for the Masses 2.02a, which is
 *	Copyright (c) 1998-2000 Paul Le Roux and which is governed by the 'License
 *	Agreement for Encryption for the Masses'. Modifications and additions to
 *	the original source code (contained in this file) and all other portions
 *	of this file are Copyright (c) 2003-2010 TrueCrypt Developers Association
 *	and are governed by the TrueCrypt License 3.0 the full text of which is
 *	contained in the file License.txt included in TrueCrypt binary and source
 *	code distribution packages. 
 */

#include "Gstdefs.h"
#include "Crypto.h"
#include "Xts.h"
#include "Crc.h"
#include "Common/Endian.h"
#include <string.h>
#ifndef GST_WINDOWS_BOOT
#include "EncryptionThreadPool.h"
#endif
#include "Volumes.h"

/* Update the following when adding a new cipher or EA:

   Crypto.h:
     ID #define
     MAX_EXPANDED_KEY #define

   Crypto.c:
     Ciphers[]
     EncryptionAlgorithms[]
     CipherInit()
     EncipherBlock()
     DecipherBlock()

*/

#ifndef GST_WINDOWS_BOOT_SINGLE_CIPHER_MODE

// Cipher configuration
static Cipher Ciphers[] =
{
//								Block Size	Key Size	Key Schedule Size
//	  ID		Name			(Bytes)		(Bytes)		(Bytes)
	{ GOST,     "GOST 28147-89",         8,         32,         GOST_KS           },
	{ GRASSHOPPER, "Gost Grasshopper",   16,        32,         GRASSHOPPER_KS    },
	{ 0,		0,				0,			0,			0					}
};


// Encryption algorithm configuration
static EncryptionAlgorithm EncryptionAlgorithms[] =
{
	//  Cipher(s)                     Modes						FormatEnabled

#ifndef GST_WINDOWS_BOOT

	{ { 0,						0 },	{ 0, 0 },				0 },	// Must be all-zero
	{ { GOST,					0 },	{ XTS8, 0 }, 1 },
	{ { GRASSHOPPER,			0 },	{ XTS, 0 }, 1 },
	{ { 0,						0 },	{ 0, 0 },				0 }		// Must be all-zero

#else // GST_WINDOWS_BOOT

	// Encryption algorithms available for boot drive encryption
	{ { 0,						0 }, { 0, 0 },		0 },	// Must be all-zero
	{ { GOST,               0 }, { XTS8, 0 },  1 },
	{ { GRASSHOPPER,        0 }, { XTS, 0 }, 1 },
	{ { 0,						0 }, { 0, 0 },		0 },	// Must be all-zero

#endif

};



// Hash algorithms
static Hash Hashes[] =
{	// ID			Name			Deprecated		System Encryption
	{ STRIBOG,		"GOST R 34.11-2012", FALSE,	TRUE },
	{ GOSTHASH,		"GOST R 34.11-94",	FALSE,	FALSE },
	{ WHIRLPOOL,	"Whirlpool",			FALSE,	FALSE },
	{ 0, 0, 0 }
};

/**
 *
 *	\fn int CipherInit (int cipher, unsigned char *key, unsigned __int8 *ks)
 *	\brief Initialise cipher
 *			Return values: 0 = success, ERR_CIPHER_INIT_FAILURE (fatal), ERR_CIPHER_INIT_WEAK_KEY (non-fatal)
 *	\param[in] unsigned __int8* ks
 *	\param[in] unsigned char* key
 *	\param[in] int cipher
 *	\return int 0 if success
 *			ERR_CIPHER_INIT_FAILURE if unknown/wrong cipher ID (fatal)
 *			ERR_CIPHER_INIT_WEAK_KEY (non-fatal)
 *
 */
int CipherInit (int cipher, unsigned char *key, unsigned __int8 *ks)
{
	int retVal = ERR_SUCCESS;

	switch (cipher)
	{
	case GOST:
		gost_set_key(key, (gost_kds *)ks);
		break;
	case GRASSHOPPER:
		grasshopper_set_key((unsigned long *)key, (grasshopper_kds*)ks);
		break;
	default:
		// Unknown/wrong cipher ID
		return ERR_CIPHER_INIT_FAILURE;
	}

	return retVal;
}

/**
 *
 *	\fn void EncipherBlock(int cipher, void *data, void *ks)
 *	\brief Encipher one block
 *	\param[in] void * ks
 *	\param[in] void * data
 *	\param[in] int cipher
 *	\return void
 *
 */
void EncipherBlock(int cipher, void *data, void *ks)
{
	switch (cipher)
	{
	case GOST:
		gost_encrypt((byte *)data, (byte *)data, ks);
		break;
	case GRASSHOPPER:
		grasshopper_encrypt(ks, (unsigned long *)data, (unsigned long *)data);
		break;
	default:			GST_THROW_FATAL_EXCEPTION;	// Unknown/wrong ID
	}
}

#ifndef GST_WINDOWS_BOOT

/**
 *
 *	\fn void EncipherBlocks (int cipher, void *dataPtr, void *ks, size_t blockCount)
 *	\brief Encipher blocks
 *	\param[in] size_t blockCount
 *	\param[in] void * ks
 *	\param[in] void * dataPtr
 *	\param[in] int cipher
 *	\return void
 *
 */
void EncipherBlocks (int cipher, void *dataPtr, void *ks, size_t blockCount)
{
	byte *data = dataPtr;
	size_t blockSize = CipherGetBlockSize (cipher);
	while (blockCount-- > 0)
	{
		EncipherBlock (cipher, data, ks);
		data += blockSize;
	}
}

#endif // !GST_WINDOWS_BOOT

/**
 *
 *	\fn void DecipherBlock(int cipher, void *data, void *ks)
 *	\brief Decipher one block
 *	\param[in] void * ks
 *	\param[in] void * data
 *	\param[in] int cipher
 *	\return void
 *
 */
void DecipherBlock(int cipher, void *data, void *ks)
{
	switch (cipher)
	{
	case GOST:
		gost_decrypt((byte *)data, (byte *)data, ks);
		break;
	case GRASSHOPPER:
		grasshopper_decrypt(ks, (unsigned long*)data, (unsigned long*)data);
		break;
	default:		GST_THROW_FATAL_EXCEPTION;	// Unknown/wrong ID
	}
}

#ifndef GST_WINDOWS_BOOT

/**
 *
 *	\fn void DecipherBlocks (int cipher, void *dataPtr, void *ks, size_t blockCount)
 *	\brief Decipher blocks
 *	\param[in] size_t blockCount
 *	\param[in] void * ks
 *	\param[in] void * dataPtr
 *	\param[in] int cipher
 *	\return void
 *
 */
void DecipherBlocks (int cipher, void *dataPtr, void *ks, size_t blockCount)
{
	byte *data = dataPtr;
	size_t blockSize = CipherGetBlockSize (cipher);
	while (blockCount-- > 0)
	{
		DecipherBlock (cipher, data, ks);
		data += blockSize;
	}
}

#endif // !GST_WINDOWS_BOOT


// Ciphers support

/**
 *
 *	\fn Cipher *CipherGet (int id)
 *	\brief Get cipher at id, return null if there is no cipher with this id
 *	\param[in] int id
 *	\return Cipher * if success
 *			NULL if no cipher at id
 *
 */
Cipher *CipherGet (int id)
{
	int i;
	for (i = 0; Ciphers[i].Id != 0; i++)
		if (Ciphers[i].Id == id)
			return &Ciphers[i];

	return NULL;
}

/**
 *
 *	\fn char *CipherGetName (int cipherId)
 *	\brief Get the name of a cipher at id
 *	\param[in] int cipherId
 *	\return char * name if success
 *			NULL if no cipher at id
 *
 */
char *CipherGetName (int cipherId)
{
	return CipherGet (cipherId) -> Name;
}

/**
 *
 *	\fn int CipherGetBlockSize (int cipherId)
 *	\brief Get the block size of a cipher at id
 *	\param[in] int cipherId
 *	\return int block size if success
 *			NULL if no cipher at id
 *
 */
int CipherGetBlockSize (int cipherId)
{
	return CipherGet (cipherId) -> BlockSize;
}

/**
 *
 *	\fn int CipherGetKeySize (int cipherId)
 *	\brief Get the key size of a cipher at id
 *	\param[in] int cipherId
 *	\return int key size if success
 *			NULL if no cipher at id
 *
 */
int CipherGetKeySize (int cipherId)
{
	return CipherGet (cipherId) -> KeySize;
}

/**
 *
 *	\fn int CipherGetKeyScheduleSize (int cipherId)
 *	\brief Get the key schedule size of a cipher at id
 *	\param[in] int cipherId
 *	\return int key schedul size if success
 *			NULL if no cipher at id
 *
 */
int CipherGetKeyScheduleSize (int cipherId)
{
	return CipherGet (cipherId) -> KeyScheduleSize;
}

#ifndef GST_WINDOWS_BOOT

/**
 *
 *	\fn BOOL CipherSupportsIntraDataUnitParallelization (int cipher)
 *	\brief useless
 *	\param[in] int cipher
 *	\return BOOL 0
 *
 */
BOOL CipherSupportsIntraDataUnitParallelization (int cipher)
{
	return 0;
}

#endif

/**
 *
 *	\fn void XorKeySchedule(int cipher, void *ks, void *out_ks, void *data, int len)
 *	\brief Start a crypting function depending on the algorithm.
 *	\param[in] int len
 *	\param[in] void * data
 *	\param[in] void * out_ks
 *	\param[in] void * ks
 *	\param[in] int cipher
 *	\return void
 *
 */
void XorKeySchedule(int cipher, void *ks, void *out_ks, void *data, int len)
{
	switch (cipher)
	{
	case GOST:
		gost_xor_ks(ks, out_ks, data, len / sizeof(__int32));
		break;
	case GRASSHOPPER:
		grasshopper_xor_ks(ks, out_ks, data, len / sizeof(__int32));
		break;
	default:
		GST_THROW_FATAL_EXCEPTION;
	}
}

// Encryption algorithms support

/**
 *
 *	\fn int EAGetFirst ()
 *	\brief Get first EA (half useless)
 *	\return int 1
 *
 */
int EAGetFirst ()
{
	return 1;
}


/**
 *
 *	\fn int EAGetCount (void)
 *	\brief Returns number of EAs
 *	\return int count
 *
 */
int EAGetCount (void)
{
	int ea, count = 0;

	for (ea = EAGetFirst (); ea != 0; ea = EAGetNext (ea))
	{
		count++;
	}
	return count;
}

/**
 *
 *	\fn int EAGetNext (int previousEA)
 *	\brief Get the next EA
 *	\param[in] int previousEA
 *	\return id int if there is a next EA
 *			else return 0
 *
 */
int EAGetNext (int previousEA)
{
	int id = previousEA + 1;
	if (EncryptionAlgorithms[id].Ciphers[0] != 0) return id;
	return 0;
}


/**
 *
 *	\fn int EAInit (int ea, unsigned char *key, unsigned __int8 *ks)
 *	\brief Return values: 0 = success, ERR_CIPHER_INIT_FAILURE (fatal), ERR_CIPHER_INIT_WEAK_KEY (non-fatal)
 *	\param[in] unsigned __int8* ks
 *	\param[in] unsigned char* key
 *	\param[in] int ea
 *	\return retVal int 0 if success
 *			ERR_CIPHER_INIT_FAILURE(fatal)
 *			ERR_CIPHER_INIT_WEAK_KEY(non-fatal)
 *
 */
int EAInit (int ea, unsigned char *key, unsigned __int8 *ks)
{
	int c, retVal = ERR_SUCCESS;

	if (ea == 0)
		return ERR_CIPHER_INIT_FAILURE;

	for (c = EAGetFirstCipher (ea); c != 0; c = EAGetNextCipher (ea, c))
	{
		switch (CipherInit (c, key, ks))
		{
		case ERR_CIPHER_INIT_FAILURE:
			return ERR_CIPHER_INIT_FAILURE;

		case ERR_CIPHER_INIT_WEAK_KEY:
			retVal = ERR_CIPHER_INIT_WEAK_KEY;		// Non-fatal error
			break;
		}

		key += CipherGetKeySize (c);
		ks += CipherGetKeyScheduleSize (c);
	}
	return retVal;
}


#ifndef GST_WINDOWS_BOOT

/**
 *
 *	\fn BOOL EAInitMode (PCRYPTO_INFO ci)
 *	\brief Initialize EA mode
 *	\param[in] PCRYPTO_INFO ci
 *	\return BOOL FALSE if (EAInit (ci->ea, ci->k2, ci->ks2) != ERR_SUCCESS)
 *			FALSE if (EAInit(ci->ea, ci->k2, ci->ks2) != ERR_SUCCESS)
 *			TRUE otherwise
 *
 */
BOOL EAInitMode (PCRYPTO_INFO ci)
{
	switch (ci->mode)
	{
	case XTS:
		// Secondary key schedule
		if (EAInit (ci->ea, ci->k2, ci->ks2) != ERR_SUCCESS)
			return FALSE;

		/* Note: XTS mode could potentially be initialized with a weak key causing all blocks in one data unit
		on the volume to be tweaked with zero tweaks (i.e. 512 bytes of the volume would be encrypted in ECB
		mode). However, to create a GostCrypt volume with such a weak key, each human being on Earth would have
		to create approximately 11,378,125,361,078,862 (about eleven quadrillion) GostCrypt volumes (provided 
		that the size of each of the volumes is 1024 terabytes). */
		break;
	case XTS8:
		if (EAInit(ci->ea, ci->k2, ci->ks2) != ERR_SUCCESS)
			return FALSE;
		break;

	default:		
		// Unknown/wrong ID
		GST_THROW_FATAL_EXCEPTION;
	}
	return TRUE;
}


/**
 *
 *	\fn char *EAGetName (char *buf, int ea)
 *	\brief Returns name of EA, cascaded cipher names are separated by hyphens
 *	\param[in] int ea
 *	\param[in] char * buf
 *	\return char * buf
 *
 */
char *EAGetName (char *buf, int ea)
{
	int i = EAGetLastCipher(ea);
	strcpy (buf, (i != 0) ? CipherGetName (i) : "?");

	while (i = EAGetPreviousCipher(ea, i))
	{
		strcat (buf, "-");
		strcat (buf, CipherGetName (i));
	}

	return buf;
}


/**
 *
 *	\fn int EAGetByName (char *name)
 *	\brief Get the EA by name
 *	\param[in] char* name
 *	\return int ea if (strcmp (n, name) == 0)
 *			0 else
 *
 */
int EAGetByName (char *name)
{
	int ea = EAGetFirst ();
	char n[128];

	do
	{
		EAGetName (n, ea);
		if (strcmp (n, name) == 0)
			return ea;
	}
	while (ea = EAGetNext (ea));

	return 0;
}

#endif // GST_WINDOWS_BOOT


/**
 *
 *	\fn int EAGetKeySize (int ea)
 *	\brief Returns sum of key sizes of all ciphers of the EA (in bytes)
 *	\param[in] int ea
 *	\return int size
 *
 */
int EAGetKeySize (int ea)
{
	int i = EAGetFirstCipher (ea);
	int size = CipherGetKeySize (i);

	while (i = EAGetNextCipher (ea, i))
	{
		size += CipherGetKeySize (i);
	}

	return size;
}


/**
 *
 *	\fn int EAGetFirstMode (int ea)
 *	\brief Returns the first mode of operation of EA
 *	\param[in] int ea
 *	\return int encryptionAlgorithm[].modes[]
 *
 */
int EAGetFirstMode (int ea)
{
	return (EncryptionAlgorithms[ea].Modes[0]);
}


/**
 *
 *	\fn int EAGetNextMode (int ea, int previousModeId)
 *	\brief Returns the next mode of operation of EA
 *	\param[in] int previousModeId
 *	\param[in] int ea
 *	\return int encryptionAlgorithm[].modes[] if (c == previousModeId) 
 *			0 else
 *
 */
int EAGetNextMode (int ea, int previousModeId)
{
	int c, i = 0;
	while (c = EncryptionAlgorithms[ea].Modes[i++])
	{
		if (c == previousModeId) 
			return EncryptionAlgorithms[ea].Modes[i];
	}

	return 0;
}


#ifndef GST_WINDOWS_BOOT

/**
 *
 *	\fn char *EAGetModeName (int ea, int mode, BOOL capitalLetters)
 *	\brief Returns the name of the mode of operation of the whole EA
 *	\param[in] BOOL capitalLetters
 *	\param[in] int mode
 *	\param[in] int ea
 *	\return char * string "XTS" switch (mode) case XTS:
 *			"XTS 64-bit version" switch (mode) case XTS8:
 *			"[unknown]"
 *
 */
char *EAGetModeName (int ea, int mode, BOOL capitalLetters)
{
	switch (mode)
	{
	case XTS:
		return "XTS";
	case XTS8:
		return "XTS 64-bit version";
	}
	return "[unknown]";
}

#endif // GST_WINDOWS_BOOT

/**
 *
 *	\fn int EAGetKeyScheduleSize (int ea)
 *	\brief Returns sum of key schedule sizes of all ciphers of the EA
 *	\param[in] int ea
 *	\return int size
 *
 */
int EAGetKeyScheduleSize (int ea)
{
	int i = EAGetFirstCipher(ea);
	int size = CipherGetKeyScheduleSize (i);

	while (i = EAGetNextCipher(ea, i))
	{
		size += CipherGetKeyScheduleSize (i);
	}

	return size;
}

/**
 *
 *	\fn int EAGetLargestKeyForMode (int mode)
 *	\brief Returns the largest key size needed by an EA for the specified mode of operation
 *	\param[in] int mode
 *	\returnint key
 *
 */
int EAGetLargestKeyForMode (int mode)
{
	int ea, key = 0;

	for (ea = EAGetFirst (); ea != 0; ea = EAGetNext (ea))
	{
		if (!EAIsModeSupported (ea, mode))
			continue;

		if (EAGetKeySize (ea) >= key)
			key = EAGetKeySize (ea);
	}
	return key;
}

/**
 *
 *	\fn int EAGetLargestKey ()
 *	\brief Returns the largest key needed by any EA for any mode
 *	\return int key
 *
 */
int EAGetLargestKey ()
{
	int ea, key = 0;

	for (ea = EAGetFirst (); ea != 0; ea = EAGetNext (ea))
	{
		if (EAGetKeySize (ea) >= key)
			key = EAGetKeySize (ea);
	}

	return key;
}

/**
 *
 *	\fn int EAGetCipherCount (int ea)
 *	\brief Returns number of ciphers in EA
 *	\param[in] int ea
 *	\return int i-1
 *
 */
int EAGetCipherCount (int ea)
{
	int i = 0;
	while (EncryptionAlgorithms[ea].Ciphers[i++]);

	return i - 1;
}


/**
 *
 *	\fn int EAGetFirstCipher (int ea)
 *	\brief Return the first EA
 *	\param[in] int ea
 *	\return int encryptionAlgorithm[].modes[].Ciphers[0]
 *
 */
int EAGetFirstCipher (int ea)
{
	return EncryptionAlgorithms[ea].Ciphers[0];
}


/**
 *
 *	\fn int EAGetLastCipher (int ea)
 *	\brief Get last EA cipher
 *	\param[in] int ea
 *	\return int EncryptionAlgorithms[ea].Ciphers[i - 2]
 *
 */
int EAGetLastCipher (int ea)
{
	int c, i = 0;
	while (c = EncryptionAlgorithms[ea].Ciphers[i++]);

	return EncryptionAlgorithms[ea].Ciphers[i - 2];
}


/**
 *
 *	\fn int EAGetNextCipher (int ea, int previousCipherId)
 *	\brief Return next cipher if exists
 *	\param[in] int previousCipherId
 *	\param[in] int ea
 *	\return int EncryptionAlgorithms[ea].Ciphers[i]
 *			0 if no next cipher
 *
 */
int EAGetNextCipher (int ea, int previousCipherId)
{
	int c, i = 0;
	while (c = EncryptionAlgorithms[ea].Ciphers[i++])
	{
		if (c == previousCipherId) 
			return EncryptionAlgorithms[ea].Ciphers[i];
	}

	return 0;
}


/**
 *
 *	\fn int EAGetPreviousCipher (int ea, int previousCipherId)
 *	\brief Return next cipher if exists
 *	\param[in] int previousCipherId
 *	\param[in] int ea
 *	\return int EncryptionAlgorithms[ea].Ciphers[i - 2]
 *			0 if no next cipher
 *
 */
int EAGetPreviousCipher (int ea, int previousCipherId)
{
	int c, i = 0;

	if (EncryptionAlgorithms[ea].Ciphers[i++] == previousCipherId)
		return 0;

	while (c = EncryptionAlgorithms[ea].Ciphers[i++])
	{
		if (c == previousCipherId) 
			return EncryptionAlgorithms[ea].Ciphers[i - 2];
	}

	return 0;
}


/**
 *
 *	\fn int EAIsFormatEnabled (int ea)
 *	\brief Return the state of format
 *	\param[in] int ea
 *	\return int EncryptionAlgorithms[ea].FormatEnabled
 *
 */
int EAIsFormatEnabled (int ea)
{
	return EncryptionAlgorithms[ea].FormatEnabled;
}

/**
 *
 *	\fn BOOL EAIsModeSupported (int ea, int testedMode)
 *	\brief Returns TRUE if the mode of operation is supported for the encryption algorithm
 *	\param[in] int testedMode
 *	\param[in] int ea
 *	\return BOOL TRUE if (mode == testedMode)
 *			FALSE otherwise
 *
 */
BOOL EAIsModeSupported (int ea, int testedMode)
{
	int mode;

	for (mode = EAGetFirstMode (ea); mode != 0; mode = EAGetNextMode (ea, mode))
	{
		if (mode == testedMode)
			return TRUE;
	}
	return FALSE;
}


/**
 *
 *	\fn Hash *HashGet (int id)
 *	\brief Returns hash by id or 0 if no hash
 *	\param[in] int id
 *	\return Hash &Hashes[i] if (Hashes[i].Id == id)
 *			0 otherwise
 *
 */
Hash *HashGet (int id)
{
	int i;
	for (i = 0; Hashes[i].Id != 0; i++)
		if (Hashes[i].Id == id)
			return &Hashes[i];

	return 0;
}


/**
 *
 *	\fn int HashGetIdByName (char *name)
 *	\brief Returns hash by name or 0 if no hash
 *	\param[in] char * name
 *	\return int Hashes[i].Id if (strcmp (Hashes[i].Name, name) == 0)
 *			0 otherwise
 *
 */
int HashGetIdByName (char *name)
{
	int i;
	for (i = 0; Hashes[i].Id != 0; i++)
		if (strcmp (Hashes[i].Name, name) == 0)
			return Hashes[i].Id;

	return 0;
}


/**
 *
 *	\fn char *HashGetName (int hashId)
 *	\brief Returns hash by name or 0 if no hash
 *	\param[in] int hashId
 *	\return char * HashGet (hashId)->Name
 *
 */
char *HashGetName (int hashId)
{
	return HashGet (hashId) -> Name;
}


/**
 *
 *	\fn BOOL HashIsDeprecated (int hashId)
 *	\brief Returns true if deprecated, false else.
 *	\param[in] int hashId
 *	\return BOOL HashGet (hashId)->Deprecated
 *
 */
BOOL HashIsDeprecated (int hashId)
{
	return HashGet (hashId) -> Deprecated;
}


#endif // GST_WINDOWS_BOOT_SINGLE_CIPHER_MODE


#ifdef GST_WINDOWS_BOOT

static byte CryptoInfoBufferInUse = 0;
CRYPTO_INFO CryptoInfoBuffer;

#endif

/**
 *
 *	\fn PCRYPTO_INFO crypto_open ()
 *	\brief Do the crypto
 *	\return PCRYPTO_INFO cryptoInfo
 *			NULL if (cryptoInfo == NULL)
 *			GST_THROW_FATAL_EXCEPTION if (CryptoInfoBufferInUse) #else DEVICE_DRIVER
 *			&CryptoInfoBuffer  #else DEVICE_DRIVER
 *
 */
PCRYPTO_INFO crypto_open ()
{
#ifndef GST_WINDOWS_BOOT

	/* Do the crt allocation */
	PCRYPTO_INFO cryptoInfo = (PCRYPTO_INFO) GSTalloc (sizeof (CRYPTO_INFO));
	if (cryptoInfo == NULL)
		return NULL;

	memset (cryptoInfo, 0, sizeof (CRYPTO_INFO));

#ifndef DEVICE_DRIVER
	VirtualLock (cryptoInfo, sizeof (CRYPTO_INFO));
#endif

	cryptoInfo->ea = -1;
	return cryptoInfo;

#else // GST_WINDOWS_BOOT

#if 0
	if (CryptoInfoBufferInUse)
		GST_THROW_FATAL_EXCEPTION;
#endif
	CryptoInfoBufferInUse = 1;
	return &CryptoInfoBuffer;

#endif // GST_WINDOWS_BOOT
}

/**
 *
 *	\fn void crypto_loadkey (PKEY_INFO keyInfo, char *lpszUserKey, int nUserKeyLen)
 *	\brief Load the key
 *	\param[in out] int nUserKeyLen
 *	\param[in] char * lpszUserKey
 *	\param[in] PKEY_INFO keyInfo
 *	\return void
 *
 */
void crypto_loadkey (PKEY_INFO keyInfo, char *lpszUserKey, int nUserKeyLen)
{
	keyInfo->keyLength = nUserKeyLen;
	burn (keyInfo->userKey, sizeof (keyInfo->userKey));
	memcpy (keyInfo->userKey, lpszUserKey, nUserKeyLen);
}

/**
 *
 *	\fn void crypto_close (PCRYPTO_INFO cryptoInfo)
 *	\brief Close the crytpo
 *	\param[in] PCRYPTO_INFO cryptoInfo
 *	\return void
 *
 */
void crypto_close (PCRYPTO_INFO cryptoInfo)
{
#ifndef GST_WINDOWS_BOOT

	if (cryptoInfo != NULL)
	{
		burn (cryptoInfo, sizeof (CRYPTO_INFO));
#ifndef DEVICE_DRIVER
		VirtualUnlock (cryptoInfo, sizeof (CRYPTO_INFO));
#endif
		GSTfree (cryptoInfo);
	}

#else // GST_WINDOWS_BOOT

	burn (&CryptoInfoBuffer, sizeof (CryptoInfoBuffer));
	CryptoInfoBufferInUse = FALSE;

#endif // GST_WINDOWS_BOOT
}


#ifndef GST_WINDOWS_BOOT_SINGLE_CIPHER_MODE


#ifndef GST_NO_COMPILER_INT64

/**
 *
 *	\fn void Xor128 (unsigned __int64 *a, unsigned __int64 *b)
 *	\brief Do a xor 128bits
 *	\param[in] unsigned __int64 * b
 *	\param[in out] unsigned __int64 * a
 *	\return void
 *
 */
void Xor128 (unsigned __int64 *a, unsigned __int64 *b)
{
	*a++ ^= *b++;
	*a ^= *b;
}

/**
 *
 *	\fn void Xor64 (unsigned __int64 *a, unsigned __int64 *b)
 *	\brief Do a xor 64bits
 *	\param[in out] unsigned __int64 * b
 *	\param[in out] unsigned __int64 * a
 *	\return void
 *
 */
void Xor64 (unsigned __int64 *a, unsigned __int64 *b)
{
	*a ^= *b;
}
#endif	// #ifndef GST_NO_COMPILER_INT64

/**
 *
 *	\fn void EncryptBuffer (unsigned __int8 *buf, GST_LARGEST_COMPILER_UINT len, PCRYPTO_INFO cryptoInfo)
 *	\brief	buf:  data to be encrypted; the start of the buffer is assumed to be aligned with the start of a data unit.
 *			len:  number of bytes to encrypt; must be divisible by the block size (for cascaded ciphers, divisible by the largest block size used within the cascade)
 *	\param[in] PCRYPTO_INFO cryptoInfo
 *	\param[in] GST_LARGEST_COMPILER_UINT len
 *	\param[in out] unsigned __int8 * buf
 *	\return void
 *
 */
void EncryptBuffer (unsigned __int8 *buf, GST_LARGEST_COMPILER_UINT len, PCRYPTO_INFO cryptoInfo)
{
	switch (cryptoInfo->mode)
	{
	case XTS:
		{
			unsigned __int8 *ks = cryptoInfo->ks;
			unsigned __int8 *ks2 = cryptoInfo->ks2;
			UINT64_STRUCT dataUnitNo;
			int cipher;

			// When encrypting/decrypting a buffer (typically a volume header) the sequential number
			// of the first XTS data unit in the buffer is always 0 and the start of the buffer is
			// always assumed to be aligned with the start of a data unit.
			dataUnitNo.LowPart = 0;
			dataUnitNo.HighPart = 0;

			for (cipher = EAGetFirstCipher (cryptoInfo->ea);
				cipher != 0;
				cipher = EAGetNextCipher (cryptoInfo->ea, cipher))
			{
				EncryptBufferXTS (buf, len, &dataUnitNo, 0, ks, ks2, cipher);

				ks += CipherGetKeyScheduleSize (cipher);
				ks2 += CipherGetKeyScheduleSize (cipher);
			}
		}
		break;
	case XTS8:
		{
				 unsigned __int8 *ks = cryptoInfo->ks;
				 unsigned __int8 *ks2 = cryptoInfo->ks2;
				 UINT64_STRUCT dataUnitNo;
				 int cipher;

				 // When encrypting/decrypting a buffer (typically a volume header) the sequential number
				 // of the first XTS data unit in the buffer is always 0 and the start of the buffer is
				 // always assumed to be aligned with the start of a data unit.
				 dataUnitNo.LowPart = 0;
				 dataUnitNo.HighPart = 0;

				 for (cipher = EAGetFirstCipher(cryptoInfo->ea);
					 cipher != 0;
					 cipher = EAGetNextCipher(cryptoInfo->ea, cipher))
				 {
					 EncryptBufferXTS8Byte(buf, len, &dataUnitNo, 0, ks, ks2, cipher);

					 ks += CipherGetKeyScheduleSize(cipher);
					 ks2 += CipherGetKeyScheduleSize(cipher);
				 }
		}
		break;

	default:		
		// Unknown/wrong ID
		GST_THROW_FATAL_EXCEPTION;
	}
}

/**
 *
 *	\fn void void EncryptDataUnits (unsigned __int8 *buf, const UINT64_STRUCT *structUnitNo, uint32 nbrUnits, PCRYPTO_INFO ci)
 *	\brief	buf:			data to be encrypted
 *			unitNo:		sequential number of the data unit with which the buffer starts
 *			nbrUnits:	number of data units in the buffer
 *	\param[in] PCRYPTO_INFO ci
 *	\param[in] uint32 nbrUnits
 *	\param[in] const UINT64_STRUCT * structUnitNo
 *	\param[in out] unsigned __int8 * buf
 *	\return void
 *
 */
void EncryptDataUnits (unsigned __int8 *buf, const UINT64_STRUCT *structUnitNo, uint32 nbrUnits, PCRYPTO_INFO ci)
#ifndef GST_WINDOWS_BOOT
{
	EncryptionThreadPoolDoWork (EncryptDataUnitsWork, buf, structUnitNo, nbrUnits, ci);
}

/**
 *
 *	\fn void EncryptDataUnitsCurrentThread (unsigned __int8 *buf, const UINT64_STRUCT *structUnitNo, GST_LARGEST_COMPILER_UINT nbrUnits, PCRYPTO_INFO ci)
 *	\brief Encryt data of current thread
 *	\param[in] PCRYPTO_INFO ci
 *	\param[in] GST_LARGEST_COMPILER_UINT nbrUnits
 *	\param[in] UINT64_STRUCT * structUnitNo
 *	\param[in out] unsigned __int8 * buf
 *	\return void
 *
 */
void EncryptDataUnitsCurrentThread (unsigned __int8 *buf, const UINT64_STRUCT *structUnitNo, GST_LARGEST_COMPILER_UINT nbrUnits, PCRYPTO_INFO ci)
#endif // !GST_WINDOWS_BOOT
{
	int ea = ci->ea;
	unsigned __int8 *ks = ci->ks;
	unsigned __int8 *ks2 = ci->ks2;
	int cipher;

	switch (ci->mode)
	{
	case XTS:
		for (cipher = EAGetFirstCipher (ea); cipher != 0; cipher = EAGetNextCipher (ea, cipher))
		{
			EncryptBufferXTS (buf,
				nbrUnits * ENCRYPTION_DATA_UNIT_SIZE,
				structUnitNo,
				0,
				ks,
				ks2,
				cipher);

			ks += CipherGetKeyScheduleSize (cipher);
			ks2 += CipherGetKeyScheduleSize (cipher);
		}
		break;
	case XTS8:
		for (cipher = EAGetFirstCipher(ea); cipher != 0; cipher = EAGetNextCipher(ea, cipher))
		{
			EncryptBufferXTS8Byte(buf,
				nbrUnits * ENCRYPTION_DATA_UNIT_SIZE,
				structUnitNo,
				0,
				ks,
				ks2,
				cipher);

			ks += CipherGetKeyScheduleSize(cipher);
			ks2 += CipherGetKeyScheduleSize(cipher);
		}
		break;
	default:		
		// Unknown/wrong ID
		GST_THROW_FATAL_EXCEPTION;
	}
}

/**
 *
 *	\fn void DecryptBuffer (unsigned __int8 *buf, GST_LARGEST_COMPILER_UINT len, PCRYPTO_INFO cryptoInfo)
 *	\brief	buf:  data to be decrypted; the start of the buffer is assumed to be aligned with the start of a data unit.
 *			len:  number of bytes to decrypt; must be divisible by the block size (for cascaded ciphers, divisible by the largest block size used within the cascade)
 *	\param[in] PCRYPTO_INFO cryptoInfo
 *	\param[in] GST_LARGEST_COMPILER_UINT len
 *	\param[in] unsigned __int8 * buf
 *	\return void
 *
 */
void DecryptBuffer (unsigned __int8 *buf, GST_LARGEST_COMPILER_UINT len, PCRYPTO_INFO cryptoInfo)
{
	switch (cryptoInfo->mode)
	{
	case XTS:
		{
			unsigned __int8 *ks = cryptoInfo->ks + EAGetKeyScheduleSize (cryptoInfo->ea);
			unsigned __int8 *ks2 = cryptoInfo->ks2 + EAGetKeyScheduleSize (cryptoInfo->ea);
			UINT64_STRUCT dataUnitNo;
			int cipher;

			// When encrypting/decrypting a buffer (typically a volume header) the sequential number
			// of the first XTS data unit in the buffer is always 0 and the start of the buffer is
			// always assumed to be aligned with the start of the data unit 0.
			dataUnitNo.LowPart = 0;
			dataUnitNo.HighPart = 0;

			for (cipher = EAGetLastCipher (cryptoInfo->ea);
				cipher != 0;
				cipher = EAGetPreviousCipher (cryptoInfo->ea, cipher))
			{
				ks -= CipherGetKeyScheduleSize (cipher);
				ks2 -= CipherGetKeyScheduleSize (cipher);

				DecryptBufferXTS (buf, len, &dataUnitNo, 0, ks, ks2, cipher);
			}
		}
		break;
	case XTS8:
	{
				 unsigned __int8 *ks = cryptoInfo->ks + EAGetKeyScheduleSize(cryptoInfo->ea);
				 unsigned __int8 *ks2 = cryptoInfo->ks2 + EAGetKeyScheduleSize(cryptoInfo->ea);
				 UINT64_STRUCT dataUnitNo;
				 int cipher;

				 // When encrypting/decrypting a buffer (typically a volume header) the sequential number
				 // of the first XTS data unit in the buffer is always 0 and the start of the buffer is
				 // always assumed to be aligned with the start of the data unit 0.
				 dataUnitNo.LowPart = 0;
				 dataUnitNo.HighPart = 0;

				 for (cipher = EAGetLastCipher(cryptoInfo->ea);
					 cipher != 0;
					 cipher = EAGetPreviousCipher(cryptoInfo->ea, cipher))
				 {
					 ks -= CipherGetKeyScheduleSize(cipher);
					 ks2 -= CipherGetKeyScheduleSize(cipher);

					 DecryptBufferXTS8Byte(buf, len, &dataUnitNo, 0, ks, ks2, cipher);
				 }
	}
		break;
	default:		
		// Unknown/wrong ID
		GST_THROW_FATAL_EXCEPTION;
	}
}

/**
 *
 *	\fn void DecryptDataUnits (unsigned __int8 *buf, const UINT64_STRUCT *structUnitNo, uint32 nbrUnits, PCRYPTO_INFO ci)
 *	\brief	buf:		data to be decrypted
 *			unitNo:		sequential number of the data unit with which the buffer starts
 *			nbrUnits:	number of data units in the buffer
 *	\param[in] PCRYPTO_INFO ci
 *	\param[in] uint32 nbrUnits
 *	\param[in] UINT64_STRUCT * structUnitNo
 *	\param[in out] unsigned __int8 * buf
 *	\return void
 *
 */
void DecryptDataUnits (unsigned __int8 *buf, const UINT64_STRUCT *structUnitNo, uint32 nbrUnits, PCRYPTO_INFO ci)
#ifndef GST_WINDOWS_BOOT
{
	EncryptionThreadPoolDoWork (DecryptDataUnitsWork, buf, structUnitNo, nbrUnits, ci);
}

/**
 *
 *	\fn void DecryptDataUnitsCurrentThread (unsigned __int8 *buf, const UINT64_STRUCT *structUnitNo, GST_LARGEST_COMPILER_UINT nbrUnits, PCRYPTO_INFO ci)
 *	\brief	buf:		data to be decrypted
 *			unitNo:		sequential number of the data unit with which the buffer starts
 *			nbrUnits:	number of data units in the buffer
 *	\param[in] PCRYPTO_INFO ci
 *	\param[in] uint32 nbrUnits
 *	\param[in] UINT64_STRUCT* structUnitNo
 *	\param[in out] unsigned __int8* buf
 *	\return void
 *
 */
void DecryptDataUnitsCurrentThread (unsigned __int8 *buf, const UINT64_STRUCT *structUnitNo, GST_LARGEST_COMPILER_UINT nbrUnits, PCRYPTO_INFO ci)
#endif // !GST_WINDOWS_BOOT
{
	int ea = ci->ea;
	unsigned __int8 *ks = ci->ks;
	unsigned __int8 *ks2 = ci->ks2;
	int cipher;

	switch (ci->mode)
	{
	case XTS:
		ks += EAGetKeyScheduleSize (ea);
		ks2 += EAGetKeyScheduleSize (ea);

		for (cipher = EAGetLastCipher (ea); cipher != 0; cipher = EAGetPreviousCipher (ea, cipher))
		{
			ks -= CipherGetKeyScheduleSize (cipher);
			ks2 -= CipherGetKeyScheduleSize (cipher);

			DecryptBufferXTS (buf,
				nbrUnits * ENCRYPTION_DATA_UNIT_SIZE,
				structUnitNo,
				0,
				ks,
				ks2,
				cipher);
		}
		break;
	case XTS8:
		ks += EAGetKeyScheduleSize(ea);
		ks2 += EAGetKeyScheduleSize(ea);

		for (cipher = EAGetLastCipher(ea); cipher != 0; cipher = EAGetPreviousCipher(ea, cipher))
		{
			ks -= CipherGetKeyScheduleSize(cipher);
			ks2 -= CipherGetKeyScheduleSize(cipher);

			DecryptBufferXTS8Byte(buf,
				nbrUnits * ENCRYPTION_DATA_UNIT_SIZE,
				structUnitNo,
				0,
				ks,
				ks2,
				cipher);
		}
		break;
	default:		
		// Unknown/wrong ID
		GST_THROW_FATAL_EXCEPTION;
	}
}

/**
 *
 *	\fn int GetMaxPkcs5OutSize (void)
 *	\brief Returns the maximum number of bytes necessary to be generated by the PBKDF2 (PKCS #5)
 *	\return int size
 *
 */
int GetMaxPkcs5OutSize (void)
{
	int size = 32;

	size = max (size, EAGetLargestKeyForMode (XTS) * 2);	// Sizes of primary + secondary keys
	size = max (size, EAGetLargestKeyForMode (XTS8) * 2);

	return size;
}


#else // GST_WINDOWS_BOOT_SINGLE_CIPHER_MODE


#if !defined (GST_WINDOWS_BOOT_AES) && !defined (GST_WINDOWS_BOOT_SERPENT) && !defined (GST_WINDOWS_BOOT_TWOFISH)
#error No cipher defined
#endif

/**
 *
 *	\fn void EncipherBlock(int cipher, void *data, void *ks)
 *	\brief Returns the maximum number of bytes necessary to be generated by the PBKDF2 (PKCS #5)
 *	\param[in] void * ks
 *	\param[in] void * data
 *	\param[in] int cipher
 *	\return void
 *
 */
void EncipherBlock(int cipher, void *data, void *ks)
{
#ifdef GST_WINDOWS_BOOT_AES
	if (IsAesHwCpuSupported())
		aes_hw_cpu_encrypt ((byte *) ks, data);
	else
		aes_encrypt (data, data, ks); 
#elif defined (GST_WINDOWS_BOOT_SERPENT)
	serpent_encrypt (data, data, ks);
#elif defined (GST_WINDOWS_BOOT_TWOFISH)
	twofish_encrypt (ks, data, data);
#endif
}

/**
 *
 *	\fn void DecipherBlock(int cipher, void *data, void *ks)
 *	\brief Decipher a block
 *	\param[in] void * ks
 *	\param[in] void * data
 *	\param[in] int cipher
 *	\return void
 *
 */
void DecipherBlock(int cipher, void *data, void *ks)
{
#ifdef GST_WINDOWS_BOOT_AES
	if (IsAesHwCpuSupported())
		aes_hw_cpu_decrypt ((byte *) ks + sizeof (aes_encrypt_ctx) + 14 * 16, data);
	else
		aes_decrypt (data, data, (aes_decrypt_ctx *) ((byte *) ks + sizeof(aes_encrypt_ctx))); 
#elif defined (GST_WINDOWS_BOOT_SERPENT)
	serpent_decrypt (data, data, ks);
#elif defined (GST_WINDOWS_BOOT_TWOFISH)
	twofish_decrypt (ks, data, data);
#endif
}

/**
 *
 *	\fn int EAGetFirst ()
 *	\brief Get first EA (id)
 *	\return int 1
 *
 */
int EAGetFirst ()
{
	return 1;
}

/**
 *
 *	\fn int EAGetNext ()
 *	\brief Get next EA (id)
 *	\return int 0
 *
 */
int EAGetNext (int previousEA)
{
	return 0;
}

/**
 *
 *	\fn int EAInit (int ea, unsigned char *key, unsigned __int8 *ks)
 *	\brief Initialize the EA
 *	\param[in] unsigned __int8 * ks
 *	\param[in] unsigned char * key
 *	\param[in] int ea
 *	\return int ERR_SUCCESS if success
 *			ERR_CIPHER_INIT_FAILURE if (aes_encrypt_key256 (key, (aes_encrypt_ctx *) ks) != EXIT_SUCCESS)
 *			ERR_CIPHER_INIT_FAILURE if (aes_decrypt_key256 (key, (aes_decrypt_ctx *) (ks + sizeof (aes_encrypt_ctx))) != EXIT_SUCCESS)
 *
 */
int EAInit (int ea, unsigned char *key, unsigned __int8 *ks)
{
#ifdef GST_WINDOWS_BOOT_AES

	aes_init();

	if (aes_encrypt_key256 (key, (aes_encrypt_ctx *) ks) != EXIT_SUCCESS)
		return ERR_CIPHER_INIT_FAILURE;
	if (aes_decrypt_key256 (key, (aes_decrypt_ctx *) (ks + sizeof (aes_encrypt_ctx))) != EXIT_SUCCESS)
		return ERR_CIPHER_INIT_FAILURE;

#elif defined (GST_WINDOWS_BOOT_SERPENT)
	serpent_set_key (key, 32 * 8, ks);
#elif defined (GST_WINDOWS_BOOT_TWOFISH)
	twofish_set_key ((TwofishInstance *)ks, (const u4byte *)key, 32 * 8);
#endif
	return ERR_SUCCESS;
}

/**
 *
 *	\fn int EAGetKeySize (int ea)
 *	\brief Get size of ea key
 *	\param[in] int ea
 *	\return int 32
 *
 */
int EAGetKeySize (int ea)
{
	return 32;
}

/**
 *
 *	\fn int EAGetFirstCipher (int ea)
 *	\brief Get first cipher
 *	\param[in] int ea
 *	\return int 1
 *
 */
int EAGetFirstCipher (int ea)
{
	return 1;
}

/**
 *
 *	\fn void EncryptBuffer (unsigned __int8 *buf, GST_LARGEST_COMPILER_UINT len, PCRYPTO_INFO cryptoInfo)
 *	\brief Encrypt the buffer
 *	\param[in] PCRYPTO_INFO cryptoInfo
 *	\param[in] GST_LARGEST_COMPILER_UINT len
 *	\param[in] unsigned __int8 * buf
 *	\return void
 *
 */
void EncryptBuffer (unsigned __int8 *buf, GST_LARGEST_COMPILER_UINT len, PCRYPTO_INFO cryptoInfo)
{
	UINT64_STRUCT dataUnitNo;
	dataUnitNo.LowPart = 0; dataUnitNo.HighPart = 0;
	switch(cryptoInfo->mode)
	{
	case XTS:
		EncryptBufferXTS (buf, len, ,&dataUnitNo, 0, cryptoInfo->ks, cryptoInfo->ks2, 1);
		break;
	case XTS8:
		EncryptBufferXTS8Byte (buf, len, ,&dataUnitNo, 0, cryptoInfo->ks, cryptoInfo->ks2, 1);
		break;
	}
}

/**
 *
 *	\fn void EncryptDataUnits (unsigned __int8 *buf, const UINT64_STRUCT *structUnitNo, GST_LARGEST_COMPILER_UINT nbrUnits, PCRYPTO_INFO ci)
 *	\brief Encrypt data units
 *	\param[in out] PCRYPTO_INFO ci
 *	\param[in] GST_LARGEST_COMPILER_UINT nbrUnits
 *	\param[in] const UINT64_STRUCT
 *	\param[in] unsigned __int8
 *	\return void
 *
 */
void EncryptDataUnits (unsigned __int8 *buf, const UINT64_STRUCT *structUnitNo, GST_LARGEST_COMPILER_UINT nbrUnits, PCRYPTO_INFO ci)
{
	switch(ci->mode)
	{
	case XTS:
		EncryptBufferXTS (buf, nbrUnits * ENCRYPTION_DATA_UNIT_SIZE, structUnitNo, 0, ci->ks, ci->ks2, 1);
		break;
	case XTS8:
		EncryptBufferXTS8Byte (buf, nbrUnits * ENCRYPTION_DATA_UNIT_SIZE, structUnitNo, 0, ci->ks, ci->ks2, 1);
		break;
	}
}

/**
 *
 *	\fn void DecryptBuffer (unsigned __int8 *buf, GST_LARGEST_COMPILER_UINT len, PCRYPTO_INFO cryptoInfo)
 *	\brief Decrypt the buffer
 *	\param[in] PCRYPTO_INFO cryptoInfo
 *	\param[in] GST_LARGEST_COMPILER_UINT len
 *	\param[in] unsigned __int8 * buf
 *	\return void
 *
 */
void DecryptBuffer (unsigned __int8 *buf, GST_LARGEST_COMPILER_UINT len, PCRYPTO_INFO cryptoInfo)
{
	UINT64_STRUCT dataUnitNo;
	dataUnitNo.LowPart = 0; dataUnitNo.HighPart = 0;
	switch(cryptoInfo->mode)
	{
	case XTS:
		DecryptBufferXTS (buf, len, &dataUnitNo, 0, cryptoInfo->ks, cryptoInfo->ks2, 1);
		break;
	case XTS8:
		DecryptBufferXTS8Byte (buf, len, &dataUnitNo, 0, cryptoInfo->ks, cryptoInfo->ks2, 1);
		break;
	}
}

/**
 *
 *	\fn void DecryptDataUnits (unsigned __int8 *buf, const UINT64_STRUCT *structUnitNo, GST_LARGEST_COMPILER_UINT nbrUnits, PCRYPTO_INFO ci)
 *	\brief Decrypt data units
 *	\param[in] PCRYPTO_INFO ci
 *	\param[in] GST_LARGEST_COMPILER_UINT nbrUnits
 *	\param[in] const UINT64_STRUCT * structUnitNo
 *	\param[in] unsigned __int8 * buf
 *	\return void
 *
 */
void DecryptDataUnits (unsigned __int8 *buf, const UINT64_STRUCT *structUnitNo, GST_LARGEST_COMPILER_UINT nbrUnits, PCRYPTO_INFO ci)
{
	switch(ci->mode)
	{
	case XTS:
		DecryptBufferXTS (buf, nbrUnits * ENCRYPTION_DATA_UNIT_SIZE, structUnitNo, 0, ci->ks, ci->ks2, 1);
		break;
	case XTS8:
		DecryptBufferXTS8Byte (buf, nbrUnits * ENCRYPTION_DATA_UNIT_SIZE, structUnitNo, 0, ci->ks, ci->ks2, 1);
		break;
	}
	
}

#endif // GST_WINDOWS_BOOT_SINGLE_CIPHER_MODE


#if !defined (GST_WINDOWS_BOOT) || defined (GST_WINDOWS_BOOT_AES)

static BOOL HwEncryptionDisabled = FALSE;

/**
 *
 *	\fn BOOL IsAesHwCpuSupported ()
 *	\brief Is Aes Hardware cpu supported
 *	\return BOOL FALSE if cpu is supported, TRUE else
 *
 */
BOOL IsAesHwCpuSupported ()
{
	static BOOL state = FALSE;
	static BOOL stateValid = FALSE;

	if (!stateValid)
	{
		state = FALSE;//state = is_aes_hw_cpu_supported() ? TRUE : FALSE;
		stateValid = TRUE;
	}

	return FALSE;//state && !HwEncryptionDisabled;
}

/**
 *
 *	\fn void EnableHwEncryption (BOOL enable)
 *	\brief Deprecated useless
 *	\param[in] BOOL enable
 *	\return void
 *
 */
void EnableHwEncryption (BOOL enable)
{
	//Deprecated
}

/**
 *
 *	\fn BOOL IsHwEncryptionEnabled ()
 *	\brief Is Hardware encryption enabled
 *	\return BOOL !HwEncryptionDisabled
 *
 */
BOOL IsHwEncryptionEnabled ()
{
	return !HwEncryptionDisabled;
}

#endif // !GST_WINDOWS_BOOT
