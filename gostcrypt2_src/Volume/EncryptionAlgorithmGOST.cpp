#include "EncryptionAlgorithmGOST.h"
#include "EncryptionModeXTS.h"

namespace GostCrypt {
namespace Volume {
    // GOST
    EncryptionAlgorithmGOST::EncryptionAlgorithmGOST ()
    {
        Ciphers.push_back (shared_ptr <CipherAlgorithm> (new CipherAlgorithmGOST ()));

        SupportedModes.push_back (shared_ptr <EncryptionMode> (new EncryptionModeXTS ()));
    }

}
}
