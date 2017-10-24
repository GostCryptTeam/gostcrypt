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

	Cipher::Cipher () : Initialized (false)
	{
	}

	Cipher::~Cipher ()
	{
	}

	void Cipher::DecryptBlock (byte *data) const
	{
		if (!Initialized)
			throw NotInitialized (SRC_POS);

		Decrypt (data);
	}

	void Cipher::DecryptBlocks (byte *data, size_t blockCount) const
	{
		if (!Initialized)
			throw NotInitialized (SRC_POS);

		while (blockCount-- > 0)
		{
			Decrypt (data);
			data += GetBlockSize();
		}
	}

	void Cipher::EncryptBlock (byte *data) const
	{
		if (!Initialized)
			throw NotInitialized (SRC_POS);

		Encrypt (data);
	}

	void Cipher::EncryptBlocks (byte *data, size_t blockCount) const
	{
		if (!Initialized)
			throw NotInitialized (SRC_POS);

		while (blockCount-- > 0)
		{
			Encrypt (data);
			data += GetBlockSize();
		}
	}

	CipherList Cipher::GetAvailableCiphers ()
	{
		CipherList l;

		l.push_back (shared_ptr <Cipher> (new CipherGOST ()));
		l.push_back (shared_ptr <Cipher> (new CipherGRASSHOPPER()));

		return l;
	}

	void Cipher::SetKey (const ConstBufferPtr &key)
	{
		if (key.Size() != GetKeySize ())
			throw ParameterIncorrect (SRC_POS);

		if (!Initialized)
			ScheduledKey.Allocate (GetScheduledKeySize ());

		SetCipherKey (key);
		Key.CopyFrom (key);
		Initialized = true;
		KeySwapped = false;
	}

	void Cipher::StoreCipherKey ()
	{
		SwapScheduledKey.CopyFrom ((ConstBufferPtr) ScheduledKey);
		KeySwapped = true;
	}

	void Cipher::RestoreCipherKey ()
	{
		ScheduledKey.CopyFrom ((ConstBufferPtr) SwapScheduledKey);
		KeySwapped = false;
	}

	bool Cipher::IsKeySwapped ()
	{
		return KeySwapped;
	}

#define GST_EXCEPTION(TYPE) GST_SERIALIZER_FACTORY_ADD(TYPE)
#undef GST_EXCEPTION_NODECL
#define GST_EXCEPTION_NODECL(TYPE) GST_SERIALIZER_FACTORY_ADD(TYPE)

	GST_SERIALIZER_FACTORY_ADD_EXCEPTION_SET (CipherException);

	// GOST
	void CipherGOST::Decrypt (byte *data) const
	{
		gost_decrypt (data, data, (gost_kds *) ScheduledKey.Ptr());
	}

	void CipherGOST::Encrypt (byte *data) const
	{
		gost_encrypt (data, data, (gost_kds *) ScheduledKey.Ptr());
	}

	void CipherGOST::DecryptWithKS (byte *data, byte *ks) const
	{
		gost_decrypt (data, data, (gost_kds *) ks);
	}

	void CipherGOST::EncryptWithKS (byte *data, byte *ks) const
	{
		gost_encrypt (data, data, (gost_kds *) ks);
	}

	size_t CipherGOST::GetScheduledKeySize () const
	{
		return sizeof(gost_kds);
	}

	void CipherGOST::SetCipherKey (const byte *key)
	{
		gost_set_key ((byte *)key, (gost_kds *)ScheduledKey.Ptr());
	}

	void CipherGOST::XorCipherKey (byte *ks, byte *data, int len) const
	{
		gost_xor_ks((gost_kds *) ScheduledKey.Ptr(), (gost_kds *) ks, (unsigned int *)data, len / sizeof(unsigned int));
	}

	void CipherGOST::CopyCipherKey (byte *ks) const
	{
		size_t i;
		size_t size = ScheduledKey.Size();
		byte *ptr = ScheduledKey.Ptr();
		for (i = 0; i < size; i++)
			ks[i] = ptr[i];
	}

	// GRASSHOPPER
	void CipherGRASSHOPPER::Decrypt (byte *data) const
	{
		grasshopper_decrypt ((grasshopper_kds*) ScheduledKey.Ptr(), (gst_ludword *) data, (gst_ludword *) data);
	}

	void CipherGRASSHOPPER::Encrypt (byte *data) const
	{
		grasshopper_encrypt ((grasshopper_kds*) ScheduledKey.Ptr(), (gst_ludword *) data, (gst_ludword *) data);
	}

	void CipherGRASSHOPPER::DecryptWithKS (byte *data, byte *ks) const
	{
		grasshopper_decrypt ((grasshopper_kds*) ks, (gst_ludword *) data, (gst_ludword *) data);
	}

	void CipherGRASSHOPPER::EncryptWithKS (byte *data, byte *ks) const
	{
		grasshopper_encrypt ((grasshopper_kds*) ks, (gst_ludword *) data, (gst_ludword *) data);
	}

	size_t CipherGRASSHOPPER::GetScheduledKeySize () const
	{
		return sizeof(grasshopper_kds);
	}

	void CipherGRASSHOPPER::SetCipherKey (const byte *key)
	{
		grasshopper_set_key((gst_ludword*)key, (grasshopper_kds *) ScheduledKey.Ptr());
	}

	void CipherGRASSHOPPER::XorCipherKey (byte *ks, byte *data, int len) const
	{
		grasshopper_xor_ks((grasshopper_kds *) ScheduledKey.Ptr(), (grasshopper_kds *) ks, (gst_ludword *)data, len / sizeof(unsigned long long));
	}

	void CipherGRASSHOPPER::CopyCipherKey (byte *ks) const
	{
		size_t i;
		size_t size = ScheduledKey.Size();
		byte *ptr = ScheduledKey.Ptr();
		for (i = 0; i < size; i++)
			ks[i] = ptr[i];
	}
	bool Cipher::HwSupportEnabled = false;
}
}
