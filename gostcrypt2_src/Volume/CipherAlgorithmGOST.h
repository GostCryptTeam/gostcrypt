#ifndef CIPHERALGORITHMGOST_H
#define CIPHERALGORITHMGOST_H

#include "CipherAlgorithm.h"

namespace GostCrypt
{
namespace Volume
{

class CipherAlgorithmGOST : public CipherAlgorithm
{
 public:
    CipherAlgorithmGOST() { }
    virtual ~CipherAlgorithmGOST() { }

    virtual size_t GetBlockSize() const { return 8; }
    virtual size_t GetKeySize() const { return 32; }
    virtual QString GetName() const { return QStringLiteral("GOST 28147-89"); }
    virtual QString GetDescription() const
    {
        return QStringLiteral("The GOST 28147-89 block cipher is part of "
                              "the Russian cryptographic standard algorithms. "
                              "It is a symmetric key block cipher based on a balanced "
                              "Feistel network.<br><font color='719c24'><a href='#'>"
                              "More information on GOST 28147-89</a></font>");
    }
    virtual QSharedPointer <CipherAlgorithm> GetNew() const { return QSharedPointer <CipherAlgorithm> (new CipherAlgorithmGOST()); }

    virtual void XorCipherKey(quint8* ks, quint8* data, int len) const;
    virtual void EncryptWithKS(quint8* data, quint8* ks) const;
    virtual void DecryptWithKS(quint8* data, quint8* ks) const;
    virtual void CopyCipherKey(quint8* ks) const;
    virtual size_t GetScheduledKeySize() const;

 protected:
    virtual void Decrypt(quint8* data) const;
    virtual void Encrypt(quint8* data) const;
    virtual void SetCipherKey(const quint8* key);
 private:
    CipherAlgorithmGOST(const CipherAlgorithmGOST&);
    CipherAlgorithmGOST& operator= (const CipherAlgorithmGOST&);
};

}
}

#endif // CIPHERALGORITHMGOST_H
