#ifndef VOLUMEINFORMATION_H
#define VOLUMEINFORMATION_H
#include <QFileInfo>
#include "Core/SerializationUtil.h"
#include "VolumeProtection.h"
#include "VolumeType.h"

namespace GostCrypt {
namespace Volume {

struct VolumeInformation
{
    VolumeInformation() = default;
    QSharedPointer<QFileInfo> fuseMountPoint;
    QString encryptionAlgorithmName;
    QSharedPointer<QFileInfo> virtualDevice;
    QSharedPointer<QFileInfo> mountPoint;
    QSharedPointer<QFileInfo> volumePath;
    VolumeProtection::Enum protection;
    quint64 size;
    VolumeType::Enum type;
    DEC_SERIALIZABLE(VolumeInformation);
};

}
}

#endif // VOLUMEINFORMATION_H
