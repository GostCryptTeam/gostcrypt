#include "VolumeHashStribog.h"

#include "Crypto/Stribog.h"
#include "Crypto/Pkcs5.h"

namespace GostCrypt
{
namespace Volume
{

// Stribog
VolumeHashStribog::VolumeHashStribog()
{
    Context.allocate(sizeof(STRIBOG_CTX));
    Init();
}

void VolumeHashStribog::GetDigest(BufferPtr& buffer)
{
    //if_debug (ValidateDigestParameters (buffer));
    STRIBOG_finalize((STRIBOG_CTX*) Context.get(), buffer);
}

void VolumeHashStribog::Init()
{
    STRIBOG_init((STRIBOG_CTX*) Context.get());
}

void VolumeHashStribog::ProcessData(const BufferPtr& data)
{
    //if_debug (ValidateDataParameters (data));
    STRIBOG_add((STRIBOG_CTX*) Context.get(), (quint8*) data.get(), data.size());
}

void VolumeHashStribog::HMAC_DeriveKey(const BufferPtr& key, const VolumePassword& password,
                                       const BufferPtr& salt, int iterationCount) const
{
    ValidateKeyDerivationParameters(key, password, salt, iterationCount);
    derive_key_stribog((char*) password.DataPtr(), (int) password.Size(), (char*) salt.get(),
                       (int) salt.size(), iterationCount, (char*) key.get(), (int) key.size());
}

}
}
