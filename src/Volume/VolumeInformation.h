#ifndef VOLUMEINFORMATION_H
#define VOLUMEINFORMATION_H
#include <QFileInfo>
#include "Core/SerializationUtil.h"
#include "VolumeProtection.h"
#include "VolumeType.h"

namespace GostCrypt
{
namespace Volume
{

struct VolumeInformation
{
    VolumeInformation() = default;
    QFileInfo fuseMountPoint;
    QString encryptionAlgorithmName;
    QFileInfo virtualDevice;
    QFileInfo mountPoint;
    QFileInfo volumePath;
    VolumeProtection::Enum protection;
    quint64 size;
    VolumeType::Enum type;
    DEC_SERIALIZABLE(VolumeInformation);
};

}
}

#endif // VOLUMEINFORMATION_H
