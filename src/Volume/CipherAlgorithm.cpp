/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#include "VolumeException.h"
#include "CipherAlgorithm.h"
#include "CipherAlgorithmGOST.h"
#include "CipherAlgorithmGrasshopper.h"

namespace GostCrypt
{
namespace Volume
{

CipherAlgorithm::CipherAlgorithm() : Initialized(false)
{
}

CipherAlgorithm::~CipherAlgorithm()
{
}

void CipherAlgorithm::DecryptBlocks(quint8* data, size_t blockCount) const
{
    if (!Initialized)
    {
        throw CipherAlgorithmNotInitializedException();
    }

    while (blockCount-- > 0)
    {
        Decrypt(data);
        data += GetBlockSize();
    }
}

void CipherAlgorithm::EncryptBlock(quint8* data) const
{
    if (!Initialized)
    {
        throw CipherAlgorithmNotInitializedException();
    }

    Encrypt(data);
}

void CipherAlgorithm::EncryptBlocks(quint8* data, size_t blockCount) const
{
    if (!Initialized)
    {
        throw CipherAlgorithmNotInitializedException();
    }

    while (blockCount-- > 0)
    {
        Encrypt(data);
        data += GetBlockSize();
    }
}

void CipherAlgorithm::SetKey(const BufferPtr& key)
{
    if (key.size() != GetKeySize())
    {
        throw InvalidParameterException("key", "key.Size() != GetKeySize ()");
    }

    if (!Initialized)
    {
        ScheduledKey.allocate(GetScheduledKeySize());
    }

    SetCipherKey(key);
    Key.copyFrom(key);
    Initialized = true;
}

}
}
