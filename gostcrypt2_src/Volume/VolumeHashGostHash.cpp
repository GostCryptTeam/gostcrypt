#include "VolumeHashGostHash.h"

#include "Crypto/GostHash.h"
#include "Crypto/Pkcs5.h"

namespace GostCrypt {
namespace Volume {

// GOST R 34.11-94
VolumeHashGostHash::VolumeHashGostHash ()
{
    Context.allocate (sizeof (gost_hash_ctx));
    Init ();
}

void VolumeHashGostHash::GetDigest (BufferPtr &buffer)
{
    //if_debug (ValidateDigestParameters (buffer));
    GOSTHASH_finalize ((gost_hash_ctx *) Context.get(), buffer);
}

void VolumeHashGostHash::Init ()
{
    GOSTHASH_init ((gost_hash_ctx *) Context.get());
}

void VolumeHashGostHash::ProcessData (const BufferPtr &data)
{
    //if_debug (ValidateDataParameters (data));
    GOSTHASH_add ((quint8 *) data.get(), data.size(), (gost_hash_ctx *) Context.get());
}

void VolumeHashGostHash::HMAC_DeriveKey (const BufferPtr &key, const VolumePassword &password, const BufferPtr &salt, int iterationCount) const
{
    ValidateKeyDerivationParameters (key, password, salt, iterationCount);
    derive_key_gosthash ((char *) password.DataPtr(), (int) password.Size(), (char *) salt.get(), (int) salt.size(), iterationCount, (char *) key.get(), (int) key.size());
}

}
}
