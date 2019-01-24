/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#include "Core/GostCryptException.h"
#include "VolumeHash.h"
#include "VolumeHashGostHash.h"
#include "VolumeHashStribog.h"
#include "VolumeHashWhirlpool.h"

namespace GostCrypt
{
namespace Volume
{

VolumeHashList VolumeHash::GetAvailableAlgorithms()
{
    VolumeHashList l;

    l.push_back(QSharedPointer <VolumeHash> (new VolumeHashStribog()));
    l.push_back(QSharedPointer <VolumeHash> (new VolumeHashGostHash()));
    l.push_back(QSharedPointer <VolumeHash> (new VolumeHashWhirlpool()));

    return l;
}

void VolumeHash::ValidateKeyDerivationParameters(const BufferPtr& key,
        const VolumePassword& password, const BufferPtr& salt, int iterationCount)
{
    if (key.size() < 1 || password.Size() < 1 || salt.size() < 1 || iterationCount < 1)
    {
        throw InvalidParameterException("key derivation parameters",
                                        "Incorrect key derivation parameters");
    }
}

void VolumeHash::HMAC_DeriveKey(const BufferPtr& key, const VolumePassword& password,
                                const BufferPtr& salt) const
{
    HMAC_DeriveKey(key, password, salt, HMAC_GetIterationCount());
}

}
}
