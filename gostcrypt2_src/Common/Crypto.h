/*
 Legal Notice: Some portions of the source code contained in this file were
 derived from the source code of Encryption for the Masses 2.02a, which is
 Copyright (c) 1998-2000 Paul Le Roux and which is governed by the 'License
 Agreement for Encryption for the Masses'. Modifications and additions to
 the original source code (contained in this file) and all other portions
 of this file are Copyright (c) 2003-2010 TrueCrypt Developers Association
 and are governed by the TrueCrypt License 3.0 the full text of which is
 contained in the file License.txt included in TrueCrypt binary and source
 code distribution packages. */


/*
 Legal Notice: Some portions of the source code contained in this file were
 derived from the source code of Encryption for the Masses 2.02a, which is
 Copyright (c) 1998-2000 Paul Le Roux and which is governed by the 'License
 Agreement for Encryption for the Masses'. */

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

#ifndef CRYPTO_H
#define CRYPTO_H

#include "Gstdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

// Encryption data unit size, which may differ from the sector size and must always be 512
#define ENCRYPTION_DATA_UNIT_SIZE	512

// Size of the salt (in bytes)
#define PKCS5_SALT_SIZE				64

// Size of the volume header area containing concatenated master key(s) and secondary key(s) (XTS mode)
#define MASTER_KEYDATA_SIZE			256

// The first PRF to try when mounting
#define FIRST_PRF_ID		1	

// Hash algorithms (pseudorandom functions). 
enum
{
	STRIBOG = FIRST_PRF_ID,
	WHIRLPOOL,
	GOSTHASH,
	HASH_ENUM_END_ID
};

// The last PRF to try when mounting and also the number of implemented PRFs
#define LAST_PRF_ID			(HASH_ENUM_END_ID - 1)	

#define WHIRLPOOL_BLOCKSIZE		64
#define WHIRLPOOL_DIGESTSIZE	64

#define STRIBOG_BLOCKSIZE		64
#define STRIBOG_DIGESTSIZE		64

#define GOSTHASH_BLOCKSIZE		32
#define GOSTHASH_DIGESTSIZE		32

#define MAX_DIGESTSIZE			WHIRLPOOL_DIGESTSIZE

#define DEFAULT_HASH_ALGORITHM			STRIBOG
#define DEFAULT_HASH_ALGORITHM_BOOT		STRIBOG

// The mode of operation used for newly created volumes and first to try when mounting
#define FIRST_MODE_OF_OPERATION_ID		1

// Modes of operation
enum
{
	/* If you add/remove a mode, update the following: GetMaxPkcs5OutSize(), EAInitMode() */

	XTS = FIRST_MODE_OF_OPERATION_ID,
	MODE_ENUM_END_ID
};


// The last mode of operation to try when mounting and also the number of implemented modes
#define LAST_MODE_OF_OPERATION		(MODE_ENUM_END_ID - 1)

// Ciphertext/plaintext block size for XTS mode (in bytes)
#define BYTES_PER_XTS_BLOCK			16
#define BYTES_PER_XTS_BLOCK_SMALL	8

// Number of ciphertext/plaintext blocks per XTS data unit
#define BLOCKS_PER_XTS_DATA_UNIT	(ENCRYPTION_DATA_UNIT_SIZE / BYTES_PER_XTS_BLOCK)
#define BLOCKS_PER_XTS_DATA_UNIT_SMALL (ENCRYPTION_DATA_UNIT_SIZE / BYTES_PER_XTS_BLOCK_SMALL)


// Cipher IDs
enum
{
	NONE = 0,
	GOST,
	GRASSHOPPER
};


typedef struct
{
	int Ciphers[4];			// Null terminated array of ciphers used by encryption algorithm
	int Modes[LAST_MODE_OF_OPERATION + 1];			// Null terminated array of modes of operation
	int FormatEnabled;
} EncryptionAlgorithm;


#ifdef DEBUG
#	define PRAND_DISK_WIPE_PASSES	3
#else
#	define PRAND_DISK_WIPE_PASSES	256
#endif

#include "GostCipher.h"
#include "GrasshopperCipher.h"

#include "Whirlpool.h"
#include "Stribog.h"
#include "GostHash.h"

#include "GfMul.h"
#include "Password.h"


#define GOST_KS	(sizeof(gost_kds))
#define GRASSHOPPER_KS (sizeof(grasshopper_kds))


#define MAX_EXPANDED_KEY	(GOST_KS)



typedef struct keyInfo_t
{
	int noIterations;					/* Number of times to iterate (PKCS-5) */
	int keyLength;						/* Length of the key */
	__int8 userKey[MAX_PASSWORD];		/* Password (to which keyfiles may have been applied). WITHOUT +1 for the null terminator. */
	__int8 salt[PKCS5_SALT_SIZE];		/* PKCS-5 salt */
	__int8 master_keydata[MASTER_KEYDATA_SIZE];		/* Concatenated master primary and secondary key(s) (XTS mode). */
} KEY_INFO, *PKEY_INFO;

