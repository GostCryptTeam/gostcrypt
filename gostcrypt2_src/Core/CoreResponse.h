#ifndef CORERESPONSE_H
#define CORERESPONSE_H
#include <QVariant>
#include <QList>
#include <QSharedPointer>
#include <QFileInfo>
#include <QtGlobal>
#include "SerializationUtil.h"
#include "CoreRequest.h"
#include "CoreException.h"

namespace GostCrypt {
	namespace Core {
        /**
         * @brief Initialize the CoreResponse class and subclass to make them serializable across processes
         *
         */
        void initCoreResponse();

		struct HostDevice;
        /**
         * @brief Parent class of all response objects used to store results of GostCrypt actions and transmit it back to the UI process
         *
         */
        struct CoreResponse {
            QVariantMap passThrough; /**< QVariantMap storing data coming from the source code executed before the action has been requested and that will be used by source code after the action successful execution */
            DEC_SERIALIZABLE(CoreResponse);
		};

        /**
         * @brief Class storing results from the volume creation action
         *
         */
        struct CreateVolumeResponse : CoreResponse {
			DEC_SERIALIZABLE(CreateVolumeResponse);
		};

        /**
         * @brief Class storing results from the volume instance password changement action
         *
         */
        struct ChangeVolumePasswordResponse : CoreResponse {
			DEC_SERIALIZABLE(ChangeVolumePasswordResponse);
		};

        /**
         * @brief Class storing results from the key file creation action
         *
         */
        struct CreateKeyFileResponse : CoreResponse {
			DEC_SERIALIZABLE(CreateKeyFileResponse);
		};

        /**
         * @brief Class storing results from the volume header backup action
         * This action will save the headers of the volume instances contained inside the given volume container
         */
        struct BackupHeaderResponse : CoreResponse {
            DEC_SERIALIZABLE(BackupHeaderResponse);
        };

        /**
         * @brief Class storing results from the volume header restore action
         * This action will restore the header of the volume instance corresponding to given credentials.
         * It will either use the internal backup or an external header backup file.
         * For this action to succeed the password and keyfiles of the backup header should correspond to the password and keyfiles of the volume instance for which to restore the header
         */
        struct RestoreHeaderResponse : CoreResponse {
            DEC_SERIALIZABLE(RestoreHeaderResponse);
        };

        /**
         * @brief Class storing results from the mount volume action
         *
         */
        struct MountVolumeResponse : CoreResponse {
			MountVolumeResponse();
            bool readOnlyFailover; /**< Boolean true if the volume could not be mounted with write rights, but succeed to mount in read-only mode */
            QSharedPointer<Volume::VolumeInformation> volumeInfo; /**< Mount information about the volume which has just been mounted */
			DEC_SERIALIZABLE(MountVolumeResponse);
		};

        /**
         * @brief Class storing results from the dismount volume action
         *
         */
        struct DismountVolumeResponse : CoreResponse {
            QList<QFileInfo> volumePath; /**< List of the path of the volume container which have just been successfully dismounted */
			DEC_SERIALIZABLE(DismountVolumeResponse);
		};

        /**
         * @brief Class storing results from the action to get the current host devices on the system.
         *
         */
        struct GetHostDevicesResponse : CoreResponse {
            QList<QSharedPointer<HostDevice>> hostDevices; /**< List of current devices on the system */
			DEC_SERIALIZABLE(GetHostDevicesResponse);
		};

        /**
         * @brief Class storing results from the action to get the current mounted GostCrypt volumes on the system and information about them.
         *
         */
        struct GetMountedVolumesResponse : CoreResponse {
            QList<QSharedPointer<Volume::VolumeInformation>> volumeInfoList; /**< List of the currently mounted GostCrypt volumes. For each mounted volume mounting information are provided in the VolumeInformation object */
			DEC_SERIALIZABLE(GetMountedVolumesResponse);
		};

        /**
         * @brief Class storing results from the action to get the list of available encryption algorithms
         *
         */
        struct GetEncryptionAlgorithmsResponse : CoreResponse {
            QStringList algorithms; /**< List of available encryption algorithms */
            DEC_SERIALIZABLE(GetEncryptionAlgorithmsResponse);
        };

        /**
         * @brief Class storing results from the action to get the list of available key derivation functions
         *
         */
        struct GetDerivationFunctionsResponse : CoreResponse {
            QStringList algorithms; /**< List of available key derivation functions */
            DEC_SERIALIZABLE(GetDerivationFunctionsResponse);
        };

        /**
         * @brief Class storing information about a mounted file system (corresponding to GostCrypt or not)
         *
         */
        struct MountedFilesystem
		{
            QFileInfo Device; /**< Path of the device block file */
            QFileInfo MountPoint; /**< Path of the directory where this device is mounted */
            QString Type; /**< Filesystem type */
            DEC_SERIALIZABLE(MountedFilesystem);
		};

        /**
         * @brief Class storing information about a host device (corresponding to GostCrypt or not)
         *
         */
        struct HostDevice {
            QFileInfo mountPoint; /**< Path of the directory where this device is mounted (Empty if the device is not mounted) */
            QFileInfo devicePath; /**< Path of the device block file */
            quint64 size; /**< Size of the device in bytes */
            QList<QSharedPointer<HostDevice>> partitions; /**< List of partition in this device */
			DEC_SERIALIZABLE(HostDevice);
		};

        /**
         * @brief Class storing results from the encryption algorithm benchmark action
         *
         */
        struct BenchmarkAlgorithmsResponse: CoreResponse {
            QStringList algorithmsNames; /**< Names of the encryption algorithms benchmarked */
            QList<quint32> encryptionSpeed;/**< Encryption speed in bytes/s for each encryption algorithm in the same order as in algorithmsNames */
            QList<quint32> decryptionSpeed;/**< Decryption speed in bytes/s for each encryption algorithm in the same order as in algorithmsNames */
            QList<quint32> meanSpeed;/**< Mean of the encryption/decryption speed in bytes/s for each encryption algorithm in the same order as in algorithmsNames */
            DEC_SERIALIZABLE(BenchmarkAlgorithmsResponse);
        };
}
}

SERIALIZABLE(GostCrypt::Core::CoreResponse)
SERIALIZABLE(GostCrypt::Core::CreateVolumeResponse)
SERIALIZABLE(GostCrypt::Core::ChangeVolumePasswordResponse)
SERIALIZABLE(GostCrypt::Core::CreateKeyFileResponse)
SERIALIZABLE(GostCrypt::Core::MountVolumeResponse)
SERIALIZABLE(GostCrypt::Core::DismountVolumeResponse)
SERIALIZABLE(GostCrypt::Core::GetHostDevicesResponse)
SERIALIZABLE(GostCrypt::Core::GetMountedVolumesResponse)
SERIALIZABLE(GostCrypt::Core::GetEncryptionAlgorithmsResponse)
SERIALIZABLE(GostCrypt::Core::GetDerivationFunctionsResponse)
SERIALIZABLE(GostCrypt::Core::HostDevice)
SERIALIZABLE(GostCrypt::Core::MountedFilesystem)
SERIALIZABLE(GostCrypt::Core::BackupHeaderResponse)
SERIALIZABLE(GostCrypt::Core::RestoreHeaderResponse)
SERIALIZABLE(GostCrypt::Core::BenchmarkAlgorithmsResponse)

#endif // CORERESPONSE_H
