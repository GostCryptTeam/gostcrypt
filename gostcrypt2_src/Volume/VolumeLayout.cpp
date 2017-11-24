/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "Volume/EncryptionMode.h"
#include "Volume/EncryptionModeXTS.h"
#include "VolumeLayout.h"
#include "EncryptionAlgorithmGOST.h"
#include "EncryptionAlgorithmGrasshopper.h"

namespace GostCrypt
{
namespace Volume {

	VolumeLayout::VolumeLayout ()
	{
	}

	VolumeLayout::~VolumeLayout ()
	{
	}

	VolumeLayoutList VolumeLayout::GetAvailableLayouts (VolumeType::Enum type)
	{
		VolumeLayoutList layouts;

		layouts.push_back (QSharedPointer <VolumeLayout> (new VolumeLayoutV2Normal ()));
		layouts.push_back (QSharedPointer <VolumeLayout> (new VolumeLayoutV1Normal ()));
		layouts.push_back (QSharedPointer <VolumeLayout> (new VolumeLayoutV2Hidden ()));
		layouts.push_back (QSharedPointer <VolumeLayout> (new VolumeLayoutV1Hidden ()));

		if (type != VolumeType::Unknown)
		{
			VolumeLayoutList l;

			foreach (QSharedPointer <VolumeLayout> vl, layouts)
			{
				if (vl->GetType() == type)
					l.push_back (vl);
			}

			layouts = l;
		}

		return layouts;
	}

    QSharedPointer <VolumeHeader> VolumeLayout::GetHeader ()
	{
        if (Header.isNull())
			Header.reset (new VolumeHeader (GetHeaderSize()));

		return Header;
	}


	VolumeLayoutV1Normal::VolumeLayoutV1Normal ()
	{
		Type = VolumeType::Normal;
		HeaderOffset = GST_VOLUME_HEADER_OFFSET;
		HeaderSize = GST_VOLUME_HEADER_SIZE_LEGACY;

        SupportedEncryptionAlgorithms.push_back (QSharedPointer <EncryptionAlgorithm> (new EncryptionAlgorithmGOST ()));
        SupportedEncryptionAlgorithms.push_back (QSharedPointer <EncryptionAlgorithm> (new EncryptionAlgorithmGrasshopper()));

		SupportedEncryptionModes.push_back (QSharedPointer <EncryptionMode> (new EncryptionModeXTS ()));

	}

	uint64 VolumeLayoutV1Normal::GetDataOffset (uint64 volumeHostSize) const
	{
        (void)volumeHostSize;
        return HeaderSize;
	}

	uint64 VolumeLayoutV1Normal::GetDataSize (uint64 volumeHostSize) const
	{
		return volumeHostSize - GetHeaderSize();
	}


	VolumeLayoutV1Hidden::VolumeLayoutV1Hidden ()
	{
		Type = VolumeType::Hidden;
		HeaderOffset = -GST_HIDDEN_VOLUME_HEADER_OFFSET_LEGACY;
		HeaderSize = GST_VOLUME_HEADER_SIZE_LEGACY;

        SupportedEncryptionAlgorithms.push_back (QSharedPointer <EncryptionAlgorithm> (new EncryptionAlgorithmGOST ()));
        SupportedEncryptionAlgorithms.push_back (QSharedPointer <EncryptionAlgorithm> (new EncryptionAlgorithmGrasshopper()));

		SupportedEncryptionModes.push_back (QSharedPointer <EncryptionMode> (new EncryptionModeXTS ()));

	}

	uint64 VolumeLayoutV1Hidden::GetDataOffset (uint64 volumeHostSize) const
	{
		return volumeHostSize - GetDataSize (volumeHostSize) + HeaderOffset;
	}

	uint64 VolumeLayoutV1Hidden::GetDataSize (uint64 volumeHostSize) const
	{
        (void)volumeHostSize;
        return Header->GetHiddenVolumeDataSize ();
	}


	VolumeLayoutV2Normal::VolumeLayoutV2Normal ()
	{
		Type = VolumeType::Normal;
		HeaderOffset = GST_VOLUME_HEADER_OFFSET;
		HeaderSize = GST_VOLUME_HEADER_SIZE;
		BackupHeaderOffset = -GST_VOLUME_HEADER_GROUP_SIZE;

        SupportedEncryptionAlgorithms.push_back (QSharedPointer <EncryptionAlgorithm> (new EncryptionAlgorithmGOST ()));
        SupportedEncryptionAlgorithms.push_back (QSharedPointer <EncryptionAlgorithm> (new EncryptionAlgorithmGrasshopper()));

		SupportedEncryptionModes.push_back (QSharedPointer <EncryptionMode> (new EncryptionModeXTS ()));
	}

	uint64 VolumeLayoutV2Normal::GetDataOffset (uint64 volumeHostSize) const
	{
        (void)volumeHostSize;
        return Header->GetEncryptedAreaStart();
	}

	uint64 VolumeLayoutV2Normal::GetDataSize (uint64 volumeHostSize) const
	{
        (void)volumeHostSize;
        return Header->GetVolumeDataSize();
	}

	uint64 VolumeLayoutV2Normal::GetMaxDataSize (uint64 volumeSize) const
	{
		if (volumeSize < GST_TOTAL_VOLUME_HEADERS_SIZE)
			return 0;

		return volumeSize - GST_TOTAL_VOLUME_HEADERS_SIZE;
	}


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

	uint64 VolumeLayoutV2Hidden::GetDataOffset (uint64 volumeHostSize) const
	{
        (void)volumeHostSize;
        return Header->GetEncryptedAreaStart();
	}

	uint64 VolumeLayoutV2Hidden::GetDataSize (uint64 volumeHostSize) const
	{
        (void)volumeHostSize;
        return Header->GetVolumeDataSize();
	}

	uint64 VolumeLayoutV2Hidden::GetMaxDataSize (uint64 volumeSize) const
	{
		// Reserve free space at the end of the host filesystem
		uint64 reservedSize;

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
