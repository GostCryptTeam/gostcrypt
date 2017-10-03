/*
 Legal Notice: Some portions of the source code contained in this file were
 derived from the source code of Encryption for the Masses 2.02a, which is
 Copyright (c) 1998-2000 Paul Le Roux and which is governed by the 'License
 Agreement for Encryption for the Masses'. Modifications and additions to
 the original source code (contained in this file) and all other portions
 of this file are Copyright (c) 2003-2009 TrueCrypt Developers Association
 and are governed by the TrueCrypt License 3.0 the full text of which is
 contained in the file License.txt included in TrueCrypt binary and source
 code distribution packages. */


/*
 Legal Notice: Some portions of the source code contained in this file were
 derived from the source code of Encryption for the Masses 2.02a, which is
 Copyright (c) 1998-2000 Paul Le Roux and which is governed by the 'License
 Agreement for Encryption for the Masses'. */

#include "Gstdefs.h"

#include <memory.h>
#include "Stribog.h"
#include "Whirlpool.h"
#include "GostHash.h"
#include "Pkcs5.h"
#include "Crypto.h"

void hmac_truncate
  (
	  char *d1,		/* data to be truncated */
	  char *d2,		/* truncated data */
	  int len		/* length in bytes to keep */
)
{
	int i;
	for (i = 0; i < len; i++)
		d2[i] = d1[i];
}

void hmac_stribog
(
	  char *k,		/* secret key */
	  int32 lk,		/* length of the key in bytes */
	  char *d,		/* data */
	  int32 ld,		/* length of data in bytes */
	  char *out		/* output buffer */
)
{
	STRIBOG_CTX ctx;
	char inner[STRIBOG_DIGESTSIZE], outer[STRIBOG_DIGESTSIZE];
	char key[STRIBOG_DIGESTSIZE];
	char buf[STRIBOG_BLOCKSIZE];
	int32 i;

    /* If the key is longer than the hash algorithm block size,
	   let key = stribog(key), as per HMAC specifications. */
	if (lk > STRIBOG_BLOCKSIZE)
	{
		STRIBOG_init (&ctx);
		STRIBOG_add (&ctx, (unsigned char *)k, lk);
		STRIBOG_finalize (&ctx, (unsigned char *) key);

		k = key;
		lk = STRIBOG_DIGESTSIZE;

		burn (&ctx, sizeof(ctx));		// Prevent leaks
	}

	/**** Inner Digest ****/

	STRIBOG_init (&ctx);

	/* Pad the key for inner digest */
	for (i = 0; i < lk; ++i)
		buf[i] = (char) (k[i] ^ 0x36);
	for (i = lk; i < STRIBOG_BLOCKSIZE; ++i)
		buf[i] = 0x36;

	STRIBOG_add (&ctx, (unsigned char *) buf, STRIBOG_BLOCKSIZE);
	STRIBOG_add (&ctx, (unsigned char *) d, ld);

	STRIBOG_finalize (&ctx, (unsigned char *) inner);

	/**** Outer Digest ****/
	
	STRIBOG_init (&ctx);

	for (i = 0; i < lk; ++i)
		buf[i] = (char) (k[i] ^ 0x5C);
	for (i = lk; i < STRIBOG_BLOCKSIZE; ++i)
		buf[i] = 0x5C;

	STRIBOG_add (&ctx, (unsigned char *) buf, STRIBOG_BLOCKSIZE);
	STRIBOG_add (&ctx, (unsigned char *) inner, STRIBOG_DIGESTSIZE);

	STRIBOG_finalize (&ctx, (unsigned char *) out);

	/* Prevent possible leaks. */
	burn (&ctx, sizeof(ctx));
	burn (outer, sizeof(outer));
	burn (inner, sizeof(inner));
	burn (buf, sizeof(buf));
	burn (key, sizeof(key));
}

