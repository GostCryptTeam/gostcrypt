/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_EncryptionTest
#define GST_HEADER_Encryption_EncryptionTest

#include "Crypto/Crypto.h"

namespace GostCrypt
{
namespace Volume
{

class EncryptionTest
{
 public:
    /**
     * @brief TestCiphers will test GOST's cipher with standard vectors.
     * @todo What about Grasshopper ...
     */
    static void TestCiphers();

    /**
     * @brief TestPkcs5 will test all derivation functions.
     */
    static void TestPkcs5();

    /**
     * @brief TestXts will test all algorihthms with XTS encryption mode.
     * @todo Not very scalable. Test vectors could be inside the EncryptionAlgorithm object for example.
     */
    static void TestXts();
 protected:

    /**
     * @brief The XtsTestVector struct defines a vector that can be used to test a pass of XTS.
     */
    struct XtsTestVector
    {
        quint8 key1[32];
        quint8 key2[32];
        quint8 dataUnitNo[8];
        unsigned int blockNo;
        quint8 plaintext[ENCRYPTION_DATA_UNIT_SIZE];
        quint8 ciphertext[ENCRYPTION_DATA_UNIT_SIZE];
    };

    /**
     * @brief XtsTestVectors stores a few IEEE vectors to test XTS
     */
    static const XtsTestVector XtsTestVectors[];

 private:
    EncryptionTest();
    virtual ~EncryptionTest();
    EncryptionTest(const EncryptionTest&);
    EncryptionTest& operator= (const EncryptionTest&);
};
}
}

#endif // GST_HEADER_Encryption_EncryptionTest
