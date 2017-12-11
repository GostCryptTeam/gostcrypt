#include "VolumeHashGostHash.h"

#include "Crypto/GostHash.h"
#include "Crypto/Pkcs5.h"

namespace GostCrypt {
namespace Volume {

// GOST R 34.11-94
VolumeHashGostHash::VolumeHashGostHash ()
{
    Context.Allocate (sizeof (gost_hash_ctx));
    Init ();
}

void VolumeHashGostHash::GetDigest (BufferPtr &buffer)
{
    //if_debug (ValidateDigestParameters (buffer));
    GOSTHASH_finalize ((gost_hash_ctx *) Context.Get(), buffer);
}

void VolumeHashGostHash::Init ()
{
    GOSTHASH_init ((gost_hash_ctx *) Context.Get());
}

void VolumeHashGostHash::ProcessData (const BufferPtr &data)
{
    //if_debug (ValidateDataParameters (data));
    GOSTHASH_add ((quint8 *) data.Get(), data.Size(), (gost_hash_ctx *) Context.Get());
}

void VolumeHashGostHash::HMAC_DeriveKey (const BufferPtr &key, const VolumePassword &password, const BufferPtr &salt, int iterationCount) const
{
    ValidateKeyDerivationParameters (key, password, salt, iterationCount);
    derive_key_gosthash ((char *) password.DataPtr(), (int) password.Size(), (char *) salt.Get(), (int) salt.Size(), iterationCount, (char *) key.Get(), (int) key.Size());
}

}
}