void derive_u_stribog (char *pwd, int pwd_len, char *salt, int salt_len, int iterations, char *u, int b)
{
	char j[STRIBOG_DIGESTSIZE], k[STRIBOG_DIGESTSIZE];
	char init[128];
	char counter[4];
	int c, i;

	/* iteration 1 */
	memset (counter, 0, 4);
	counter[3] = (char) b;
	memcpy (init, salt, salt_len);	/* salt */
	memcpy (&init[salt_len], counter, 4);	/* big-endian block number */
	hmac_stribog (pwd, pwd_len, init, salt_len + 4, j);
	memcpy (u, j, STRIBOG_DIGESTSIZE);

	/* remaining iterations */
	for (c = 1; c < iterations; c++)
	{
		hmac_stribog (pwd, pwd_len, j, STRIBOG_DIGESTSIZE, k);
		for (i = 0; i < STRIBOG_DIGESTSIZE; i++)
		{
			u[i] ^= k[i];
			j[i] = k[i];
		}
	}

	/* Prevent possible leaks. */
	burn (j, sizeof(j));
	burn (k, sizeof(k));
}

void derive_key_stribog (char *pwd, int pwd_len, char *salt, int salt_len, int iterations, char *dk, int dklen)
{
	char u[STRIBOG_DIGESTSIZE];
	int b, l, r;

	if (dklen % STRIBOG_DIGESTSIZE)
	{
		l = 1 + dklen / STRIBOG_DIGESTSIZE;
	}
	else
	{
		l = dklen / STRIBOG_DIGESTSIZE;
	}

	r = dklen - (l - 1) * STRIBOG_DIGESTSIZE;

	/* first l - 1 blocks */
	for (b = 1; b < l; b++)
	{
		derive_u_stribog (pwd, pwd_len, salt, salt_len, iterations, u, b);
		memcpy (dk, u, STRIBOG_DIGESTSIZE);
		dk += STRIBOG_DIGESTSIZE;
	}

	/* last block */
	derive_u_stribog (pwd, pwd_len, salt, salt_len, iterations, u, b);
	memcpy (dk, u, r);


	/* Prevent possible leaks. */
	burn (u, sizeof(u));
}

void hmac_gosthash
(
	  char *k,		/* secret key */
	  int32 lk,		/* length of the key in bytes */
	  char *d,		/* data */
	  int32 ld,		/* length of data in bytes */
	  char *out		/* output buffer */
)
{
	gost_hash_ctx ctx;
	char inner[GOSTHASH_DIGESTSIZE], outer[GOSTHASH_DIGESTSIZE];
	char key[GOSTHASH_DIGESTSIZE];
	char buf[GOSTHASH_BLOCKSIZE];
	int32 i;

    /* If the key is longer than the hash algorithm block size,
	   let key = gosthash(key), as per HMAC specifications. */
	if (lk > GOSTHASH_BLOCKSIZE)
	{
		GOSTHASH_init (&ctx);
		GOSTHASH_add ((unsigned char *)k, lk, &ctx);
		GOSTHASH_finalize (&ctx, (unsigned char *) key);

		k = key;
		lk = GOSTHASH_DIGESTSIZE;

		burn (&ctx, sizeof(ctx));		// Prevent leaks
	}

	/**** Inner Digest ****/

	GOSTHASH_init (&ctx);

	/* Pad the key for inner digest */
	for (i = 0; i < lk; ++i)
		buf[i] = (char) (k[i] ^ 0x36);
	for (i = lk; i < GOSTHASH_BLOCKSIZE; ++i)
		buf[i] = 0x36;

	GOSTHASH_add ((unsigned char *) buf, GOSTHASH_BLOCKSIZE, &ctx);
	GOSTHASH_add ((unsigned char *) d, ld, &ctx);

	GOSTHASH_finalize (&ctx, (unsigned char *) inner);

	/**** Outer Digest ****/
	
	GOSTHASH_init (&ctx);

	for (i = 0; i < lk; ++i)
		buf[i] = (char) (k[i] ^ 0x5C);
	for (i = lk; i < GOSTHASH_BLOCKSIZE; ++i)
		buf[i] = 0x5C;

	GOSTHASH_add ((unsigned char *) buf, GOSTHASH_BLOCKSIZE, &ctx);
	GOSTHASH_add ((unsigned char *) inner, GOSTHASH_DIGESTSIZE, &ctx);

	GOSTHASH_finalize (&ctx, (unsigned char *) out);

	/* Prevent possible leaks. */
	burn (&ctx, sizeof(ctx));
	burn (outer, sizeof(outer));
	burn (inner, sizeof(inner));
	burn (buf, sizeof(buf));
	burn (key, sizeof(key));
}

