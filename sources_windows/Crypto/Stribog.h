
/*
 Copyright (c) 2008-2011 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/



#ifndef STRIBOG_H
#define STRIBOG_H

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

typedef struct
{
	byte N[64];
	byte hash[64];
	byte left;
	byte remainder[64];
	byte v512[64];
	byte v0[64];
	byte EPSILON[64];
} STRIBOG_CTX;

void STRIBOG_init(STRIBOG_CTX *ctx);
void STRIBOG_add(STRIBOG_CTX *ctx, byte *msg, gst_udword len);
void STRIBOG_finalize(STRIBOG_CTX *ctx, byte *out);

#endif
