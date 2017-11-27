#include "EncryptionAlgorithmGrasshopper.h"
#include "CipherAlgorithmGrasshopper.h"

namespace GostCrypt {
namespace Volume {
    //GRASSHOPPER
    EncryptionAlgorithmGrasshopper::EncryptionAlgorithmGrasshopper()
    {
        Ciphers.push_back (QSharedPointer <CipherAlgorithm> (new CipherAlgorithmGrasshopper ()));
        SupportedModes.push_back (QSharedPointer <EncryptionMode> (new EncryptionModeXTS ()));

    }

}
}
