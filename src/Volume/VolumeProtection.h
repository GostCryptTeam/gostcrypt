#ifndef VOLUMEPROTECTION_H
#define VOLUMEPROTECTION_H

namespace GostCrypt
{
namespace Volume
{
struct VolumeProtection
{
    enum Enum
    {
        None,
        ReadOnly,
        HiddenVolumeReadOnly
    };
};
}
}

#endif // VOLUMEPROTECTION_H
