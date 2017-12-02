/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#include "VolumeException.h"
#include "CipherAlgorithm.h"
#include "CipherAlgorithmGOST.h"
#include "CipherAlgorithmGrasshopper.h"

namespace GostCrypt
{
namespace Volume {

    CipherAlgorithm::CipherAlgorithm () : Initialized (false)
	{
	}

    CipherAlgorithm::~CipherAlgorithm ()
	{
	}

    void CipherAlgorithm::DecryptBlock (quint8 *data) const
	{
		if (!Initialized)
            throw CipherAlgorithmNotInitializedException();

		Decrypt (data);
	}

    void CipherAlgorithm::DecryptBlocks (quint8 *data, size_t blockCount) const
	{
		if (!Initialized)
            throw CipherAlgorithmNotInitializedException();

		while (blockCount-- > 0)
		{
			Decrypt (data);
			data += GetBlockSize();
		}
	}

    void CipherAlgorithm::EncryptBlock (quint8 *data) const
	{
		if (!Initialized)
            throw CipherAlgorithmNotInitializedException();

		Encrypt (data);
	}

    void CipherAlgorithm::EncryptBlocks (quint8 *data, size_t blockCount) const
	{
		if (!Initialized)
            throw CipherAlgorithmNotInitializedException();

		while (blockCount-- > 0)
		{
			Encrypt (data);
			data += GetBlockSize();
		}
	}

    CipherAlgorithmList CipherAlgorithm::GetAvailableCiphers ()
	{
        CipherAlgorithmList l;

        l.push_back (QSharedPointer <CipherAlgorithm> (new CipherAlgorithmGOST ()));
        l.push_back (QSharedPointer <CipherAlgorithm> (new CipherAlgorithmGrasshopper()));

		return l;
	}

    void CipherAlgorithm::SetKey (const ConstBufferPtr &key)
	{
		if (key.Size() != GetKeySize ())
            throw IncorrectParameterException("key.Size() != GetKeySize ()");

		if (!Initialized)
			ScheduledKey.Allocate (GetScheduledKeySize ());

		SetCipherKey (key);
		Key.CopyFrom (key);
		Initialized = true;
		KeySwapped = false;
	}

    void CipherAlgorithm::StoreCipherKey ()
	{
		SwapScheduledKey.CopyFrom ((ConstBufferPtr) ScheduledKey);
		KeySwapped = true;
	}

    void CipherAlgorithm::RestoreCipherKey ()
	{
		ScheduledKey.CopyFrom ((ConstBufferPtr) SwapScheduledKey);
		KeySwapped = false;
	}

    bool CipherAlgorithm::IsKeySwapped ()
	{
		return KeySwapped;
	}

    bool CipherAlgorithm::HwSupportEnabled = false;
}
}
