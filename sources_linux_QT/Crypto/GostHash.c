
/*
 Copyright (c) 2008-2011 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/



/*
 * This is an implementation of the GOST R 34.11-94 Hash Function Algorithm.
 */

#include "GostHash.h"

static byte C_3[32] =
{
	0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
	0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
	0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF,
	0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF
};

/* A stripped-down GOST 28147-89 encryption algorithm for the use in this hash function */
static gosthash_s_box GostR3411_94_CryptoProParamSet = {
	{0x1,0x3,0xA,0x9,0x5,0xB,0x4,0xF,0x8,0x6,0x7,0xE,0xD,0x0,0x2,0xC},
	{0xD,0xE,0x4,0x1,0x7,0x0,0x5,0xA,0x3,0xC,0x8,0xF,0x6,0x2,0x9,0xB},
	{0x7,0x6,0x2,0x4,0xD,0x9,0xF,0x0,0xA,0x1,0x5,0xB,0x8,0xE,0xC,0x3},
	{0x7,0x6,0x4,0xB,0x9,0xC,0x2,0xA,0x1,0x8,0x0,0xE,0xF,0xD,0x3,0x5},
	{0x4,0xA,0x7,0xC,0x0,0xF,0x2,0x8,0xE,0x1,0x6,0x5,0xD,0xB,0x9,0x3},
	{0x7,0xF,0xC,0xE,0x9,0x4,0x1,0x0,0x3,0xB,0x5,0x2,0x6,0xA,0x8,0xD},
	{0x5,0xF,0x4,0x0,0x2,0xD,0xB,0x9,0x1,0x7,0x6,0x3,0xC,0xE,0xA,0x8},
	{0xA,0x4,0x5,0x6,0x8,0x1,0x3,0x7,0xD,0xC,0xE,0x0,0x9,0x2,0xB,0xF}
};

static gst_udword r(gst_udword n1, gst_udword n2, gst_udword X)
{
	gosthash_s_box *sbox = &GostR3411_94_CryptoProParamSet;
	n1 += X;
	n1 =  (gst_udword)sbox->k8[(n1>>28)&0xF]<<28 | (gst_udword)sbox->k7[(n1>>24)&0xF]<<24
				| (gst_udword)sbox->k6[(n1>>20)&0xF]<<20 | (gst_udword)sbox->k5[(n1>>16)&0xF]<<16
				| (gst_udword)sbox->k4[(n1>>12)&0xF]<<12 | (gst_udword)sbox->k3[(n1>>8)&0xF]<<8
				| (gst_udword)sbox->k2[(n1>>4)&0xF]<<4 | (gst_udword)sbox->k1[n1&0xF];
	n1 = gosthash_rotl32(n1, 11);
	n2 ^= n1;
	return n2;
}

