#include "CoreResponse.h"


namespace GostCrypt {
namespace NewCore {

		bool initCoreResponse()
		{
			INIT_SERIALIZE(CoreResponse);
			INIT_SERIALIZE(CreateVolumeResponse);
			INIT_SERIALIZE(ChangeVolumePasswordResponse);
			INIT_SERIALIZE(CreateKeyFileResponse);
			INIT_SERIALIZE(MountVolumeResponse);
			INIT_SERIALIZE(DismountVolumeResponse);
			INIT_SERIALIZE(GetHostDevicesResponse);
			INIT_SERIALIZE(GetMountedVolumesResponse);

            return true;
		}

DEF_SERIALIZABLE(GostCrypt::NewCore::CoreResponse)
QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::CoreResponse& Valeur) {
    return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::CoreResponse & Valeur) {
    return in;
}
 DEF_SERIALIZABLE(GostCrypt::NewCore::CreateVolumeResponse)
QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::CreateVolumeResponse& Valeur) {
    return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::CreateVolumeResponse & Valeur) {
    return in;
}

DEF_SERIALIZABLE(GostCrypt::NewCore::ChangeVolumePasswordResponse)
QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::ChangeVolumePasswordResponse& Valeur) {
    return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::ChangeVolumePasswordResponse & Valeur) {
    return in;
}
DEF_SERIALIZABLE(GostCrypt::NewCore::CreateKeyFileResponse)
QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::CreateKeyFileResponse& Valeur) {
    return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::CreateKeyFileResponse & Valeur) {
    return in;
}
DEF_SERIALIZABLE(GostCrypt::NewCore::MountVolumeResponse)
QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::MountVolumeResponse& Valeur) {
    return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::MountVolumeResponse & Valeur) {
    return in;
}
DEF_SERIALIZABLE(GostCrypt::NewCore::DismountVolumeResponse)
QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::DismountVolumeResponse& Valeur) {
    return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::DismountVolumeResponse & Valeur) {
    return in;
}
DEF_SERIALIZABLE(GostCrypt::NewCore::GetHostDevicesResponse)
QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::GetHostDevicesResponse& Valeur) {
    return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::GetHostDevicesResponse & Valeur) {
    return in;
}
DEF_SERIALIZABLE(GostCrypt::NewCore::GetMountedVolumesResponse)
QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::GetMountedVolumesResponse& Valeur) {
    return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::GetMountedVolumesResponse & Valeur) {
    return in;
}

    }
}
