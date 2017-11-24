#include "EncryptionAlgorithmGrasshopper.h"

namespace GostCrypt {
namespace Volume {
    //GRASSHOPPER
    EncryptionAlgorithmGrasshopper::EncryptionAlgorithmGrasshopper()
    {
        Ciphers.push_back (QSharedPointer <CipherAlgorithm> (new CipherAlgorithmGRASSHOPPER ()));
        SupportedModes.push_back (QSharedPointer <EncryptionMode> (new EncryptionModeXTS ()));

    }

}
}
