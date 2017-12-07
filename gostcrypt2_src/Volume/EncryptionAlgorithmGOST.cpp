#include "EncryptionAlgorithmGOST.h"
#include "EncryptionModeXTS.h"
#include "CipherAlgorithmGOST.h"

namespace GostCrypt {
namespace Volume {
    // GOST
    EncryptionAlgorithmGOST::EncryptionAlgorithmGOST (QSharedPointer <EncryptionMode> mode)
    {
        CipherAlgorithmList ciphers;
        ciphers.push_back (QSharedPointer <CipherAlgorithm> (new CipherAlgorithmGOST ()));

        this->Mode = mode;
        this->Mode->SetCiphers(ciphers);
    }

}
}
