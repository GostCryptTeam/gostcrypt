
/*
 Copyright (c) 2008-2011 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/



#ifndef STRIBOG_H
#define STRIBOG_H
#include<QtGlobal>

#if defined(__cplusplus)
extern "C"
{
#endif

typedef struct
{
	quint8 N[64];
	quint8 hash[64];
	quint8 left;
	quint8 remainder[64];
	quint8 v512[64];
	quint8 v0[64];
	quint8 EPSILON[64];
} STRIBOG_CTX;

void STRIBOG_init(STRIBOG_CTX *ctx);
void STRIBOG_add(STRIBOG_CTX *ctx, quint8 *msg, quint32 len);
void STRIBOG_finalize(STRIBOG_CTX *ctx, quint8 *out);

#if defined(__cplusplus)
}
#endif

#endif
