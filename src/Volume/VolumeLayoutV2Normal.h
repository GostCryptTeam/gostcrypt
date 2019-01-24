#ifndef VOLUMELAYOUTV2NORMAL_H
#define VOLUMELAYOUTV2NORMAL_H

#include "VolumeLayout.h"

namespace GostCrypt
{
namespace Volume
{

class VolumeLayoutV2Normal : public VolumeLayout
{
 public:
    VolumeLayoutV2Normal();
    virtual ~VolumeLayoutV2Normal() { }

    virtual quint64 GetDataOffset(quint64 volumeHostSize) const;
    virtual quint64 GetDataSize(quint64 volumeHostSize) const;
    virtual quint64 GetMaxDataSize(quint64 volumeSize) const;
    virtual bool HasBackupHeader() const { return true; }

 private:
    VolumeLayoutV2Normal(const VolumeLayoutV2Normal&);
    VolumeLayoutV2Normal& operator= (const VolumeLayoutV2Normal&);
};

}
}

#endif // VOLUMELAYOUTV2NORMAL_H
