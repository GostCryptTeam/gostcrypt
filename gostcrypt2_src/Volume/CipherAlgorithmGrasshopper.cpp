#include "CipherAlgorithmGrasshopper.h"
#include "Crypto/GrasshopperCipher.h"

namespace GostCrypt
{
namespace Volume
{

void CipherAlgorithmGrasshopper::Decrypt(quint8* data) const
{
    grasshopper_decrypt((grasshopper_kds*) ScheduledKey.get(), (quint64*) data, (quint64*) data);
}

void CipherAlgorithmGrasshopper::Encrypt(quint8* data) const
{
    grasshopper_encrypt((grasshopper_kds*) ScheduledKey.get(), (quint64*) data, (quint64*) data);
}

void CipherAlgorithmGrasshopper::DecryptWithKS(quint8* data, quint8* ks) const
{
    grasshopper_decrypt((grasshopper_kds*) ks, (quint64*) data, (quint64*) data);
}

void CipherAlgorithmGrasshopper::EncryptWithKS(quint8* data, quint8* ks) const
{
    grasshopper_encrypt((grasshopper_kds*) ks, (quint64*) data, (quint64*) data);
}

size_t CipherAlgorithmGrasshopper::GetScheduledKeySize() const
{
    return sizeof(grasshopper_kds);
}

void CipherAlgorithmGrasshopper::SetCipherKey(const quint8* key)
{
    grasshopper_set_key((quint64*)key, (grasshopper_kds*) ScheduledKey.get());
}

void CipherAlgorithmGrasshopper::XorCipherKey(quint8* ks, quint8* data, int len) const
{
    grasshopper_xor_ks((grasshopper_kds*) ScheduledKey.get(), (grasshopper_kds*) ks, (quint64*)data,
                       len / sizeof(unsigned long long));
}

void CipherAlgorithmGrasshopper::CopyCipherKey(quint8* ks) const
{
    size_t i;
    size_t size = ScheduledKey.size();
    const quint8* ptr = ScheduledKey.get();
    for (i = 0; i < size; i++)
    {
        ks[i] = ptr[i];
    }
}

}
}
