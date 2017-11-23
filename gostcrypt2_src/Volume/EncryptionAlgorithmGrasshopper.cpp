#include "EncryptionAlgorithmGrasshopper.h"

namespace GostCrypt {
namespace Volume {
    //GRASSHOPPER
    EncryptionAlgorithmGrasshopper::EncryptionAlgorithmGrasshopper()
    {
        Ciphers.push_back (shared_ptr <CipherAlgorithm> (new CipherAlgorithmGRASSHOPPER ()));
        SupportedModes.push_back (shared_ptr <EncryptionMode> (new EncryptionModeXTS ()));

    }

}
}
