#ifndef VOLUMELAYOUTV2HIDDEN_H
#define VOLUMELAYOUTV2HIDDEN_H

#include "VolumeLayout.h"

namespace GostCrypt {
namespace Volume {

class VolumeLayoutV2Hidden : public VolumeLayout
{
public:
    VolumeLayoutV2Hidden ();
    virtual ~VolumeLayoutV2Hidden () { }

    virtual quint64 GetDataOffset (quint64 volumeHostSize) const;
    virtual quint64 GetDataSize (quint64 volumeHostSize) const;
    virtual quint64 GetMaxDataSize (quint64 volumeSize) const;
    virtual bool HasBackupHeader () const { return true; }

private:
    VolumeLayoutV2Hidden (const VolumeLayoutV2Hidden &);
    VolumeLayoutV2Hidden &operator= (const VolumeLayoutV2Hidden &);
};

}
}

#endif // VOLUMELAYOUTV2HIDDEN_H
