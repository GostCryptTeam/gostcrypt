#include "CipherAlgorithmGOST.h"
#include "Crypto/GostCipher.h"

namespace GostCrypt {
namespace Volume {

// GOST
void CipherAlgorithmGOST::Decrypt (quint8 *data) const
{
    gost_decrypt (data, data, (gost_kds *) ScheduledKey.get());
}

void CipherAlgorithmGOST::Encrypt (quint8 *data) const
{
    gost_encrypt (data, data, (gost_kds *) ScheduledKey.get());
}

void CipherAlgorithmGOST::DecryptWithKS (quint8 *data, quint8 *ks) const
{
    gost_decrypt (data, data, (gost_kds *) ks);
}

void CipherAlgorithmGOST::EncryptWithKS (quint8 *data, quint8 *ks) const
{
    gost_encrypt (data, data, (gost_kds *) ks);
}

size_t CipherAlgorithmGOST::GetScheduledKeySize () const
{
    return sizeof(gost_kds);
}

void CipherAlgorithmGOST::SetCipherKey (const quint8 *key)
{
    gost_set_key ((quint8 *)key, (gost_kds *)ScheduledKey.get());
}

void CipherAlgorithmGOST::XorCipherKey (quint8 *ks, quint8 *data, int len) const
{
    gost_xor_ks((gost_kds *) ScheduledKey.get(), (gost_kds *) ks, (unsigned int *)data, len / sizeof(unsigned int));
}

void CipherAlgorithmGOST::CopyCipherKey (quint8 *ks) const
{
    size_t i;
    size_t size = ScheduledKey.size();
    const quint8 *ptr = ScheduledKey.get();
    for (i = 0; i < size; i++)
        ks[i] = ptr[i];
}

}
}
