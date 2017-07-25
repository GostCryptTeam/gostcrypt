#ifndef COREROOT_H
#define COREROOT_H

namespace GostCrypt {
	namespace NewCore {
		class CoreRoot : public CoreBase
		{
		public:
			CoreRoot();
			virtual QSharedPointer<MountVolumeResponse> mountVolume(QSharedPointer<MountVolumeParams> params);
			virtual QSharedPointer<DismountVolumeResponse> dismountVolume(QSharedPointer<DismountVolumeParams> params);
			virtual QSharedPointer<CreateVolumeResponse> createVolume(QSharedPointer<DismountVolumeParams> params);
			virtual QSharedPointer<CreateKeyFileResponse> createKeyFile(QSharedPointer<CreateKeyFileParams> params);
			virtual QSharedPointer<ChangeVolumePasswordResponse> changeVolumePassword(QSharedPointer<ChangeVolumePasswordParams> params);
		};
	}
}

#endif // COREROOT_H
