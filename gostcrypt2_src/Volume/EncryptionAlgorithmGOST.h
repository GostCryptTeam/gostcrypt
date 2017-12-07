#ifndef ENCRYPTIONALGORITHMGOST_H
#define ENCRYPTIONALGORITHMGOST_H

#include "EncryptionAlgorithm.h"

namespace GostCrypt {
namespace Volume {

class EncryptionAlgorithmGOST : public EncryptionAlgorithm
{
public:
    EncryptionAlgorithmGOST (QSharedPointer <EncryptionMode>);
    virtual ~EncryptionAlgorithmGOST () { }

    virtual QSharedPointer <EncryptionAlgorithm> GetNew () const { return QSharedPointer <EncryptionAlgorithm> (new EncryptionAlgorithmGOST(this->Mode->GetNew())); }

private:
    EncryptionAlgorithmGOST (const EncryptionAlgorithmGOST &);
    EncryptionAlgorithmGOST &operator= (const EncryptionAlgorithmGOST &);
};

}
}

#endif // ENCRYPTIONALGORITHMGOST_H
