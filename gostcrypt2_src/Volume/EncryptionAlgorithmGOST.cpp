#include "EncryptionAlgorithmGOST.h"
#include "EncryptionModeXTS.h"

namespace GostCrypt {
namespace Volume {
    // GOST
    EncryptionAlgorithmGOST::EncryptionAlgorithmGOST ()
    {
        Ciphers.push_back (shared_ptr <Cipher> (new CipherGOST ()));

        SupportedModes.push_back (shared_ptr <EncryptionMode> (new EncryptionModeXTS ()));
    }

}
}
