#ifndef VOLUMEHASHWHIRLPOOL_H
#define VOLUMEHASHWHIRLPOOL_H

#include "VolumeHash.h"

namespace GostCrypt {
namespace Volume {

// Whirlpool
class Whirlpool : public VolumeHash
{
public:
    Whirlpool ();
    virtual ~Whirlpool () { }

    virtual void GetDigest (const BufferPtr &buffer);
    virtual size_t GetBlockSize () const { return 64; }
    virtual size_t GetDigestSize () const { return 512 / 8; }
    virtual std::wstring GetName () const { return L"Whirlpool"; }
    virtual QSharedPointer <VolumeHash> GetNew () const { return QSharedPointer <VolumeHash> (new Whirlpool); }
    virtual void Init ();
    virtual void ProcessData (const ConstBufferPtr &data);
    virtual void DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const;
    virtual int GetIterationCount () const { return 1000; }

protected:

private:
    Whirlpool (const Whirlpool &);
    Whirlpool &operator= (const Whirlpool &);
};

}
}

#endif // VOLUMEHASHWHIRLPOOL_H