static void gost_encrypt_with_key(byte *in, byte *out, byte *key)
{
	gst_dword i;
	gst_udword n1, n2;
	gst_udword X0, X1, X2, X3, X4, X5, X6, X7;

	n1 = (gst_udword)in[3] << 24 | (gst_udword)in[2] << 16 | (gst_udword)in[1] << 8 | (gst_udword)in[0];
	n2 = (gst_udword)in[7] << 24 | (gst_udword)in[6] << 16 | (gst_udword)in[5] << 8 | (gst_udword)in[4];

	/* Set the 8 round keys */
	X0 = 0;
	X1 = 0;
	X2 = 0;
	X3 = 0;
	X4 = 0;
	X5 = 0;
	X6 = 0;
	X7 = 0;
	
	for (i = 0; i < GOSTHASH_GOST_KEYSIZE / 8; i++)
	{
		X0 |= (gst_udword)key[i + 0] << (i * 8);
		X1 |= (gst_udword)key[i + 4] << (i * 8);
		X2 |= (gst_udword)key[i + 8] << (i * 8);
		X3 |= (gst_udword)key[i + 12] << (i * 8);
		X4 |= (gst_udword)key[i + 16] << (i * 8);
		X5 |= (gst_udword)key[i + 20] << (i * 8);
		X6 |= (gst_udword)key[i + 24] << (i * 8);
		X7 |= (gst_udword)key[i + 28] << (i * 8);
	}

	/* Encryption rounds 1-24 */
	n2 = r(n1, n2, X0);
	n1 = r(n2, n1, X1);
	n2 = r(n1, n2, X2);
	n1 = r(n2, n1, X3);
	n2 = r(n1, n2, X4);
	n1 = r(n2, n1, X5);
	n2 = r(n1, n2, X6);
	n1 = r(n2, n1, X7);
	
	n2 = r(n1, n2, X0);
	n1 = r(n2, n1, X1);
	n2 = r(n1, n2, X2);
	n1 = r(n2, n1, X3);
	n2 = r(n1, n2, X4);
	n1 = r(n2, n1, X5);
	n2 = r(n1, n2, X6);
	n1 = r(n2, n1, X7);

	n2 = r(n1, n2, X0);
	n1 = r(n2, n1, X1);
	n2 = r(n1, n2, X2);
	n1 = r(n2, n1, X3);
	n2 = r(n1, n2, X4);
	n1 = r(n2, n1, X5);
	n2 = r(n1, n2, X6);
	n1 = r(n2, n1, X7);

	/* Encryption rounds 25-32 */
	n2 = r(n1, n2, X7);
	n1 = r(n2, n1, X6);
	n2 = r(n1, n2, X5);
	n1 = r(n2, n1, X4);
	n2 = r(n1, n2, X3);
	n1 = r(n2, n1, X2);
	n2 = r(n1, n2, X1);
	n1 = r(n2, n1, X0);

	out[0] = (byte)(n2 & 0xFF); out[1] = (byte)((n2 >> 8) & 0xFF); out[2] = (byte)((n2 >> 16) & 0xFF); out[3] = (byte)((n2 >> 24) & 0xFF);
	out[4] = (byte)(n1 & 0xFF); out[5] = (byte)((n1 >> 8) & 0xFF); out[6] = (byte)((n1 >> 16) & 0xFF); out[7] = (byte)((n1 >> 24) & 0xFF);
}


/* Library-neutral memset, memcpy and memmove */
static void copy_blocks(byte *dst, byte *src, gst_dword len)
{
	gst_dword i;
	for (i = 0; i < len; i++)
		dst[i] = src[i];
}

static void move_blocks (byte *to, byte *from, gst_dword len)
{
	gst_dword i;
	if (from < to)
		for (i = len-1; i >= 0; i--)
			to[i] = from[i];
	else
		for (i = 0; i < len; i++)
			to[i] = from[i];
}

static void set_blocks (byte *ptr, byte val, gst_dword len)
{
	gst_dword i;
	for (i = 0; i < len; i++)
		ptr[i] = val;
}

/* arbitrary-length add and xor */
static void add_blocks (byte *T, byte *F, gst_dword len)
{
	gst_dword i;
	gst_word carry = 0;
	gst_word sum;
	for (i = 0; i < len; i++)
	{
		sum = (gst_word)T[i] + (gst_word)F[i] + carry;
		T[i] = (byte)sum & 0xFF;
		carry = sum >> 8;
	}
}

static void xor_blocks (byte *T, byte *F, byte *S, gst_dword len)
{
	gst_dword i;
	for (i = 0; i < len; i++)
		T[i] = F[i] ^ S[i];
}

/* GOST R 34.11-94 mixing functions */
/* This function maps bytes from F to T in the following way:
 * T[0+0] = F[0+0]; T[1+0] = F[8+0]; ...; T[3+0] = F[24+0];
 * T[0+4] = F[0+1]; T[1+4] = F[8+1]; ...;
 * ...; T[2+28] = F[16+7]; T[3+28] = F[24+7];
 */
static void P_transform (byte *F, byte *T)
{
	gst_dword i, k;
	for (i = 0; i < 4; i++)
		for (k = 0; k < 8; k++)
			T[i+4*k] = F[8*i+k];
}

/* This function maps bytes from F to T in the following way:
 * T[ 0..23] = F[8..31];
 * T[24..31] = F[0..7] XOR F[8..15] */
static void A(byte *F, byte *T)
{
	byte i;
	for (i = 0; i < 24; i++)
		T[i] = F[i+8];
	for (i = 0; i < 8; i++)
		T[i+24] = F[i] ^ F[i+8];
}

static void PSI (byte *X)
{
	gst_word T;

	T = (gst_word)(X[0]^X[2]^X[4]^X[6]^X[24]^X[30])|
		((gst_word)(X[1]^X[3]^X[5]^X[7]^X[25]^X[31])<<8);
	move_blocks(X, X+2, 30);
	X[30] = (byte)(T&0xFF);
	X[31] = (byte)(T>>8);
}

