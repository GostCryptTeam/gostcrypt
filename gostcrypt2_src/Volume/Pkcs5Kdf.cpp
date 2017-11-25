/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "Common/Pkcs5.h"
#include "Pkcs5Kdf.h"
#include "VolumePassword.h"

namespace GostCrypt
{
namespace Volume {

	Pkcs5Kdf::Pkcs5Kdf ()
	{
	}

	Pkcs5Kdf::~Pkcs5Kdf ()
	{
	}

	void Pkcs5Kdf::DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt) const
	{
		DeriveKey (key, password, salt, GetIterationCount());
	}

	QSharedPointer <Pkcs5Kdf> Pkcs5Kdf::GetAlgorithm (const wstring &name)
	{
        std::list<QSharedPointer <Pkcs5Kdf>>::const_iterator algorithmsIt;
        Pkcs5KdfList algorithms = GetAvailableAlgorithms();

        for (algorithmsIt = algorithms.begin(); algorithmsIt != algorithms.end(); ++algorithmsIt)
        {
            if ((*algorithmsIt)->GetName() == name)
                return *algorithmsIt;
		}
        throw;// ParameterIncorrect (SRC_POS);
	}

	QSharedPointer <Pkcs5Kdf> Pkcs5Kdf::GetAlgorithm (const VolumeHash &hash)
	{
        std::list<QSharedPointer <Pkcs5Kdf>>::const_iterator algorithmsIt;
        Pkcs5KdfList algorithms = GetAvailableAlgorithms();

        for (algorithmsIt = algorithms.begin(); algorithmsIt != algorithms.end(); ++algorithmsIt)        {
            if (typeid (*(*algorithmsIt)->GetHash()) == typeid (hash))
                return *algorithmsIt;
		}

        throw;// (SRC_POS);
	}

	Pkcs5KdfList Pkcs5Kdf::GetAvailableAlgorithms ()
	{
		Pkcs5KdfList l;

		l.push_back (QSharedPointer <Pkcs5Kdf> (new Pkcs5HmacStribog ()));
		l.push_back (QSharedPointer <Pkcs5Kdf> (new Pkcs5HmacGostHash()));
		l.push_back (QSharedPointer <Pkcs5Kdf> (new Pkcs5HmacWhirlpool ()));

		return l;
	}

	void Pkcs5Kdf::ValidateParameters (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const
	{
		if (key.Size() < 1 || password.Size() < 1 || salt.Size() < 1 || iterationCount < 1)
            throw;// (SRC_POS);
	}

	void Pkcs5HmacWhirlpool::DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const
	{
		ValidateParameters (key, password, salt, iterationCount);
		derive_key_whirlpool ((char *) password.DataPtr(), (int) password.Size(), (char *) salt.Get(), (int) salt.Size(), iterationCount, (char *) key.Get(), (int) key.Size());
	}

	void Pkcs5HmacStribog::DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const
	{
		ValidateParameters (key, password, salt, iterationCount);
		derive_key_stribog ((char *) password.DataPtr(), (int) password.Size(), (char *) salt.Get(), (int) salt.Size(), iterationCount, (char *) key.Get(), (int) key.Size());
	}

	void Pkcs5HmacGostHash::DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const
	{
		ValidateParameters (key, password, salt, iterationCount);
		derive_key_gosthash ((char *) password.DataPtr(), (int) password.Size(), (char *) salt.Get(), (int) salt.Size(), iterationCount, (char *) key.Get(), (int) key.Size());
	}
}
}
