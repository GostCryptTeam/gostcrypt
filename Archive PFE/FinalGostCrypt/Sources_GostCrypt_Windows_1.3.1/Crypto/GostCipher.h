
/*
 Copyright (c) 2008-2011 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/



#ifndef GOST_CIPHER_H
#define GOST_CIPHER_H

//In unsigned chars
#define GOST_KEYSIZE	32
#define GOST_BLOCKSIZE	8
#define GOST_SBOX_SIZE	16

//Production setting, but can be turned off to compare the algorithm with other implementations
#define GOST_DYNAMIC_SBOXES

#define rotl32(b, shift) ((b << shift) | (b >> (32 - shift)))

typedef unsigned char byte;
#ifdef GST_WINDOWS_BOOT
typedef int gst_word;
typedef long gst_dword;
typedef unsigned int gst_uword;
typedef unsigned long gst_udword;
#else
typedef short gst_word;
typedef int gst_dword;
typedef unsigned short gst_uword;
typedef unsigned int gst_udword;
#endif

typedef struct gost_s_box
{
	gst_udword k8[GOST_SBOX_SIZE];
	gst_udword k7[GOST_SBOX_SIZE];
	gst_udword k6[GOST_SBOX_SIZE];
	gst_udword k5[GOST_SBOX_SIZE];
	gst_udword k4[GOST_SBOX_SIZE];
	gst_udword k3[GOST_SBOX_SIZE];
	gst_udword k2[GOST_SBOX_SIZE];
	gst_udword k1[GOST_SBOX_SIZE];
} gost_s_box;

typedef struct gost_kds
{
	gst_udword X0;
	gst_udword X1;
	gst_udword X2;
	gst_udword X3;
	gst_udword X4;
	gst_udword X5;
	gst_udword X6;
	gst_udword X7;
#ifdef GOST_DYNAMIC_SBOXES
	gost_s_box sbox;
#else
	gost_s_box *sbox;
#endif
} gost_kds;

void gost_encrypt(byte *in, byte *out, gost_kds *ks);
void gost_decrypt(byte *in, byte *out, gost_kds *ks);
void gost_set_key(byte *key, gost_kds *ks);
void gost_xor_ks(gost_kds *ks, gost_kds *out_ks, gst_udword *in, gst_dword len);

#endif
