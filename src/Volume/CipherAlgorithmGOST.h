#ifndef CIPHERALGORITHMGOST_H
#define CIPHERALGORITHMGOST_H

#include "CipherAlgorithm.h"

namespace GostCrypt
{
namespace Volume
{

/**
 * @brief The CipherAlgorithmGOST class implements the CipherAlgorithm class for GOST-89
 * It is mostly definitions and interfaces for the .c code.
 * @see GostCrypt::Volume::CipherAlgorithm
 */
class CipherAlgorithmGOST : public CipherAlgorithm
{
 public:
    CipherAlgorithmGOST() { }
    virtual ~CipherAlgorithmGOST() { }
    /**
     * @brief GetBlockSize returns GOST's block size
     */
    virtual size_t GetBlockSize() const { return 8; }

    /**
     * @brief GetKeySize returns GOST's Key size
     */
    virtual size_t GetKeySize() const { return 32; }

    /**
     * @brief GetName returns GOST's name
     */
    virtual QString GetName() const { return QStringLiteral("GOST 28147-89"); }

    /**
     * @brief GetDescription returns GOST's description
     */
    virtual QString GetDescription() const
    {
        return QStringLiteral("The GOST 28147-89 block cipher is part of "
                              "the Russian cryptographic standard algorithms. "
                              "It is a symmetric key block cipher based on a balanced "
                              "Feistel network.<br><font color='719c24'><a href='#'>"
                              "More information on GOST 28147-89</a></font>");
    }

    /**
     * @brief GetNew returns a new instance of CipherAlgorithmGOST casted to a CipherAlgorithm
     * @return a CipherAlgorithmGOST
     */
    virtual QSharedPointer <CipherAlgorithm> GetNew() const { return QSharedPointer <CipherAlgorithm> (new CipherAlgorithmGOST()); }

    /**
     * @brief custom implementation for GOST and XTS
     */
    virtual void XorCipherKey(quint8* ks, quint8* data, int len) const;

    /**
     * @brief custom implementation for GOST and XTS
     */
    virtual void EncryptWithKS(quint8* data, quint8* ks) const;

    /**
     * @brief custom implementation for GOST and XTS
     */
    virtual void DecryptWithKS(quint8* data, quint8* ks) const;

    /**
     * @brief custom implementation for GOST and XTS
     */
    virtual void CopyCipherKey(quint8* ks) const;

    /**
     * @brief GetScheduledKeySize returns the size of the structure used to store gost's key.
     * @return sizeof(gost_kds)
     */
    virtual size_t GetScheduledKeySize() const;

 protected:
    /**
     * @brief Decrypt decrypt a full block at index data. This is the only function that an algorithm has to implement to decrypt
     * @param data the data of size getBlockSize() to decrypt
     */
    virtual void Decrypt(quint8* data) const;

    /**
     * @brief Decrypt encrypt a full block at index data. This is the only function that an algorithm has to implement to encrypt
     * @param data the data of size getBlockSize() to encrypt
     */
    virtual void Encrypt(quint8* data) const;

    /**
     * @brief SetCipherKey sets the internal gost_kds structure from the passed key.
     * @param key the passed key of size GetKeySize()
     */
    virtual void SetCipherKey(const quint8* key);
 private:
    CipherAlgorithmGOST(const CipherAlgorithmGOST&);
    CipherAlgorithmGOST& operator= (const CipherAlgorithmGOST&);
};

}
}

#endif // CIPHERALGORITHMGOST_H