void GOSTHASH_init (gost_hash_ctx *ctx)
{
	set_blocks((byte *)ctx, 0, sizeof(gost_hash_ctx));
}

static void step (byte *H, byte *M, gost_hash_ctx *ctx)
{
	byte U[32], W[32], V[32], S[32], K[4][32];
	gst_dword i;
	
	xor_blocks(W, H, M, 32);
	P_transform(W, K[0]); //First key

	gost_encrypt_with_key (H, S, K[0]);

	A (H, U);
	A (M, V);
	A (V, V);
	xor_blocks(W, U, V, 32);
	P_transform(W, K[1]); //Second key

	gost_encrypt_with_key (H + 8, S + 8, K[1]);

	A (U, U);
	/* As the other C values are all 0's, we only need to XOR
     * with C[3] */
	xor_blocks(U, U, C_3, 32);
	A (V, V);
	A (V, V);
	xor_blocks (W, U, V, 32);
	P_transform (W, K[2]); //Third key
	
	gost_encrypt_with_key (H + 16, S + 16, K[2]);
	
	A (U, U);
	A (V, V);
	A (V, V);
	xor_blocks (W, U, V, 32);
	P_transform (W, K[3]); //Fourth key
	
	gost_encrypt_with_key (H + 24, S + 24, K[3]);

	for (i = 0; i < 12; i++)
		PSI (S);
	xor_blocks (S, S, M, 32);
	PSI (S);
	xor_blocks (S, S, H, 32);
	for (i = 0; i < 61; i++)
		PSI (S);
	copy_blocks (H, S, 32);
}

void GOSTHASH_add (byte *block, gst_udword len, gost_hash_ctx *ctx)
{
	gst_udword add_bytes;

	byte *curptr = block;
	byte *barrier = block + (len - 32); //In order that curptr += 32 won't overshoot len.

	if (ctx->left) //There are unsigned chars left from the last GOSTHASH_add
	{
		add_bytes = (32 - ctx->left) > len ? len : (32 - ctx->left);
		copy_blocks(ctx->remainder + (byte)ctx->left, block, (gst_dword)add_bytes);
		if ((ctx->left + add_bytes) < 32) //This can be finished in the finalize stage if needed
		{
			return;
		}
		
		curptr += add_bytes;
		step (ctx->H, ctx->remainder, ctx);
		add_blocks(ctx->S, ctx->remainder, 32);
		ctx->len += 32;
		ctx->left = 0;
	}
	else if (ctx->left + len < 32)
	{
		copy_blocks(ctx->remainder + (byte)ctx->left, block, (gst_dword)len);
		ctx->left += len;
		return;
	}

	while (curptr <= barrier) //Add the input block to the hash
	{
		step(ctx->H, curptr, ctx);
		
		add_blocks(ctx->S, curptr, 32); //Add 32 unsigned chars of the message to the encrypted message
		
		ctx->len += 32; //We've processed 32 unsigned chars
		curptr += 32; //Advance the block-pointer 32 unsigned chars
	}

	if (curptr != block + len) //If we have unsigned chars remaining, add them for the next GOSTHASH_add of _finalize
	{
		ctx->left = (gst_udword)(block + len - curptr);
		copy_blocks(ctx->remainder, curptr, (gst_dword)ctx->left);
	}
}

void GOSTHASH_finalize (gost_hash_ctx *ctx, byte *out)
{
	byte buf[32];
	byte H[32];
	byte S[32];
	gst_dword final_len;
	gst_dword bptr;

	final_len = ctx->len;
	
	copy_blocks(H, ctx->H, 32);
	copy_blocks(S, ctx->S, 32);

	if (ctx->left) //Handle any remaining bytes
	{
		set_blocks(buf, 0, 32);
		copy_blocks(buf, ctx->remainder, (gst_dword)ctx->left);
		step (H, buf, ctx);
		add_blocks (S, buf, 32);
		final_len += ctx->left;
	}

	set_blocks(buf, 0, 32);
	bptr = 0;
	final_len <<= 3;
	while (final_len > 0)
	{
		buf[bptr++] = (byte)(final_len & 0xFF);
		final_len >>= 8;
	}

	step (H, buf, ctx);
	step (H, S, ctx);
	copy_blocks (out, H, 32);
}