typedef struct CRYPTO_INFO_t
{
	int ea;									/* Encryption algorithm ID */
	int mode;								/* Mode of operation (e.g., XTS) */
	unsigned __int8 ks[MAX_EXPANDED_KEY];	/* Primary key schedule (if it is a cascade, it conatins multiple concatenated keys) */
	unsigned __int8 ks2[MAX_EXPANDED_KEY];	/* Secondary key schedule (if cascade, multiple concatenated) for XTS mode. */

	BOOL hiddenVolume;						// Indicates whether the volume is mounted/mountable as hidden volume

	quint16 HeaderVersion;

	GfCtx gf_ctx; 

	unsigned __int8 master_keydata[MASTER_KEYDATA_SIZE];	/* This holds the volume header area containing concatenated master key(s) and secondary key(s) (XTS mode). */
	unsigned __int8 k2[MASTER_KEYDATA_SIZE];				/* For XTS, this contains the secondary key (if cascade, multiple concatenated). */
	unsigned __int8 salt[PKCS5_SALT_SIZE];
	int noIterations;
	int pkcs5;

	quint64 volume_creation_time;	// Legacy
	quint64 header_creation_time;	// Legacy

	BOOL bProtectHiddenVolume;			// Indicates whether the volume contains a hidden volume to be protected against overwriting
	BOOL bHiddenVolProtectionAction;		// TRUE if a write operation has been denied by the driver in order to prevent the hidden volume from being overwritten (set to FALSE upon volume mount).
	
	quint64 volDataAreaOffset;		// Absolute position, in bytes, of the first data sector of the volume.

	quint64 hiddenVolumeSize;		// Size of the hidden volume excluding the header (in bytes). Set to 0 for standard volumes.
	quint64 hiddenVolumeOffset;	// Absolute position, in bytes, of the first hidden volume data sector within the host volume (provided that there is a hidden volume within). This must be set for all hidden volumes; in case of a normal volume, this variable is only used when protecting a hidden volume within it.
	quint64 hiddenVolumeProtectedSize;

	BOOL bPartitionInInactiveSysEncScope;	// If TRUE, the volume is a partition located on an encrypted system drive and mounted without pre-boot authentication.

	UINT64_STRUCT FirstDataUnitNo;			// First data unit number of the volume. This is 0 for file-hosted and non-system partition-hosted volumes. For partitions within key scope of system encryption this reflects real physical offset within the device (this is used e.g. when such a partition is mounted as a regular volume without pre-boot authentication).

	quint16 RequiredProgramVersion;
	BOOL LegacyVolume;

	quint32 SectorSize;


	UINT64_STRUCT VolumeSize;

	UINT64_STRUCT EncryptedAreaStart;
	UINT64_STRUCT EncryptedAreaLength;

	quint32 HeaderFlags;

} CRYPTO_INFO, *PCRYPTO_INFO;

PCRYPTO_INFO crypto_open (void);
void crypto_loadkey (PKEY_INFO keyInfo, char *lpszUserKey, int nUserKeyLen);
void crypto_close (PCRYPTO_INFO cryptoInfo);

int CipherGetBlockSize (int cipher);
int CipherGetKeySize (int cipher);
int CipherGetKeyScheduleSize (int cipher);
BOOL CipherSupportsIntraDataUnitParallelization (int cipher);
char * CipherGetName (int cipher);

int CipherInit (int cipher, unsigned char *key, unsigned char *ks);
int EAInit (int ea, unsigned char *key, unsigned char *ks);
BOOL EAInitMode (PCRYPTO_INFO ci);
void EncipherBlock(int cipher, void *data, void *ks);
void DecipherBlock(int cipher, void *data, void *ks);
void EncipherBlocks (int cipher, void *dataPtr, void *ks, size_t blockCount);
void DecipherBlocks (int cipher, void *dataPtr, void *ks, size_t blockCount);

int EAGetFirst ();
int EAGetCount (void);
int EAGetNext (int previousEA);
char * EAGetName (char *buf, int ea);
int EAGetByName (char *name);
int EAGetKeySize (int ea);
int EAGetFirstMode (int ea);
int EAGetNextMode (int ea, int previousModeId);
char * EAGetModeName (int ea, int mode, BOOL capitalLetters);
int EAGetKeyScheduleSize (int ea);
int EAGetLargestKey ();
int EAGetLargestKeyForMode (int mode);

int EAGetCipherCount (int ea);
int EAGetFirstCipher (int ea);
int EAGetLastCipher (int ea);
int EAGetNextCipher (int ea, int previousCipherId);
int EAGetPreviousCipher (int ea, int previousCipherId);
int EAIsFormatEnabled (int ea);
BOOL EAIsModeSupported (int ea, int testedMode);

char *HashGetName (int hash_algo_id);
BOOL HashIsDeprecated (int hashId);

int GetMaxPkcs5OutSize (void);

void EncryptDataUnits (unsigned __int8 *buf, const UINT64_STRUCT *structUnitNo, quint32 nbrUnits, PCRYPTO_INFO ci);
void EncryptDataUnitsCurrentThread (unsigned __int8 *buf, const UINT64_STRUCT *structUnitNo, quint64 nbrUnits, PCRYPTO_INFO ci);
void DecryptDataUnits (unsigned __int8 *buf, const UINT64_STRUCT *structUnitNo, quint32 nbrUnits, PCRYPTO_INFO ci);
void DecryptDataUnitsCurrentThread (unsigned __int8 *buf, const UINT64_STRUCT *structUnitNo, quint64 nbrUnits, PCRYPTO_INFO ci);
void EncryptBuffer (unsigned __int8 *buf, quint64 len, PCRYPTO_INFO cryptoInfo);
void DecryptBuffer (unsigned __int8 *buf, quint64 len, PCRYPTO_INFO cryptoInfo);

void EnableHwEncryption (BOOL enable);
BOOL IsHwEncryptionEnabled ();

#ifdef __cplusplus
}
#endif

#endif		/* CRYPTO_H */
