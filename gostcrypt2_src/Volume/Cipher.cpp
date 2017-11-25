/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "Platform/Platform.h"
#include "Cipher.h"
#include "Crypto/GostCipher.h"
#include "Crypto/GrasshopperCipher.h"


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
            throw;// NotInitialized (SRC_POS);

		Decrypt (data);
	}

    void CipherAlgorithm::DecryptBlocks (quint8 *data, size_t blockCount) const
	{
		if (!Initialized)
            throw;// NotInitialized (SRC_POS);

		while (blockCount-- > 0)
		{
			Decrypt (data);
			data += GetBlockSize();
		}
	}

    void CipherAlgorithm::EncryptBlock (quint8 *data) const
	{
		if (!Initialized)
            throw;// NotInitialized (SRC_POS);

		Encrypt (data);
	}

    void CipherAlgorithm::EncryptBlocks (quint8 *data, size_t blockCount) const
	{
		if (!Initialized)
            throw;// NotInitialized (SRC_POS);

		while (blockCount-- > 0)
		{
			Encrypt (data);
			data += GetBlockSize();
		}
	}

    CipherList CipherAlgorithm::GetAvailableCiphers ()
	{
		CipherList l;

        l.push_back (QSharedPointer <CipherAlgorithm> (new CipherAlgorithmGOST ()));
        l.push_back (QSharedPointer <CipherAlgorithm> (new CipherAlgorithmGRASSHOPPER()));

		return l;
	}

    void CipherAlgorithm::SetKey (const ConstBufferPtr &key)
	{
		if (key.Size() != GetKeySize ())
            throw;// ParameterIncorrect (SRC_POS);

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

	// GOST
    void CipherAlgorithmGOST::Decrypt (quint8 *data) const
	{
		gost_decrypt (data, data, (gost_kds *) ScheduledKey.Ptr());
	}

    void CipherAlgorithmGOST::Encrypt (quint8 *data) const
	{
		gost_encrypt (data, data, (gost_kds *) ScheduledKey.Ptr());
	}

    void CipherAlgorithmGOST::DecryptWithKS (quint8 *data, quint8 *ks) const
	{
		gost_decrypt (data, data, (gost_kds *) ks);
	}

    void CipherAlgorithmGOST::EncryptWithKS (quint8 *data, quint8 *ks) const
	{
		gost_encrypt (data, data, (gost_kds *) ks);
	}

    size_t CipherAlgorithmGOST::GetScheduledKeySize () const
	{
		return sizeof(gost_kds);
	}

    void CipherAlgorithmGOST::SetCipherKey (const quint8 *key)
	{
		gost_set_key ((quint8 *)key, (gost_kds *)ScheduledKey.Ptr());
	}

    void CipherAlgorithmGOST::XorCipherKey (quint8 *ks, quint8 *data, int len) const
	{
		gost_xor_ks((gost_kds *) ScheduledKey.Ptr(), (gost_kds *) ks, (unsigned int *)data, len / sizeof(unsigned int));
	}

    void CipherAlgorithmGOST::CopyCipherKey (quint8 *ks) const
	{
		size_t i;
		size_t size = ScheduledKey.Size();
		quint8 *ptr = ScheduledKey.Ptr();
		for (i = 0; i < size; i++)
			ks[i] = ptr[i];
	}

	// GRASSHOPPER
    void CipherAlgorithmGRASSHOPPER::Decrypt (quint8 *data) const
	{
		grasshopper_decrypt ((grasshopper_kds*) ScheduledKey.Ptr(), (gst_ludword *) data, (gst_ludword *) data);
	}

    void CipherAlgorithmGRASSHOPPER::Encrypt (quint8 *data) const
	{
		grasshopper_encrypt ((grasshopper_kds*) ScheduledKey.Ptr(), (gst_ludword *) data, (gst_ludword *) data);
	}

    void CipherAlgorithmGRASSHOPPER::DecryptWithKS (quint8 *data, quint8 *ks) const
	{
		grasshopper_decrypt ((grasshopper_kds*) ks, (gst_ludword *) data, (gst_ludword *) data);
	}

    void CipherAlgorithmGRASSHOPPER::EncryptWithKS (quint8 *data, quint8 *ks) const
	{
		grasshopper_encrypt ((grasshopper_kds*) ks, (gst_ludword *) data, (gst_ludword *) data);
	}

    size_t CipherAlgorithmGRASSHOPPER::GetScheduledKeySize () const
	{
		return sizeof(grasshopper_kds);
	}

    void CipherAlgorithmGRASSHOPPER::SetCipherKey (const quint8 *key)
	{
		grasshopper_set_key((gst_ludword*)key, (grasshopper_kds *) ScheduledKey.Ptr());
	}

    void CipherAlgorithmGRASSHOPPER::XorCipherKey (quint8 *ks, quint8 *data, int len) const
	{
		grasshopper_xor_ks((grasshopper_kds *) ScheduledKey.Ptr(), (grasshopper_kds *) ks, (gst_ludword *)data, len / sizeof(unsigned long long));
	}

    void CipherAlgorithmGRASSHOPPER::CopyCipherKey (quint8 *ks) const
	{
		size_t i;
		size_t size = ScheduledKey.Size();
		quint8 *ptr = ScheduledKey.Ptr();
		for (i = 0; i < size; i++)
			ks[i] = ptr[i];
	}
    bool CipherAlgorithm::HwSupportEnabled = false;
}
}
