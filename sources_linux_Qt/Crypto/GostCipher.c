
/*
 Copyright (c) 2008-2011 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/



#include "GostCipher.h"
#include "Stribog.h"

static gost_s_box GostR3411_94_CryptoProParamSet = {
	{0x1,0x3,0xA,0x9,0x5,0xB,0x4,0xF,0x8,0x6,0x7,0xE,0xD,0x0,0x2,0xC},
	{0xD,0xE,0x4,0x1,0x7,0x0,0x5,0xA,0x3,0xC,0x8,0xF,0x6,0x2,0x9,0xB},
	{0x7,0x6,0x2,0x4,0xD,0x9,0xF,0x0,0xA,0x1,0x5,0xB,0x8,0xE,0xC,0x3},
	{0x7,0x6,0x4,0xB,0x9,0xC,0x2,0xA,0x1,0x8,0x0,0xE,0xF,0xD,0x3,0x5},
	{0x4,0xA,0x7,0xC,0x0,0xF,0x2,0x8,0xE,0x1,0x6,0x5,0xD,0xB,0x9,0x3},
	{0x7,0xF,0xC,0xE,0x9,0x4,0x1,0x0,0x3,0xB,0x5,0x2,0x6,0xA,0x8,0xD},
	{0x5,0xF,0x4,0x0,0x2,0xD,0xB,0x9,0x1,0x7,0x6,0x3,0xC,0xE,0xA,0x8},
	{0xA,0x4,0x5,0x6,0x8,0x1,0x3,0x7,0xD,0xC,0xE,0x0,0x9,0x2,0xB,0xF}
	};

#ifdef GOST_DYNAMIC_SBOXES
static void xor_s_box(gost_s_box *s_box, byte *seed)
{
   int i;
   for (i = 0; i < 16; i++)
   {
      s_box->k1[i] ^= (seed[ (i * 4) + 0 ]   ) & 0xF;
      s_box->k2[i] ^= (seed[ (i * 4) + 0 ]>>4) & 0xF;
      s_box->k3[i] ^= (seed[ (i * 4) + 1 ]   ) & 0xF;
      s_box->k4[i] ^= (seed[ (i * 4) + 1 ]>>4) & 0xF;
      s_box->k5[i] ^= (seed[ (i * 4) + 2 ]   ) & 0xF;
      s_box->k6[i] ^= (seed[ (i * 4) + 2 ]>>4) & 0xF;
      s_box->k7[i] ^= (seed[ (i * 4) + 3 ]   ) & 0xF;
      s_box->k8[i] ^= (seed[ (i * 4) + 3 ]>>4) & 0xF;
   }
}
#endif

static gst_udword r(gst_udword n1, gst_udword n2, gst_udword X, gost_s_box *sbox)
{
	n1 += X;
#ifdef GOST_DYNAMIC_SBOXES
	n1 =  sbox->k8[(n1>>28)&0xF] | sbox->k7[(n1>>24)&0xF]
				| sbox->k6[(n1>>20)&0xF] | sbox->k5[(n1>>16)&0xF]
				| sbox->k4[(n1>>12)&0xF] | sbox->k3[(n1>>8)&0xF]
				| sbox->k2[(n1>>4)&0xF] | sbox->k1[n1&0xF];
#else
	n1 =  (gst_udword)sbox->k8[(n1>>28)&0xF]<<28 | (gst_udword)sbox->k7[(n1>>24)&0xF]<<24
				| (gst_udword)sbox->k6[(n1>>20)&0xF]<<20 | (gst_udword)sbox->k5[(n1>>16)&0xF]<<16
				| (gst_udword)sbox->k4[(n1>>12)&0xF]<<12 | (gst_udword)sbox->k3[(n1>>8)&0xF]<<8
				| (gst_udword)sbox->k2[(n1>>4)&0xF]<<4 | (gst_udword)sbox->k1[n1&0xF];
#endif
	n1 = rotl32(n1, 11);
	n2 ^= n1;
	return n2;
}

void gost_encrypt(byte *in, byte *out, gost_kds *ks)
{
	gst_udword n1, n2;
	gost_s_box *sbox;
	n1 = 0;
	n2 = 0;

	n1 = (gst_udword)in[3] << 24 | (gst_udword)in[2] << 16 | (gst_udword)in[1] << 8 | (gst_udword)in[0];
	n2 = (gst_udword)in[7] << 24 | (gst_udword)in[6] << 16 | (gst_udword)in[5] << 8 | (gst_udword)in[4];
#ifdef GOST_DYNAMIC_SBOXES
	sbox = &ks->sbox;
#else
	sbox = ks->sbox;
#endif

	//Rounds 1-24
	n2 = r(n1, n2, ks->X0, sbox);
	n1 = r(n2, n1, ks->X1, sbox);
	n2 = r(n1, n2, ks->X2, sbox);
	n1 = r(n2, n1, ks->X3, sbox);
	n2 = r(n1, n2, ks->X4, sbox);
	n1 = r(n2, n1, ks->X5, sbox);
	n2 = r(n1, n2, ks->X6, sbox);
	n1 = r(n2, n1, ks->X7, sbox);

	n2 = r(n1, n2, ks->X0, sbox);
	n1 = r(n2, n1, ks->X1, sbox);
	n2 = r(n1, n2, ks->X2, sbox);
	n1 = r(n2, n1, ks->X3, sbox);
	n2 = r(n1, n2, ks->X4, sbox);
	n1 = r(n2, n1, ks->X5, sbox);
	n2 = r(n1, n2, ks->X6, sbox);
	n1 = r(n2, n1, ks->X7, sbox);

	n2 = r(n1, n2, ks->X0, sbox);
	n1 = r(n2, n1, ks->X1, sbox);
	n2 = r(n1, n2, ks->X2, sbox);
	n1 = r(n2, n1, ks->X3, sbox);
	n2 = r(n1, n2, ks->X4, sbox);
	n1 = r(n2, n1, ks->X5, sbox);
	n2 = r(n1, n2, ks->X6, sbox);
	n1 = r(n2, n1, ks->X7, sbox);

	//Rounds 25-32
	n2 = r(n1, n2, ks->X7, sbox);
	n1 = r(n2, n1, ks->X6, sbox);
	n2 = r(n1, n2, ks->X5, sbox);
	n1 = r(n2, n1, ks->X4, sbox);
	n2 = r(n1, n2, ks->X3, sbox);
	n1 = r(n2, n1, ks->X2, sbox);
	n2 = r(n1, n2, ks->X1, sbox);
	n1 = r(n2, n1, ks->X0, sbox);

	out[0] = (byte)(n2 & 0xFF); out[1] = (byte)((n2 >> 8) & 0xFF); out[2] = (byte)((n2 >> 16) & 0xFF); out[3] = (byte)((n2 >> 24) & 0xFF);
	out[4] = (byte)(n1 & 0xFF); out[5] = (byte)((n1 >> 8) & 0xFF); out[6] = (byte)((n1 >> 16) & 0xFF); out[7] = (byte)((n1 >> 24) & 0xFF);
}

void gost_decrypt(byte *in, byte *out, gost_kds *ks)
{
	gst_udword n1, n2;
	gost_s_box *sbox;
	n1 = 0;
	n2 = 0;

	n1 = (gst_udword)in[3] << 24 | (gst_udword)in[2] << 16 | (gst_udword)in[1] << 8 | (gst_udword)in[0];
	n2 = (gst_udword)in[7] << 24 | (gst_udword)in[6] << 16 | (gst_udword)in[5] << 8 | (gst_udword)in[4];
#ifdef GOST_DYNAMIC_SBOXES
	sbox = &ks->sbox;
#else
	sbox = ks->sbox;
#endif

	//Rounds 1-8
	n2 = r(n1, n2, ks->X0, sbox);
	n1 = r(n2, n1, ks->X1, sbox);
	n2 = r(n1, n2, ks->X2, sbox);
	n1 = r(n2, n1, ks->X3, sbox);
	n2 = r(n1, n2, ks->X4, sbox);
	n1 = r(n2, n1, ks->X5, sbox);
	n2 = r(n1, n2, ks->X6, sbox);
	n1 = r(n2, n1, ks->X7, sbox);

	//Rounds 9-32
	n2 = r(n1, n2, ks->X7, sbox);
	n1 = r(n2, n1, ks->X6, sbox);
	n2 = r(n1, n2, ks->X5, sbox);
	n1 = r(n2, n1, ks->X4, sbox);
	n2 = r(n1, n2, ks->X3, sbox);
	n1 = r(n2, n1, ks->X2, sbox);
	n2 = r(n1, n2, ks->X1, sbox);
	n1 = r(n2, n1, ks->X0, sbox);

	n2 = r(n1, n2, ks->X7, sbox);
	n1 = r(n2, n1, ks->X6, sbox);
	n2 = r(n1, n2, ks->X5, sbox);
	n1 = r(n2, n1, ks->X4, sbox);
	n2 = r(n1, n2, ks->X3, sbox);
	n1 = r(n2, n1, ks->X2, sbox);
	n2 = r(n1, n2, ks->X1, sbox);
	n1 = r(n2, n1, ks->X0, sbox);

	n2 = r(n1, n2, ks->X7, sbox);
	n1 = r(n2, n1, ks->X6, sbox);
	n2 = r(n1, n2, ks->X5, sbox);
	n1 = r(n2, n1, ks->X4, sbox);
	n2 = r(n1, n2, ks->X3, sbox);
	n1 = r(n2, n1, ks->X2, sbox);
	n2 = r(n1, n2, ks->X1, sbox);
	n1 = r(n2, n1, ks->X0, sbox);
	
	out[0] = (byte)(n2 & 0xFF); out[1] = (byte)((n2 >> 8) & 0xFF); out[2] = (byte)((n2 >> 16) & 0xFF); out[3] = (byte)((n2 >> 24) & 0xFF);
	out[4] = (byte)(n1 & 0xFF); out[5] = (byte)((n1 >> 8) & 0xFF); out[6] = (byte)((n1 >> 16) & 0xFF); out[7] = (byte)((n1 >> 24) & 0xFF);
}

void gost_set_key(byte *key, gost_kds *ks)
{
    gst_udword i;

#ifdef GOST_DYNAMIC_SBOXES
	STRIBOG_CTX sctx;
	byte sbox_seed[64];

	ks->sbox = GostR3411_94_CryptoProParamSet;

	//Generate pseudorandom data based on the key
	STRIBOG_init(&sctx);
	STRIBOG_add(&sctx, key, 32);
	STRIBOG_finalize(&sctx, sbox_seed);

	xor_s_box(&ks->sbox, sbox_seed);

	/* In order to avoid 7 bitshift operations on each pass through the S-Box,
	* this step is pre-processed once during the set_key phase. If memory contraints
	* become a problem, this step can be skipped and the sbox struct can change its
	* unsigned int arrays to unsigned char arrays, going from 512 unsigned chars to 128.
	*/
	for (i = 0; i < GOST_SBOX_SIZE; i++)
	{
		ks->sbox.k8[i] <<= 28;
		ks->sbox.k7[i] <<= 24;
		ks->sbox.k6[i] <<= 20;
		ks->sbox.k5[i] <<= 16;
		ks->sbox.k4[i] <<= 12;
		ks->sbox.k3[i] <<= 8;
		ks->sbox.k2[i] <<= 4;
	}
