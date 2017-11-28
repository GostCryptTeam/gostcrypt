#ifndef VOLUMEHASHSTRIBOG_H
#define VOLUMEHASHSTRIBOG_H

#include "VolumeHash.h"

namespace GostCrypt {
namespace Volume {

// Stribog
class Stribog : public VolumeHash
{
public:
    Stribog ();
    virtual ~Stribog () { }

    virtual void GetDigest (const BufferPtr &buffer);
    virtual size_t GetBlockSize () const { return 64; }
    virtual size_t GetDigestSize () const { return 64; }
    virtual std::wstring GetName () const { return L"GOST R 34.11-2012"; }
    virtual QSharedPointer <VolumeHash> GetNew () const { return QSharedPointer <VolumeHash> (new Stribog); }
    virtual void Init ();
    virtual void ProcessData (const ConstBufferPtr &data);
    virtual void DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const;
    virtual int GetIterationCount () const { return 1000; }

protected:

private:
    Stribog (const Stribog &);
    Stribog &operator= (const Stribog &);
};

}
}
#endif // VOLUMEHASHWHIRLPOOL_H
