#ifndef CIPHERALGORITHMGRASSHOPPER_H
#define CIPHERALGORITHMGRASSHOPPER_H

#include "CipherAlgorithm.h"

namespace GostCrypt {
namespace Volume {

class CipherAlgorithmGrasshopper : public CipherAlgorithm
{
public:
    CipherAlgorithmGrasshopper () { }
    virtual ~CipherAlgorithmGrasshopper () { }

    virtual size_t GetBlockSize () const { return 8; }
    virtual size_t GetKeySize () const { return 32; }
    virtual std::wstring GetName () const { return L"Gost Grasshopper"; }
    virtual std::wstring GetDescription () const { return L"No description for Gost Grasshopper."; }
    virtual QSharedPointer <CipherAlgorithm> GetNew () const { return QSharedPointer <CipherAlgorithm> (new CipherAlgorithmGrasshopper()); }

    virtual void XorCipherKey (quint8 *ks, quint8 *data, int len) const;
    virtual void EncryptWithKS (quint8 *data, quint8 *ks) const;
    virtual void DecryptWithKS (quint8 *data, quint8 *ks) const;
    virtual void CopyCipherKey (quint8 *ks) const;
    virtual size_t GetScheduledKeySize () const;

protected:
    virtual void Decrypt (quint8 *data) const;
    virtual void Encrypt (quint8 *data) const;
    virtual void SetCipherKey (const quint8 *key);
private:
    CipherAlgorithmGrasshopper (const CipherAlgorithmGrasshopper &);
    CipherAlgorithmGrasshopper &operator= (const CipherAlgorithmGrasshopper &);
    };

}
}

#endif // CIPHERALGORITHMGRASSHOPPER_H