void derive_u_gosthash (char *pwd, int pwd_len, char *salt, int salt_len, int iterations, char *u, int b)
{
	char j[GOSTHASH_DIGESTSIZE], k[GOSTHASH_DIGESTSIZE];
	char init[128];
	char counter[4];
	int c, i;

	/* iteration 1 */
	memset (counter, 0, 4);
	counter[3] = (char) b;
	memcpy (init, salt, salt_len);	/* salt */
	memcpy (&init[salt_len], counter, 4);	/* big-endian block number */
	hmac_gosthash (pwd, pwd_len, init, salt_len + 4, j);
	memcpy (u, j, GOSTHASH_DIGESTSIZE);

	/* remaining iterations */
	for (c = 1; c < iterations; c++)
	{
		hmac_gosthash (pwd, pwd_len, j, GOSTHASH_DIGESTSIZE, k);
		for (i = 0; i < GOSTHASH_DIGESTSIZE; i++)
		{
			u[i] ^= k[i];
			j[i] = k[i];
		}
	}

	/* Prevent possible leaks. */
	burn (j, sizeof(j));
	burn (k, sizeof(k));
}

void derive_key_gosthash (char *pwd, int pwd_len, char *salt, int salt_len, int iterations, char *dk, int dklen)
{
	char u[GOSTHASH_DIGESTSIZE];
	int b, l, r;

	if (dklen % GOSTHASH_DIGESTSIZE)
	{
		l = 1 + dklen / GOSTHASH_DIGESTSIZE;
	}
	else
	{
		l = dklen / GOSTHASH_DIGESTSIZE;
	}

	r = dklen - (l - 1) * GOSTHASH_DIGESTSIZE;

	/* first l - 1 blocks */
	for (b = 1; b < l; b++)
	{
		derive_u_gosthash (pwd, pwd_len, salt, salt_len, iterations, u, b);
		memcpy (dk, u, GOSTHASH_DIGESTSIZE);
		dk += GOSTHASH_DIGESTSIZE;
	}

	/* last block */
	derive_u_gosthash (pwd, pwd_len, salt, salt_len, iterations, u, b);
	memcpy (dk, u, r);


	/* Prevent possible leaks. */
	burn (u, sizeof(u));
}

void hmac_whirlpool
(
	  char *k,		/* secret key */
	  int lk,		/* length of the key in bytes */
	  char *d,		/* data */
	  int ld,		/* length of data in bytes */
	  char *out,	/* output buffer, at least "t" bytes */
	  int t
)
{
	WHIRLPOOL_CTX ictx, octx;
	char iwhi[WHIRLPOOL_DIGESTSIZE], owhi[WHIRLPOOL_DIGESTSIZE];
	char key[WHIRLPOOL_DIGESTSIZE];
	char buf[WHIRLPOOL_BLOCKSIZE];
	int i;

    /* If the key is longer than the hash algorithm block size,
	   let key = whirlpool(key), as per HMAC specifications. */
	if (lk > WHIRLPOOL_BLOCKSIZE)
	{
		WHIRLPOOL_CTX tctx;

		WHIRLPOOL_init (&tctx);
		WHIRLPOOL_add ((unsigned char *) k, lk * 8, &tctx);
		WHIRLPOOL_finalize (&tctx, (unsigned char *) key);

		k = key;
		lk = WHIRLPOOL_DIGESTSIZE;

		burn (&tctx, sizeof(tctx));		// Prevent leaks
	}

	/**** Inner Digest ****/

	WHIRLPOOL_init (&ictx);

	/* Pad the key for inner digest */
	for (i = 0; i < lk; ++i)
		buf[i] = (char) (k[i] ^ 0x36);
	for (i = lk; i < WHIRLPOOL_BLOCKSIZE; ++i)
		buf[i] = 0x36;

	WHIRLPOOL_add ((unsigned char *) buf, WHIRLPOOL_BLOCKSIZE * 8, &ictx);
	WHIRLPOOL_add ((unsigned char *) d, ld * 8, &ictx);

	WHIRLPOOL_finalize (&ictx, (unsigned char *) iwhi);

	/**** Outer Digest ****/

	WHIRLPOOL_init (&octx);

	for (i = 0; i < lk; ++i)
		buf[i] = (char) (k[i] ^ 0x5C);
	for (i = lk; i < WHIRLPOOL_BLOCKSIZE; ++i)
		buf[i] = 0x5C;

	WHIRLPOOL_add ((unsigned char *) buf, WHIRLPOOL_BLOCKSIZE * 8, &octx);
	WHIRLPOOL_add ((unsigned char *) iwhi, WHIRLPOOL_DIGESTSIZE * 8, &octx);

	WHIRLPOOL_finalize (&octx, (unsigned char *) owhi);

	/* truncate and print the results */
	t = t > WHIRLPOOL_DIGESTSIZE ? WHIRLPOOL_DIGESTSIZE : t;
	hmac_truncate (owhi, out, t);

	/* Prevent possible leaks. */
	burn (&ictx, sizeof(ictx));
	burn (&octx, sizeof(octx));
	burn (owhi, sizeof(owhi));
	burn (iwhi, sizeof(iwhi));
	burn (buf, sizeof(buf));
	burn (key, sizeof(key));
}

