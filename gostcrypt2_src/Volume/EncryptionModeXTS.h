/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Volume_EncryptionModeXTS
#define GST_HEADER_Volume_EncryptionModeXTS

#include "EncryptionMode.h"

namespace GostCrypt
{
namespace Volume
{

/**
 * @brief The EncryptionModeXTS class implements the class EncryptionMode.
 * XTS is the default and only implemented mode for now.
 */
class EncryptionModeXTS : public EncryptionMode
{
 public:
    EncryptionModeXTS() { }
    virtual ~EncryptionModeXTS() { }

    /**
     * @brief The implementation of this method just sets the unit number to 0.
     * @param data pointer to the data to decrypt
     * @param length size of the data to decrypt
     * @warning This function should only be used in specific cases where the position of the data is not relevant. Use DecryptSectors instead.
     */
    virtual void Decrypt(quint8* data, quint64 length) const;

    /**
     * @brief DecryptSectorsCurrentThread's XTS implementation will treat the group of sectors as a huge sector and decrypt it.
     * @todo The function might work differently if called on a whole group of sectors or multiple times on each sector of the group. Is this normal ?
     * @warning Will not work on sector sizes not divisibles by 512
     */
    virtual void DecryptSectorsCurrentThread(quint8* data, quint64 sectorIndex, quint64 sectorCount,
            size_t sectorSize) const;

    /**
     * @brief The implementation of this method just sets the unit number to 0.
     * @param data pointer to the data to encrypt
     * @param length size of the data to encrypt
     * @warning This function should only be used in specific cases where the position of the data is not relevant. Use DecryptSectors instead.
     */
    virtual void Encrypt(quint8* data, quint64 length) const;

    /**
     * @brief EncryptSectorsCurrentThread's XTS implementation will treat the group of sectors as a huge sector and decrypt it.
     * @todo The function might work differently if called on a whole group of sectors or multiple times on each sector of the group. Is this normal ?
     * @warning Will not work on sector sizes not divisibles by 512
     */
    virtual void EncryptSectorsCurrentThread(quint8* data, quint64 sectorIndex, quint64 sectorCount,
            size_t sectorSize) const;

    /**
     * @brief GetKeySize returns the sum of the Cipher's sizes
     * @return the key size for the list of ciphers
     */
    virtual size_t GetKeySize() const;

    /**
     * @brief GetName
     * @return XTS's name
     */
    virtual QString GetName() const { return QStringLiteral("XTS"); }

    /**
     * @brief GetNew returns a new EncryptionModeXTS without Ciphers
     * @return a new clean EncryptionModeXTS as an EncryptionMode
     */
    virtual QSharedPointer <EncryptionMode> GetNew() const { return QSharedPointer <EncryptionMode> (new EncryptionModeXTS); }

    /**
     * @brief SetCiphers sets the Ciphers (just like EncryptionMode), but also sets the Secondary Ciphers.
     * XTS mode uses two algorithm lists for encryption. But from this implementation we can tell that the two lists are the same.
     * @todo This whole class must be rewritten, DecryptBufferXTS and EncryptBufferXTS8Byte look too much the same and we don't need SecondaryCiphers since they're the same as Ciphers.
     * @param ciphers the Ciphers to use as both primary and secondary.
     */
    virtual void SetCiphers(const CipherAlgorithmList& ciphers);

    /**
     * @brief SetKey cuts the key in parts and applies them to the Ciphers
     * @param key the combination of the keys to use
     */
    virtual void SetKey(const BufferPtr& key);

 protected:

    /**
     * @brief DecryptBuffer custom decrypt function for XTS.
     * Will loop over the Ciphers and call either DecryptBufferXTS of DecryptBufferXTS8Byte
     */
    void DecryptBuffer(quint8* data, quint64 length, quint64 startDataUnitNo) const;

    /**
     * @brief DecryptBufferXTS is XTS's implementation for decrypting
     */
    static void DecryptBufferXTS(const CipherAlgorithm& cipher, const CipherAlgorithm& secondaryCipher,
                                 quint8* buffer, quint64 length, quint64 startDataUnitNo, unsigned int startCipherBlockNo);

    /**
     * @brief DecryptBufferXTS8Byte is XTS's implementation for 8Byte Ciphers.
     * @todo Might be cleaner to fusion the 8Byte and the default function.
     */
    static void DecryptBufferXTS8Byte(const CipherAlgorithm& cipher,
                                      const CipherAlgorithm& secondaryCipher, quint8* buffer, quint64 length, quint64 startDataUnitNo,
                                      unsigned int startCipherBlockNo);

    /**
     * @brief EncryptBuffer custom decrypt function for XTS.
     * Will loop over the Ciphers and call either EncryptBufferXTS of EncryptBufferXTS8Byte
     */
    void EncryptBuffer(quint8* data, quint64 length, quint64 startDataUnitNo) const;

    /**
     * @brief EncryptBufferXTS is XTS's implementation for encrypting
     */
    static void EncryptBufferXTS(const CipherAlgorithm& cipher, const CipherAlgorithm& secondaryCipher,
                                 quint8* buffer, quint64 length, quint64 startDataUnitNo, unsigned int startCipherBlockNo);

    /**
     * @brief EncryptBufferXTS8Byte is XTS's implementation for 8Byte Ciphers.
     * @todo Might be cleaner to fusion the 8Byte and the default function.
     */
    static void EncryptBufferXTS8Byte(const CipherAlgorithm& cipher,
                                      const CipherAlgorithm& secondaryCipher, quint8* buffer, quint64 length, quint64 startDataUnitNo,
                                      unsigned int startCipherBlockNo);

    /**
     * @brief SetSecondaryCipherKeys sets the keys for the Secondary alggorithms only
     * Keys for Ciphers are never set. They are set in the EncryptionAlgorithm class
     * To define the environnement, you must set the Primary keys like encryptionAlgo->SetKey()
     * and then the secondary keys with encryptionAlgo->getMode()->SetKey()
     * @todo This is very unpractical...
     */
    void SetSecondaryCipherKeys();

    /**
     * @brief SecondaryKey is the key for the Mode
     * @todo Once the keys are set, this variable is completely useless.
     */
    SecureBuffer SecondaryKey;

    /**
     * @brief SecondaryCiphers are the Ciphers used for the mode
     * Opposed to "Ciphers" the ciphers of the algorithm.
     * Ultimately, they are set to the same value, only the keys may change between Secondary and primary Ciphers.
     */
    CipherAlgorithmList SecondaryCiphers;

 private:
    EncryptionModeXTS(const EncryptionModeXTS&);
    EncryptionModeXTS& operator= (const EncryptionModeXTS&);
};
}
}

#endif // GST_HEADER_Volume_EncryptionModeXTS
