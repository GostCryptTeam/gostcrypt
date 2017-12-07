#include "VolumeLayoutV2Normal.h"
#include "EncryptionAlgorithmGOST.h"
#include "EncryptionAlgorithmGrasshopper.h"

namespace GostCrypt {
namespace Volume {

VolumeLayoutV2Normal::VolumeLayoutV2Normal ()
{
    Type = VolumeType::Normal;
    HeaderOffset = GST_VOLUME_HEADER_OFFSET;
    HeaderSize = GST_VOLUME_HEADER_SIZE;
    BackupHeaderOffset = -GST_VOLUME_HEADER_GROUP_SIZE;

    SupportedEncryptionAlgorithms.push_back (QSharedPointer <EncryptionAlgorithm> (new EncryptionAlgorithmGOST (QSharedPointer <EncryptionMode>(new EncryptionModeXTS()))));
    SupportedEncryptionAlgorithms.push_back (QSharedPointer <EncryptionAlgorithm> (new EncryptionAlgorithmGrasshopper(QSharedPointer <EncryptionMode>(new EncryptionModeXTS()))));

    SupportedEncryptionModes.push_back (QSharedPointer <EncryptionMode> (new EncryptionModeXTS ()));
}

quint64 VolumeLayoutV2Normal::GetDataOffset (quint64 volumeHostSize) const
{
    (void)volumeHostSize;
    return Header->GetEncryptedAreaStart();
}

quint64 VolumeLayoutV2Normal::GetDataSize (quint64 volumeHostSize) const
{
    (void)volumeHostSize;
    return Header->GetVolumeDataSize();
}

quint64 VolumeLayoutV2Normal::GetMaxDataSize (quint64 volumeSize) const
{
    if (volumeSize < GST_TOTAL_VOLUME_HEADERS_SIZE)
        return 0;

    return volumeSize - GST_TOTAL_VOLUME_HEADERS_SIZE;
}

}
}
