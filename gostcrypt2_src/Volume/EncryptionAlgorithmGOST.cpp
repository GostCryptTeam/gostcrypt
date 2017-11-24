#include "EncryptionAlgorithmGOST.h"
#include "EncryptionModeXTS.h"

namespace GostCrypt {
namespace Volume {
    // GOST
    EncryptionAlgorithmGOST::EncryptionAlgorithmGOST ()
    {
        Ciphers.push_back (QSharedPointer <CipherAlgorithm> (new CipherAlgorithmGOST ()));

        SupportedModes.push_back (QSharedPointer <EncryptionMode> (new EncryptionModeXTS ()));
    }

}
}