#else
	ks->sbox = &GostR3411_94_CryptoProParamSet;
#endif

	ks->X0 = 0;
	ks->X1 = 0;
	ks->X2 = 0;
	ks->X3 = 0;
	ks->X4 = 0;
	ks->X5 = 0;
	ks->X6 = 0;
	ks->X7 = 0;

	//Set the key
	for (i = 0; i < GOST_KEYSIZE / 8; i++)
	{
		ks->X0 |= (gst_udword)key[i + 0] << (i * 8);
		ks->X1 |= (gst_udword)key[i + 4] << (i * 8);
		ks->X2 |= (gst_udword)key[i + 8] << (i * 8);
		ks->X3 |= (gst_udword)key[i + 12] << (i * 8);
		ks->X4 |= (gst_udword)key[i + 16] << (i * 8);
		ks->X5 |= (gst_udword)key[i + 20] << (i * 8);
		ks->X6 |= (gst_udword)key[i + 24] << (i * 8);
		ks->X7 |= (gst_udword)key[i + 28] << (i * 8);
	}
}

void gost_xor_ks(gost_kds *ks, gost_kds *out_ks, gst_udword *in, gst_dword len)
{
	gst_dword i;
	if (!len)
		return;

	out_ks->X0 = ks->X0 ^ in[0];
	out_ks->X1 = ks->X1 ^ in[0];
	out_ks->X2 = ks->X2 ^ in[0];
	out_ks->X3 = ks->X3 ^ in[0];
	out_ks->X4 = ks->X4 ^ in[0];
	out_ks->X5 = ks->X5 ^ in[0];
	out_ks->X6 = ks->X6 ^ in[0];
	out_ks->X7 = ks->X7 ^ in[0];

	for (i = 1; i < len; i++)
	{
		out_ks->X0 ^= in[i];
		out_ks->X1 ^= in[i];
		out_ks->X2 ^= in[i];
		out_ks->X3 ^= in[i];
		out_ks->X4 ^= in[i];
		out_ks->X5 ^= in[i];
		out_ks->X6 ^= in[i];
		out_ks->X7 ^= in[i];
	}
}
