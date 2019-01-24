#ifndef CIPHERALGORITHMGRASSHOPPER_H
#define CIPHERALGORITHMGRASSHOPPER_H

#include "CipherAlgorithm.h"

namespace GostCrypt
{
namespace Volume
{

/**
 * @brief The CipherAlgorithmGrasshopper class implements the CipherAlgorithm class for GOST Grasshopper
 * It is mostly definitions and interfaces for the .c code.
 * @see GostCrypt::Volume::CipherAlgorithm
 */
class CipherAlgorithmGrasshopper : public CipherAlgorithm
{
 public:
    CipherAlgorithmGrasshopper() { }
    virtual ~CipherAlgorithmGrasshopper() { }
    /**
     * @brief GetBlockSize returns Grasshopper's block size
     */
    virtual size_t GetBlockSize() const { return 8; }

    /**
     * @brief GetKeySize returns Grasshopper's Key size
     */
    virtual size_t GetKeySize() const { return 32; }

    /**
     * @brief GetName returns Grasshopper's name
     */
    virtual QString GetName() const { return QStringLiteral("Gost Grasshopper"); }

    /**
     * @brief GetDescription returns Grasshopper's description
     */
    virtual QString GetDescription() const { return QStringLiteral("No description for Gost Grasshopper."); }

    /**
     * @brief GetNew returns a new instance of CipherAlgorithmGrasshopper casted to a CipherAlgorithm
     * @return a CipherAlgorithmGrasshopper
     */
    virtual QSharedPointer <CipherAlgorithm> GetNew() const { return QSharedPointer <CipherAlgorithm> (new CipherAlgorithmGrasshopper()); }

    /**
     * @brief custom implementation for Grasshopper and XTS
     */
    virtual void XorCipherKey(quint8* ks, quint8* data, int len) const;

    /**
     * @brief custom implementation for Grasshopper and XTS
     */
    virtual void EncryptWithKS(quint8* data, quint8* ks) const;

    /**
     * @brief custom implementation for Grasshopper and XTS
     */
    virtual void DecryptWithKS(quint8* data, quint8* ks) const;

    /**
     * @brief custom implementation for Grasshopper and XTS
     */
    virtual void CopyCipherKey(quint8* ks) const;

    /**
     * @brief GetScheduledKeySize returns the size of the structure used to store Grasshopper's key.
     * @return sizeof(grasshopper_kds)
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
     * @brief SetCipherKey sets the internal grasshopper_kds structure from the passed key.
     * @param key the passed key of size GetKeySize()
     */
    virtual void SetCipherKey(const quint8* key);
 private:
    CipherAlgorithmGrasshopper(const CipherAlgorithmGrasshopper&);
    CipherAlgorithmGrasshopper& operator= (const CipherAlgorithmGrasshopper&);
};

}
}

#endif // CIPHERALGORITHMGRASSHOPPER_H
