
/*
 Copyright (c) 2008-2011 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GOST_CIPHER_H
#define GOST_CIPHER_H

#include<QtGlobal>

#if defined(__cplusplus)
extern "C"
{
#endif

//In unsigned chars
#define GOST_KEYSIZE	32
#define GOST_BLOCKSIZE	8
#define GOST_SBOX_SIZE	16

//Production setting, but can be turned off to compare the algorithm with other implementations
#define GOST_DYNAMIC_SBOXES

#define rotl32(b, shift) ((b << shift) | (b >> (32 - shift)))

typedef struct gost_s_box
{
    quint32 k8[GOST_SBOX_SIZE];
    quint32 k7[GOST_SBOX_SIZE];
    quint32 k6[GOST_SBOX_SIZE];
    quint32 k5[GOST_SBOX_SIZE];
    quint32 k4[GOST_SBOX_SIZE];
    quint32 k3[GOST_SBOX_SIZE];
    quint32 k2[GOST_SBOX_SIZE];
    quint32 k1[GOST_SBOX_SIZE];
} gost_s_box;

typedef struct gost_kds
{
    quint32 X0;
    quint32 X1;
    quint32 X2;
    quint32 X3;
    quint32 X4;
    quint32 X5;
    quint32 X6;
    quint32 X7;
#ifdef GOST_DYNAMIC_SBOXES
	gost_s_box sbox;
#else
	gost_s_box *sbox;
#endif
} gost_kds;

void gost_encrypt(quint8 *in, quint8 *out, gost_kds *ks);
void gost_decrypt(quint8 *in, quint8 *out, gost_kds *ks);
void gost_set_key(quint8 *key, gost_kds *ks);
void gost_xor_ks(gost_kds *ks, gost_kds *out_ks, quint32 *in, qint32 len);

#if defined(__cplusplus)
}
#endif

#endif
