
/*
 Copyright (c) 2008-2011 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/



#ifndef GOSTHASH_H
#define GOSTHASH_H
#include<QtGlobal>

#if defined(__cplusplus)
extern "C"
{
#endif

#define gosthash_rotl32(b, shift) ((b << shift) | (b >> (32 - shift)))

#define GOSTHASH_GOST_SBOX_SIZE 16
#define GOSTHASH_GOST_KEYSIZE       32


typedef struct gosthash_s_box
{
    quint8 k8[GOSTHASH_GOST_SBOX_SIZE];
    quint8 k7[GOSTHASH_GOST_SBOX_SIZE];
    quint8 k6[GOSTHASH_GOST_SBOX_SIZE];
    quint8 k5[GOSTHASH_GOST_SBOX_SIZE];
    quint8 k4[GOSTHASH_GOST_SBOX_SIZE];
    quint8 k3[GOSTHASH_GOST_SBOX_SIZE];
    quint8 k2[GOSTHASH_GOST_SBOX_SIZE];
    quint8 k1[GOSTHASH_GOST_SBOX_SIZE];
} gosthash_s_box;

typedef struct
{
    qint32 len;
    quint32 left;
    quint8 H[32];
    quint8 S[32];
    quint8 remainder[32];
} gost_hash_ctx;

void GOSTHASH_init(gost_hash_ctx* ctx);
void GOSTHASH_add(quint8* block, quint32 len, gost_hash_ctx* ctx);
void GOSTHASH_finalize(gost_hash_ctx* ctx, quint8* out);

#if defined(__cplusplus)
}
#endif

#endif
