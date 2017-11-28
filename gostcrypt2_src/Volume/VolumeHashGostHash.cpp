#include "VolumeHashGostHash.h"

#include "Crypto/GostHash.h"
#include "Common/Pkcs5.h"

namespace GostCrypt {
namespace Volume {

// GOST R 34.11-94
GostHash::GostHash ()
{
    Context.Allocate (sizeof (gost_hash_ctx));
    Init ();
}

void GostHash::GetDigest (const BufferPtr &buffer)
{
    //if_debug (ValidateDigestParameters (buffer));
    GOSTHASH_finalize ((gost_hash_ctx *) Context.Ptr(), buffer);
}

void GostHash::Init ()
{
    GOSTHASH_init ((gost_hash_ctx *) Context.Ptr());
}

void GostHash::ProcessData (const ConstBufferPtr &data)
{
    //if_debug (ValidateDataParameters (data));
    GOSTHASH_add ((quint8 *) data.Get(), data.Size(), (gost_hash_ctx *) Context.Ptr());
}

void GostHash::DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const
{
    ValidateKeyDerivationParameters (key, password, salt, iterationCount);
    derive_key_gosthash ((char *) password.DataPtr(), (int) password.Size(), (char *) salt.Get(), (int) salt.Size(), iterationCount, (char *) key.Get(), (int) key.Size());
}

}
}
