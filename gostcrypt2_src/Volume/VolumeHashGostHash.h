#ifndef VOLUMEHASHGOSTHASH_H
#define VOLUMEHASHGOSTHASH_H

#include "VolumeHash.h"

namespace GostCrypt {
namespace Volume {

// GOST R 34.11-94
class GostHash : public VolumeHash
{
public:
    GostHash ();
    virtual ~GostHash () { }

    virtual void GetDigest (const BufferPtr &buffer);
    virtual size_t GetBlockSize () const { return 32; }
    virtual size_t GetDigestSize () const { return 32; }
    virtual std::wstring GetName () const { return L"GOST R 34.11-94"; }
    virtual QSharedPointer <VolumeHash> GetNew () const { return QSharedPointer <VolumeHash> (new GostHash); }
    virtual void Init ();
    virtual void ProcessData (const ConstBufferPtr &data);
    virtual void DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const;
    virtual int GetIterationCount () const { return 1000; }

protected:

private:
    GostHash (const GostHash &);
    GostHash &operator= (const GostHash &);
};

}
}

#endif // VOLUMEHASHWHIRLPOOL_H
