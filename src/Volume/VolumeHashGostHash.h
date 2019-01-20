#ifndef VOLUMEHASHGOSTHASH_H
#define VOLUMEHASHGOSTHASH_H

#include "VolumeHash.h"

namespace GostCrypt
{
namespace Volume
{

// GOST R 34.11-94
class VolumeHashGostHash : public VolumeHash
{
 public:
    VolumeHashGostHash();
    virtual ~VolumeHashGostHash() { }

    virtual void GetDigest(BufferPtr& buffer);
    virtual size_t GetBlockSize() const { return 32; }
    virtual size_t GetDigestSize() const { return 32; }
    virtual QString GetName() const { return QStringLiteral("GOST R 34.11-94"); }
    virtual QSharedPointer <VolumeHash> GetNew() const { return QSharedPointer <VolumeHash> (new VolumeHashGostHash); }
    virtual void Init();
    virtual void ProcessData(const BufferPtr& data);
    virtual void HMAC_DeriveKey(const BufferPtr& key, const VolumePassword& password,
                                const BufferPtr& salt, int iterationCount) const;
    virtual int HMAC_GetIterationCount() const { return 1000; }

 protected:

 private:
    VolumeHashGostHash(const VolumeHashGostHash&);
    VolumeHashGostHash& operator= (const VolumeHashGostHash&);
};

}
}

#endif // VOLUMEHASHWHIRLPOOL_H
