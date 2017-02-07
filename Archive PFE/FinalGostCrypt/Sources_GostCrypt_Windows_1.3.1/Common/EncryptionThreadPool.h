/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#ifndef GST_HEADER_ENCRYPTION_THREAD_POOL
#define GST_HEADER_ENCRYPTION_THREAD_POOL

#include "Gstdefs.h"
#include "Crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	EncryptDataUnitsWork,
	DecryptDataUnitsWork,
	DeriveKeyWork
} EncryptionThreadPoolWorkType;

void EncryptionThreadPoolBeginKeyDerivation (GST_EVENT *completionEvent, GST_EVENT *noOutstandingWorkItemEvent, LONG *completionFlag, LONG *outstandingWorkItemCount, int pkcs5Prf, char *password, int passwordLength, char *salt, int iterationCount, char *derivedKey);
void EncryptionThreadPoolDoWork (EncryptionThreadPoolWorkType type, byte *data, const UINT64_STRUCT *startUnitNo, uint32 unitCount, PCRYPTO_INFO cryptoInfo);
BOOL EncryptionThreadPoolStart (size_t encryptionFreeCpuCount);
void EncryptionThreadPoolStop ();
size_t GetEncryptionThreadCount ();
size_t GetMaxEncryptionThreadCount ();
BOOL IsEncryptionThreadPoolRunning ();

#ifdef __cplusplus
}
#endif

#endif // GST_HEADER_ENCRYPTION_THREAD_POOL
