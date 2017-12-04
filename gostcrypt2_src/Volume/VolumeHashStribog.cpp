#include "VolumeHashStribog.h"

#include "Crypto/Stribog.h"
#include "Crypto/Pkcs5.h"

namespace GostCrypt {
namespace Volume {

// Stribog
VolumeHashStribog::VolumeHashStribog ()
{
    Context.Allocate (sizeof (STRIBOG_CTX));
    Init ();
}

void VolumeHashStribog::GetDigest (const BufferPtr &buffer)
{
    //if_debug (ValidateDigestParameters (buffer));
    STRIBOG_finalize ((STRIBOG_CTX *) Context.Ptr(), buffer);
}

void VolumeHashStribog::Init ()
{
    STRIBOG_init ((STRIBOG_CTX *) Context.Ptr());
}

void VolumeHashStribog::ProcessData (const ConstBufferPtr &data)
{
    //if_debug (ValidateDataParameters (data));
    STRIBOG_add ((STRIBOG_CTX *) Context.Ptr(), (quint8 *) data.Get(), data.Size());
}

void VolumeHashStribog::DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const
{
    ValidateKeyDerivationParameters (key, password, salt, iterationCount);
    derive_key_stribog ((char *) password.DataPtr(), (int) password.Size(), (char *) salt.Get(), (int) salt.Size(), iterationCount, (char *) key.Get(), (int) key.Size());
}

}
}
