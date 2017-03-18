/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "Hash.h"

#include "Crypto/Whirlpool.h"
#include "Crypto/Stribog.h"
#include "Crypto/GostHash.h"

namespace GostCrypt
{
	HashList Hash::GetAvailableAlgorithms ()
	{
		HashList l;

		l.push_back (shared_ptr <Hash> (new Stribog ()));
		l.push_back (shared_ptr <Hash> (new GostHash ()));
		l.push_back (shared_ptr <Hash> (new Whirlpool ()));

		return l;
	}

	void Hash::ValidateDataParameters (const ConstBufferPtr &data) const
	{
		if (data.Size() < 1)
			throw ParameterIncorrect (SRC_POS);
	}

	void Hash::ValidateDigestParameters (const BufferPtr &buffer) const
	{
		if (buffer.Size() != GetDigestSize ())
			throw ParameterIncorrect (SRC_POS);
	}

	// Whirlpool
	Whirlpool::Whirlpool ()
	{
		Context.Allocate (sizeof (WHIRLPOOL_CTX));
		Init();
	}

	void Whirlpool::GetDigest (const BufferPtr &buffer)
	{
		if_debug (ValidateDigestParameters (buffer));
		WHIRLPOOL_finalize ((WHIRLPOOL_CTX *) Context.Ptr(), buffer);
	}

	void Whirlpool::Init ()
	{
		WHIRLPOOL_init ((WHIRLPOOL_CTX *) Context.Ptr());
	}

	void Whirlpool::ProcessData (const ConstBufferPtr &data)
	{
		if_debug (ValidateDataParameters (data));
		WHIRLPOOL_add (data.Get(), (int) data.Size() * 8, (WHIRLPOOL_CTX *) Context.Ptr());
	}
	
	// Stribog
	Stribog::Stribog ()
	{
		Context.Allocate (sizeof (STRIBOG_CTX));
		Init ();
	}
	
	void Stribog::GetDigest (const BufferPtr &buffer)
	{
		if_debug (ValidateDigestParameters (buffer));
		STRIBOG_finalize ((STRIBOG_CTX *) Context.Ptr(), buffer);
	}
	
	void Stribog::Init ()
	{
		STRIBOG_init ((STRIBOG_CTX *) Context.Ptr());
	}
	
	void Stribog::ProcessData (const ConstBufferPtr &data)
	{
		if_debug (ValidateDataParameters (data));
		STRIBOG_add ((STRIBOG_CTX *) Context.Ptr(), (byte *) data.Get(), data.Size());
	}
	
	// GOST R 34.11-94
	GostHash::GostHash ()
	{
		Context.Allocate (sizeof (gost_hash_ctx));
		Init ();
	}
	
	void GostHash::GetDigest (const BufferPtr &buffer)
	{
		if_debug (ValidateDigestParameters (buffer));
		GOSTHASH_finalize ((gost_hash_ctx *) Context.Ptr(), buffer);
	}
	
	void GostHash::Init ()
	{
		GOSTHASH_init ((gost_hash_ctx *) Context.Ptr());
	}
	
	void GostHash::ProcessData (const ConstBufferPtr &data)
	{
		if_debug (ValidateDataParameters (data));
		GOSTHASH_add ((byte *) data.Get(), data.Size(), (gost_hash_ctx *) Context.Ptr());
	}
}
