#include "EncryptionAlgorithmGrasshopper.h"
#include "CipherAlgorithmGrasshopper.h"

namespace GostCrypt
{
namespace Volume
{
//GRASSHOPPER
EncryptionAlgorithmGrasshopper::EncryptionAlgorithmGrasshopper(QSharedPointer <EncryptionMode> mode)
{
    CipherAlgorithmList ciphers;
    ciphers.push_back(QSharedPointer <CipherAlgorithm> (new CipherAlgorithmGrasshopper()));

    this->Mode = mode;
    this->Mode->SetCiphers(ciphers);
}

}
}
