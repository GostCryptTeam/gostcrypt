#include "VolumeHashWhirlpool.h"

#include "Crypto/Whirlpool.h"
#include "Crypto/Pkcs5.h"

namespace GostCrypt {
namespace Volume {

// Whirlpool
VolumeHashWhirlpool::VolumeHashWhirlpool ()
{
    Context.Allocate (sizeof (WHIRLPOOL_CTX));
    Init();
}

void VolumeHashWhirlpool::GetDigest (BufferPtr &buffer)
{
    //if_debug (ValidateDigestParameters (buffer));
    WHIRLPOOL_finalize ((WHIRLPOOL_CTX *) Context.Get(), buffer);
}

void VolumeHashWhirlpool::Init ()
{
    WHIRLPOOL_init ((WHIRLPOOL_CTX *) Context.Get());
}

void VolumeHashWhirlpool::ProcessData (const BufferPtr &data)
{
    //if_debug (ValidateDataParameters (data));
    WHIRLPOOL_add (data.Get(), (int) data.Size() * 8, (WHIRLPOOL_CTX *) Context.Get());
}

void VolumeHashWhirlpool::HMAC_DeriveKey (const BufferPtr &key, const VolumePassword &password, const BufferPtr &salt, int iterationCount) const
{
    ValidateKeyDerivationParameters (key, password, salt, iterationCount);
    derive_key_whirlpool ((char *) password.DataPtr(), (int) password.Size(), (char *) salt.Get(), (int) salt.Size(), iterationCount, (char *) key.Get(), (int) key.Size());
}

}
}
