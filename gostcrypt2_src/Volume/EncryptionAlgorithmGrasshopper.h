#ifndef ENCRYPTIONALGORITHMGRASSHOPPER_H
#define ENCRYPTIONALGORITHMGRASSHOPPER_H

#include "EncryptionAlgorithm.h"
#include "EncryptionModeXTS.h"

namespace GostCrypt
{
namespace Volume
{

class EncryptionAlgorithm;

class EncryptionAlgorithmGrasshopper : public EncryptionAlgorithm
{
 public:
    EncryptionAlgorithmGrasshopper(QSharedPointer <EncryptionMode> mode);
    virtual ~EncryptionAlgorithmGrasshopper() { }

    virtual QSharedPointer <EncryptionAlgorithm> GetNew() const { return QSharedPointer <EncryptionAlgorithm> (new EncryptionAlgorithmGrasshopper(this->Mode->GetNew())); }

 private:
    EncryptionAlgorithmGrasshopper(const EncryptionAlgorithmGrasshopper&);
    EncryptionAlgorithmGrasshopper& operator= (const EncryptionAlgorithmGrasshopper&);
};

}
}

#endif // ENCRYPTIONALGORITHMGOST_H
