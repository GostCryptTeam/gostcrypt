#ifndef ENCRYPTIONALGORITHMGRASSHOPPER_H
#define ENCRYPTIONALGORITHMGRASSHOPPER_H

#include "EncryptionAlgorithm.h"
#include "EncryptionModeXTS.h"

namespace GostCrypt {
namespace Volume {

class EncryptionAlgorithm;

class EncryptionAlgorithmGrasshopper : public EncryptionAlgorithm
{
public:
    EncryptionAlgorithmGrasshopper ();
    virtual ~EncryptionAlgorithmGrasshopper () { }

    virtual shared_ptr <EncryptionAlgorithm> GetNew () const { return shared_ptr <EncryptionAlgorithm> (new EncryptionAlgorithmGrasshopper()); }

private:
    EncryptionAlgorithmGrasshopper (const EncryptionAlgorithmGrasshopper &);
    EncryptionAlgorithmGrasshopper &operator= (const EncryptionAlgorithmGrasshopper &);
};

}
}

#endif // ENCRYPTIONALGORITHMGOST_H
