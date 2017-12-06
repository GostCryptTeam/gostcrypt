#ifndef VOLUMEHASHWHIRLPOOL_H
#define VOLUMEHASHWHIRLPOOL_H

#include "VolumeHash.h"

namespace GostCrypt {
namespace Volume {

// Whirlpool
class VolumeHashWhirlpool : public VolumeHash
{
public:
    VolumeHashWhirlpool ();
    virtual ~VolumeHashWhirlpool () { }

    virtual void GetDigest (BufferPtr &buffer);
    virtual size_t GetBlockSize () const { return 64; }
    virtual size_t GetDigestSize () const { return 512 / 8; }
    virtual std::wstring GetName () const { return L"Whirlpool"; }
    virtual QSharedPointer <VolumeHash> GetNew () const { return QSharedPointer <VolumeHash> (new VolumeHashWhirlpool); }
    virtual void Init ();
    virtual void ProcessData (const BufferPtr &data);
    virtual void DeriveKey (const BufferPtr &key, const VolumePassword &password, const BufferPtr &salt, int iterationCount) const;
    virtual int GetIterationCount () const { return 1000; }

protected:

private:
    VolumeHashWhirlpool (const VolumeHashWhirlpool &);
    VolumeHashWhirlpool &operator= (const VolumeHashWhirlpool &);
};

}
}

#endif // VOLUMEHASHWHIRLPOOL_H
