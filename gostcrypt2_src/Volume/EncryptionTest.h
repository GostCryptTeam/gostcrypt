/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_EncryptionTest
#define GST_HEADER_Encryption_EncryptionTest

#include "Platform/Platform.h"
#include "Common/Crypto.h"

namespace GostCrypt
{
namespace Volume {

	class EncryptionTest
	{
	public:
		static void TestAll ();
	protected:
		static void TestCiphers ();
		static void TestPkcs5 ();
		static void TestXts ();

	struct XtsTestVector
	{
		quint8 key1[32];
		quint8 key2[32];
		quint8 dataUnitNo[8];
		unsigned int blockNo;
		quint8 plaintext[ENCRYPTION_DATA_UNIT_SIZE];
		quint8 ciphertext[ENCRYPTION_DATA_UNIT_SIZE];
	};

	static const XtsTestVector XtsTestVectors[];

	private:
		EncryptionTest ();
		virtual ~EncryptionTest ();
		EncryptionTest (const EncryptionTest &);
		EncryptionTest &operator= (const EncryptionTest &);
	};
}
}

#endif // GST_HEADER_Encryption_EncryptionTest
