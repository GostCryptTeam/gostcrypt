#include "VolumeInformation.h"

namespace GostCrypt
{
namespace Volume
{

DEF_SERIALIZABLE(VolumeInformation)
QDataStream& operator << (QDataStream& out, const VolumeInformation& Valeur)
{
    out << Valeur.encryptionAlgorithmName;
    out << Valeur.fuseMountPoint;
    out << Valeur.mountPoint;
    out << (quint32)Valeur.protection;
    out << Valeur.size;
    out << (quint32)Valeur.type;
    out << Valeur.virtualDevice;
    out << Valeur.volumePath;
    return out;
}
QDataStream& operator >> (QDataStream& in, VolumeInformation& Valeur)
{
    quint32 protection, type;

    in >> Valeur.encryptionAlgorithmName;
    in >> Valeur.fuseMountPoint;
    in >> Valeur.mountPoint;
    in >> protection;
    Valeur.protection = VolumeProtection::Enum(protection);
    in >> Valeur.size;
    in >> type;
    Valeur.type = VolumeType::Enum(type);
    in >> Valeur.virtualDevice;
    in >> Valeur.volumePath;
    return in;
}

}
}
