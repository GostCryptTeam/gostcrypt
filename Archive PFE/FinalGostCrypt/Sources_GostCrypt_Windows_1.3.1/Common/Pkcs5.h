/*
 Legal Notice: Some portions of the source code contained in this file were
 derived from the source code of Encryption for the Masses 2.02a, which is
 Copyright (c) 1998-2000 Paul Le Roux and which is governed by the 'License
 Agreement for Encryption for the Masses'. Modifications and additions to
 the original source code (contained in this file) and all other portions
 of this file are Copyright (c) 2003-2008 TrueCrypt Developers Association
 and are governed by the TrueCrypt License 3.0 the full text of which is
 contained in the file License.txt included in TrueCrypt binary and source
 code distribution packages. */

#ifndef GST_HEADER_PKCS5
#define GST_HEADER_PKCS5

#include "Gstdefs.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#ifndef GST_WINDOWS_BOOT
void hmac_whirlpool (char *k, int lk, char *d, int ld, char *out, int t);
void derive_u_whirlpool (char *pwd, int pwd_len, char *salt, int salt_len, int iterations, char *u, int b);
void derive_key_whirlpool (char *pwd, int pwd_len, char *salt, int salt_len, int iterations, char *dk, int dklen);

void hmac_gosthash (char *k, int32 lk, char *d, int32 ld, char *out);
void derive_u_gosthash (char *pwd, int pwd_len, char *salt, int salt_len, int iterations, char *u, int b);
void derive_key_gosthash (char *pwd, int pwd_len, char *salt, int salt_len, int iterations, char *dk, int dklen);
#endif

void hmac_stribog (char *k, int32 lk, char *d, int32 ld, char *out);
void derive_u_stribog (char *pwd, int pwd_len, char *salt, int salt_len, int iterations, char *u, int b);
void derive_key_stribog (char *pwd, int pwd_len, char *salt, int salt_len, int iterations, char *dk, int dklen);

int get_pkcs5_iteration_count (int pkcs5_prf_id, BOOL bBoot);
char *get_pkcs5_prf_name (int pkcs5_prf_id);

#if defined(__cplusplus)
}
#endif

#endif // GST_HEADER_PKCS5
