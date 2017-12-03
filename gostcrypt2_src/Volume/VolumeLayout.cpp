/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "Volume/EncryptionMode.h"
#include "Volume/EncryptionModeXTS.h"
#include "VolumeLayout.h"
#include "VolumeLayoutV2Normal.h"
#include "VolumeLayoutV2Hidden.h"


namespace GostCrypt {
namespace Volume {

VolumeLayout::VolumeLayout ()
    : BackupHeaderOffset(0),
      HeaderOffset(0),
      Type(VolumeType::Enum::Unknown)
{
}

VolumeLayout::~VolumeLayout ()
{
}

VolumeLayoutList VolumeLayout::GetAvailableLayouts (VolumeType::Enum type)
{
    VolumeLayoutList layouts;

    layouts.push_back (QSharedPointer <VolumeLayout> (new VolumeLayoutV2Normal ()));
    layouts.push_back (QSharedPointer <VolumeLayout> (new VolumeLayoutV2Hidden ()));

    if (type != VolumeType::Unknown)
    {
        VolumeLayoutList l;

        for (QSharedPointer <VolumeLayout> vl : layouts)
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

}
}