void derive_u_whirlpool (char *pwd, int pwd_len, char *salt, int salt_len, int iterations, char *u, int b)
{
	char j[WHIRLPOOL_DIGESTSIZE], k[WHIRLPOOL_DIGESTSIZE];
	char init[128];
	char counter[4];
	int c, i;

	/* iteration 1 */
	memset (counter, 0, 4);
	counter[3] = (char) b;
	memcpy (init, salt, salt_len);	/* salt */
	memcpy (&init[salt_len], counter, 4);	/* big-endian block number */
	hmac_whirlpool (pwd, pwd_len, init, salt_len + 4, j, WHIRLPOOL_DIGESTSIZE);
	memcpy (u, j, WHIRLPOOL_DIGESTSIZE);

	/* remaining iterations */
	for (c = 1; c < iterations; c++)
	{
		hmac_whirlpool (pwd, pwd_len, j, WHIRLPOOL_DIGESTSIZE, k, WHIRLPOOL_DIGESTSIZE);
		for (i = 0; i < WHIRLPOOL_DIGESTSIZE; i++)
		{
			u[i] ^= k[i];
			j[i] = k[i];
		}
	}

	/* Prevent possible leaks. */
	burn (j, sizeof(j));
	burn (k, sizeof(k));
}

void derive_key_whirlpool (char *pwd, int pwd_len, char *salt, int salt_len, int iterations, char *dk, int dklen)
{
	char u[WHIRLPOOL_DIGESTSIZE];
	int b, l, r;

	if (dklen % WHIRLPOOL_DIGESTSIZE)
	{
		l = 1 + dklen / WHIRLPOOL_DIGESTSIZE;
	}
	else
	{
		l = dklen / WHIRLPOOL_DIGESTSIZE;
	}

	r = dklen - (l - 1) * WHIRLPOOL_DIGESTSIZE;

	/* first l - 1 blocks */
	for (b = 1; b < l; b++)
	{
		derive_u_whirlpool (pwd, pwd_len, salt, salt_len, iterations, u, b);
		memcpy (dk, u, WHIRLPOOL_DIGESTSIZE);
		dk += WHIRLPOOL_DIGESTSIZE;
	}

	/* last block */
	derive_u_whirlpool (pwd, pwd_len, salt, salt_len, iterations, u, b);
	memcpy (dk, u, r);


	/* Prevent possible leaks. */
	burn (u, sizeof(u));
}


char *get_pkcs5_prf_name (int pkcs5_prf_id)
{
	switch (pkcs5_prf_id)
	{
	case WHIRLPOOL:	
		return "HMAC-Whirlpool";

	default:		
		return "(Unknown)";
	}
}

int get_pkcs5_iteration_count (int pkcs5_prf_id, BOOL bBoot)
{
	switch (pkcs5_prf_id)
	{
	case WHIRLPOOL:	
		return 1000;

	default:		
		GST_THROW_FATAL_EXCEPTION;	// Unknown/wrong ID
	}
	return 0;
}
