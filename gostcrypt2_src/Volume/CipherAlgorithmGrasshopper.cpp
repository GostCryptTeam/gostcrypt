#include "CipherAlgorithmGrasshopper.h"
#include "Crypto/GrasshopperCipher.h"

namespace GostCrypt {
namespace Volume {

void CipherAlgorithmGrasshopper::Decrypt (quint8 *data) const
{
    grasshopper_decrypt ((grasshopper_kds*) ScheduledKey.Ptr(), (gst_ludword *) data, (gst_ludword *) data);
}

void CipherAlgorithmGrasshopper::Encrypt (quint8 *data) const
{
    grasshopper_encrypt ((grasshopper_kds*) ScheduledKey.Ptr(), (gst_ludword *) data, (gst_ludword *) data);
}

void CipherAlgorithmGrasshopper::DecryptWithKS (quint8 *data, quint8 *ks) const
{
    grasshopper_decrypt ((grasshopper_kds*) ks, (gst_ludword *) data, (gst_ludword *) data);
}

void CipherAlgorithmGrasshopper::EncryptWithKS (quint8 *data, quint8 *ks) const
{
    grasshopper_encrypt ((grasshopper_kds*) ks, (gst_ludword *) data, (gst_ludword *) data);
}

size_t CipherAlgorithmGrasshopper::GetScheduledKeySize () const
{
    return sizeof(grasshopper_kds);
}

void CipherAlgorithmGrasshopper::SetCipherKey (const quint8 *key)
{
    grasshopper_set_key((gst_ludword*)key, (grasshopper_kds *) ScheduledKey.Ptr());
}

void CipherAlgorithmGrasshopper::XorCipherKey (quint8 *ks, quint8 *data, int len) const
{
    grasshopper_xor_ks((grasshopper_kds *) ScheduledKey.Ptr(), (grasshopper_kds *) ks, (gst_ludword *)data, len / sizeof(unsigned long long));
}

void CipherAlgorithmGrasshopper::CopyCipherKey (quint8 *ks) const
{
    size_t i;
    size_t size = ScheduledKey.Size();
    quint8 *ptr = ScheduledKey.Ptr();
    for (i = 0; i < size; i++)
        ks[i] = ptr[i];
}

}
}
