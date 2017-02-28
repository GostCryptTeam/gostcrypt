
/*
 Copyright (c) 2008-2011 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/



#ifndef GOSTHASH_H
#define GOSTHASH_H

#define gosthash_rotl32(b, shift) ((b << shift) | (b >> (32 - shift)))

#define GOSTHASH_GOST_SBOX_SIZE	16
#define GOSTHASH_GOST_KEYSIZE		32

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


typedef struct gosthash_s_box
{
	byte k8[GOSTHASH_GOST_SBOX_SIZE];
	byte k7[GOSTHASH_GOST_SBOX_SIZE];
	byte k6[GOSTHASH_GOST_SBOX_SIZE];
	byte k5[GOSTHASH_GOST_SBOX_SIZE];
	byte k4[GOSTHASH_GOST_SBOX_SIZE];
	byte k3[GOSTHASH_GOST_SBOX_SIZE];
	byte k2[GOSTHASH_GOST_SBOX_SIZE];
	byte k1[GOSTHASH_GOST_SBOX_SIZE];
} gosthash_s_box;

typedef struct
{
	gst_dword len;
	gst_udword left;
	byte H[32];
	byte S[32];
	byte remainder[32];
} gost_hash_ctx;

void GOSTHASH_init (gost_hash_ctx *ctx);
void GOSTHASH_add (byte *in, gst_udword len, gost_hash_ctx *ctx);
void GOSTHASH_finalize (gost_hash_ctx *ctx, byte *out);

#endif
