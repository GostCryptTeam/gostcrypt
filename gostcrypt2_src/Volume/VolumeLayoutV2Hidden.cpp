#include "VolumeLayoutV2Hidden.h"
#include "EncryptionAlgorithmGOST.h"
#include "EncryptionAlgorithmGrasshopper.h"

namespace GostCrypt {
namespace Volume {

VolumeLayoutV2Hidden::VolumeLayoutV2Hidden ()
{
    Type = VolumeType::Hidden;
    HeaderOffset = GST_HIDDEN_VOLUME_HEADER_OFFSET;
    HeaderSize = GST_VOLUME_HEADER_SIZE;
    BackupHeaderOffset = -GST_HIDDEN_VOLUME_HEADER_OFFSET;

    SupportedEncryptionAlgorithms.push_back (QSharedPointer <EncryptionAlgorithm> (new EncryptionAlgorithmGOST ()));
    SupportedEncryptionAlgorithms.push_back (QSharedPointer <EncryptionAlgorithm> (new EncryptionAlgorithmGrasshopper()));

    SupportedEncryptionModes.push_back (QSharedPointer <EncryptionMode> (new EncryptionModeXTS ()));
}

quint64 VolumeLayoutV2Hidden::GetDataOffset (quint64 volumeHostSize) const
{
    (void)volumeHostSize;
    return Header->GetEncryptedAreaStart();
}

quint64 VolumeLayoutV2Hidden::GetDataSize (quint64 volumeHostSize) const
{
    (void)volumeHostSize;
    return Header->GetVolumeDataSize();
}

quint64 VolumeLayoutV2Hidden::GetMaxDataSize (quint64 volumeSize) const
{
    // Reserve free space at the end of the host filesystem
    quint64 reservedSize;

    if (volumeSize < GST_VOLUME_SMALL_SIZE_THRESHOLD)
        reservedSize = GST_HIDDEN_VOLUME_HOST_FS_RESERVED_END_AREA_SIZE;
    else
        reservedSize = GST_HIDDEN_VOLUME_HOST_FS_RESERVED_END_AREA_SIZE_HIGH; // Ensure size of a hidden volume larger than GST_VOLUME_SMALL_SIZE_THRESHOLD is a multiple of the maximum supported sector size

    if (volumeSize < reservedSize)
        return 0;

    return volumeSize - reservedSize;
}

}
}
