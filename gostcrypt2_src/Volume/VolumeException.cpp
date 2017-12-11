#include "VolumeException.h"

namespace GostCrypt
{
namespace Volume
{

void initVolumeException()
{
    INIT_SERIALIZE(VolumeException);
    INIT_SERIALIZE(EncryptionTestFailed);
    INIT_SERIALIZE(EncryptionModeNotInitialized);
    INIT_SERIALIZE(CipherAlgorithmNotInitialized);
    INIT_SERIALIZE(EncryptionAlgorithmNotInitialized);
    INIT_SERIALIZE(BufferAlreadyFreed);
    INIT_SERIALIZE(VolumeNotOpen);
    INIT_SERIALIZE(VolumeReadOnly);
    INIT_SERIALIZE(VolumeProtected);
    INIT_SERIALIZE(VolumeCorrupted);
    INIT_SERIALIZE(VolumeVersionNotCompatible);
    INIT_SERIALIZE(FailedGetSectorSize);
    INIT_SERIALIZE(FailedGetTimestamps);
    INIT_SERIALIZE(FailedResetTimestamps);
    INIT_SERIALIZE(ProtectionPasswordOrKeyfilesIncorrect);
    INIT_SERIALIZE(PasswordOrKeyfilesIncorrect);
}

DEF_SERIALIZABLE(GostCrypt::Volume::VolumeException)
QDataStream& operator << (QDataStream& out, const GostCrypt::Volume::VolumeException& Valeur)
{
    out << static_cast<const GostCryptException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Volume::VolumeException& Valeur)
{
    in >> static_cast<GostCryptException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Volume::EncryptionTestFailed)
QDataStream& operator << (QDataStream& out, const GostCrypt::Volume::EncryptionTestFailed& Valeur)
{
    out << static_cast<const VolumeException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Volume::EncryptionTestFailed& Valeur)
{
    in >> static_cast<VolumeException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Volume::EncryptionModeNotInitialized)
QDataStream& operator << (QDataStream& out,
                          const GostCrypt::Volume::EncryptionModeNotInitialized& Valeur)
{
    out << static_cast<const VolumeException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Volume::EncryptionModeNotInitialized& Valeur)
{
    in >> static_cast<VolumeException&>(Valeur);
    return in;
}


DEF_SERIALIZABLE(GostCrypt::Volume::CipherAlgorithmNotInitialized)
QDataStream& operator << (QDataStream& out,
                          const GostCrypt::Volume::CipherAlgorithmNotInitialized& Valeur)
{
    out << static_cast<const VolumeException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::Volume::CipherAlgorithmNotInitialized& Valeur)
{
    in >> static_cast<VolumeException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Volume::EncryptionAlgorithmNotInitialized)
QDataStream& operator << (QDataStream& out,
                          const GostCrypt::Volume::EncryptionAlgorithmNotInitialized& Valeur)
{
    out << static_cast<const VolumeException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in,
                          GostCrypt::Volume::EncryptionAlgorithmNotInitialized& Valeur)
{
    in >> static_cast<VolumeException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Volume::BufferAlreadyFreed)
QDataStream& operator << (QDataStream& out,
                          const GostCrypt::Volume::BufferAlreadyFreed& Valeur)
{
    out << static_cast<const VolumeException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in,
                          GostCrypt::Volume::BufferAlreadyFreed& Valeur)
{
    in >> static_cast<VolumeException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Volume::VolumeNotOpen)
QDataStream& operator << (QDataStream& out,
                          const GostCrypt::Volume::VolumeNotOpen& Valeur)
{
    out << static_cast<const VolumeException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in,
                          GostCrypt::Volume::VolumeNotOpen& Valeur)
{
    in >> static_cast<VolumeException&>(Valeur);
    return in;
}

//define operator for a common parrent class


DEF_SERIALIZABLE(GostCrypt::Volume::VolumeReadOnly)
QDataStream& operator << (QDataStream& out,
                          const GostCrypt::Volume::VolumeReadOnly& Valeur)
{
    out << static_cast<const VolumeException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in,
                          GostCrypt::Volume::VolumeReadOnly& Valeur)
{
    in >> static_cast<VolumeException&>(Valeur);
    return in;
}
DEF_SERIALIZABLE(GostCrypt::Volume::VolumeProtected)
QDataStream& operator << (QDataStream& out,
                          const GostCrypt::Volume::VolumeProtected& Valeur)
{
    out << static_cast<const VolumeException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in,
                          GostCrypt::Volume::VolumeProtected& Valeur)
{
    in >> static_cast<VolumeException&>(Valeur);
    return in;
}
DEF_SERIALIZABLE(GostCrypt::Volume::VolumeCorrupted)
QDataStream& operator << (QDataStream& out,
                          const GostCrypt::Volume::VolumeCorrupted& Valeur)
{
    out << static_cast<const VolumeException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in,
                          GostCrypt::Volume::VolumeCorrupted& Valeur)
{
    in >> static_cast<VolumeException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Volume::FailedGetTimestamps)
QDataStream& operator << (QDataStream& out,
                          const GostCrypt::Volume::FailedGetTimestamps& Valeur)
{
    out << static_cast<const VolumeException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in,
                          GostCrypt::Volume::FailedGetTimestamps& Valeur)
{
    in >> static_cast<VolumeException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Volume::FailedGetSectorSize)
QDataStream& operator << (QDataStream& out,
                          const GostCrypt::Volume::FailedGetSectorSize& Valeur)
{
    out << static_cast<const VolumeException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in,
                          GostCrypt::Volume::FailedGetSectorSize& Valeur)
{
    in >> static_cast<VolumeException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Volume::FailedResetTimestamps)
QDataStream& operator << (QDataStream& out,
                          const GostCrypt::Volume::FailedResetTimestamps& Valeur)
{
    out << static_cast<const VolumeException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in,
                          GostCrypt::Volume::FailedResetTimestamps& Valeur)
{
    in >> static_cast<VolumeException&>(Valeur);
    return in;
}
DEF_SERIALIZABLE(GostCrypt::Volume::VolumeVersionNotCompatible)
QDataStream& operator << (QDataStream& out,
                          const GostCrypt::Volume::VolumeVersionNotCompatible& Valeur)
{
    out << static_cast<const VolumeException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in,
                          GostCrypt::Volume::VolumeVersionNotCompatible& Valeur)
{
    in >> static_cast<VolumeException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Volume::ProtectionPasswordOrKeyfilesIncorrect)
QDataStream& operator << (QDataStream& out,
                          const GostCrypt::Volume::ProtectionPasswordOrKeyfilesIncorrect& Valeur)
{
    out << static_cast<const VolumeException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in,
                          GostCrypt::Volume::ProtectionPasswordOrKeyfilesIncorrect& Valeur)
{
    in >> static_cast<VolumeException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::Volume::PasswordOrKeyfilesIncorrect)
QDataStream& operator << (QDataStream& out,
                          const GostCrypt::Volume::PasswordOrKeyfilesIncorrect& Valeur)
{
    out << static_cast<const VolumeException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in,
                          GostCrypt::Volume::PasswordOrKeyfilesIncorrect& Valeur)
{
    in >> static_cast<VolumeException&>(Valeur);
    return in;
}
DEF_SERIALIZABLE(GostCrypt::Volume::DataNotMutable)
QDataStream& operator << (QDataStream& out,
                        const GostCrypt::Volume::DataNotMutable& Valeur)
{
    out << static_cast<const VolumeException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in,
                        GostCrypt::Volume::DataNotMutable& Valeur)
{
    in >> static_cast<VolumeException&>(Valeur);
    return in;
}
}
}
