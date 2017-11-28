/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "VolumeHash.h"

#include "Crypto/Whirlpool.h"
#include "Crypto/Stribog.h"
#include "Crypto/GostHash.h"
#include "Common/Pkcs5.h"

namespace GostCrypt
{
namespace Volume {

	VolumeHashList VolumeHash::GetAvailableAlgorithms ()
	{
		VolumeHashList l;

		l.push_back (QSharedPointer <VolumeHash> (new Stribog ()));
		l.push_back (QSharedPointer <VolumeHash> (new GostHash ()));
		l.push_back (QSharedPointer <VolumeHash> (new Whirlpool ()));

		return l;
	}

	void VolumeHash::ValidateDataParameters (const ConstBufferPtr &data) const
	{
		if (data.Size() < 1)
            throw;// ParameterIncorrect (SRC_POS);
	}

	void VolumeHash::ValidateDigestParameters (const BufferPtr &buffer) const
	{
		if (buffer.Size() != GetDigestSize ())
            throw;// ParameterIncorrect (SRC_POS);
	}

    void VolumeHash::ValidateKeyDerivationParameters (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const
    {
        if (key.Size() < 1 || password.Size() < 1 || salt.Size() < 1 || iterationCount < 1)
            throw;// (SRC_POS);
    }

    void VolumeHash::DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt) const
    {
        DeriveKey (key, password, salt, GetIterationCount());
    }

	// Whirlpool
	Whirlpool::Whirlpool ()
	{
		Context.Allocate (sizeof (WHIRLPOOL_CTX));
		Init();
	}

	void Whirlpool::GetDigest (const BufferPtr &buffer)
	{
		//if_debug (ValidateDigestParameters (buffer));
		WHIRLPOOL_finalize ((WHIRLPOOL_CTX *) Context.Ptr(), buffer);
	}

	void Whirlpool::Init ()
	{
		WHIRLPOOL_init ((WHIRLPOOL_CTX *) Context.Ptr());
	}

	void Whirlpool::ProcessData (const ConstBufferPtr &data)
	{
		//if_debug (ValidateDataParameters (data));
		WHIRLPOOL_add (data.Get(), (int) data.Size() * 8, (WHIRLPOOL_CTX *) Context.Ptr());
	}

    void Whirlpool::DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const
    {
        ValidateKeyDerivationParameters (key, password, salt, iterationCount);
        derive_key_whirlpool ((char *) password.DataPtr(), (int) password.Size(), (char *) salt.Get(), (int) salt.Size(), iterationCount, (char *) key.Get(), (int) key.Size());
    }

	// Stribog
	Stribog::Stribog ()
	{
		Context.Allocate (sizeof (STRIBOG_CTX));
		Init ();
	}

	void Stribog::GetDigest (const BufferPtr &buffer)
	{
		//if_debug (ValidateDigestParameters (buffer));
		STRIBOG_finalize ((STRIBOG_CTX *) Context.Ptr(), buffer);
	}

	void Stribog::Init ()
	{
		STRIBOG_init ((STRIBOG_CTX *) Context.Ptr());
	}

	void Stribog::ProcessData (const ConstBufferPtr &data)
	{
		//if_debug (ValidateDataParameters (data));
		STRIBOG_add ((STRIBOG_CTX *) Context.Ptr(), (quint8 *) data.Get(), data.Size());
	}

    void Stribog::DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const
    {
        ValidateKeyDerivationParameters (key, password, salt, iterationCount);
        derive_key_stribog ((char *) password.DataPtr(), (int) password.Size(), (char *) salt.Get(), (int) salt.Size(), iterationCount, (char *) key.Get(), (int) key.Size());
    }

	// GOST R 34.11-94
	GostHash::GostHash ()
	{
		Context.Allocate (sizeof (gost_hash_ctx));
		Init ();
	}

	void GostHash::GetDigest (const BufferPtr &buffer)
	{
		//if_debug (ValidateDigestParameters (buffer));
		GOSTHASH_finalize ((gost_hash_ctx *) Context.Ptr(), buffer);
	}

	void GostHash::Init ()
	{
		GOSTHASH_init ((gost_hash_ctx *) Context.Ptr());
	}

	void GostHash::ProcessData (const ConstBufferPtr &data)
	{
		//if_debug (ValidateDataParameters (data));
		GOSTHASH_add ((quint8 *) data.Get(), data.Size(), (gost_hash_ctx *) Context.Ptr());
	}

    void GostHash::DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const
    {
        ValidateKeyDerivationParameters (key, password, salt, iterationCount);
        derive_key_gosthash ((char *) password.DataPtr(), (int) password.Size(), (char *) salt.Get(), (int) salt.Size(), iterationCount, (char *) key.Get(), (int) key.Size());
    }
}
}
