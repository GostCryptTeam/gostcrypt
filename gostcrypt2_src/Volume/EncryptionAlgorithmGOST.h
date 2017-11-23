#ifndef ENCRYPTIONALGORITHMGOST_H
#define ENCRYPTIONALGORITHMGOST_H

#include "EncryptionAlgorithm.h"

namespace GostCrypt {
namespace Volume {

class EncryptionAlgorithmGOST : public EncryptionAlgorithm
{
public:
    EncryptionAlgorithmGOST ();
    virtual ~EncryptionAlgorithmGOST () { }

    virtual shared_ptr <EncryptionAlgorithm> GetNew () const { return shared_ptr <EncryptionAlgorithm> (new EncryptionAlgorithmGOST()); }

private:
    EncryptionAlgorithmGOST (const EncryptionAlgorithmGOST &);
    EncryptionAlgorithmGOST &operator= (const EncryptionAlgorithmGOST &);
};

}
}

#endif // ENCRYPTIONALGORITHMGOST_H
