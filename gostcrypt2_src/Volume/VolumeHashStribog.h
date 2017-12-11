#ifndef VOLUMEHASHSTRIBOG_H
#define VOLUMEHASHSTRIBOG_H

#include "VolumeHash.h"

namespace GostCrypt {
namespace Volume {

// Stribog
class VolumeHashStribog : public VolumeHash
{
public:
    VolumeHashStribog ();
    virtual ~VolumeHashStribog () { }

    virtual void GetDigest (BufferPtr &buffer);
    virtual size_t GetBlockSize () const { return 64; }
    virtual size_t GetDigestSize () const { return 64; }
    virtual QString GetName () const { return QStringLiteral("GOST R 34.11-2012"); }
    virtual QSharedPointer <VolumeHash> GetNew () const { return QSharedPointer <VolumeHash> (new VolumeHashStribog); }
    virtual void Init ();
    virtual void ProcessData (const BufferPtr &data);
    virtual void HMAC_DeriveKey (const BufferPtr &key, const VolumePassword &password, const BufferPtr &salt, int iterationCount) const;
    virtual int HMAC_GetIterationCount () const { return 1000; }

protected:

private:
    VolumeHashStribog (const VolumeHashStribog &);
    VolumeHashStribog &operator= (const VolumeHashStribog &);
};

}
}
#endif // VOLUMEHASHWHIRLPOOL_H
