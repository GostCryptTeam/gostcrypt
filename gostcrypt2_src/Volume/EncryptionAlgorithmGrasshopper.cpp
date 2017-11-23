#include "EncryptionAlgorithmGrasshopper.h"

namespace GostCrypt {
namespace Volume {
    //GRASSHOPPER
    EncryptionAlgorithmGrasshopper::EncryptionAlgorithmGrasshopper()
    {
        Ciphers.push_back (shared_ptr <Cipher> (new CipherGRASSHOPPER ()));
        SupportedModes.push_back (shared_ptr <EncryptionMode> (new EncryptionModeXTS ()));

    }

}
}
