#ifndef COREBASE_H
#define COREBASE_H

#include <QObject>
#include <QCoreApplication>
#include "CoreRequest.h"
#include "CoreResponse.h"
#include "CoreException.h"
#include "RandomNumberGenerator.h"
#include "Volume/EncryptionModeXTS.h"
#include "Volume/EncryptionAlgorithm.h"
#include "Volume/Pkcs5Kdf.h"

#define GOSTCRYPT_FUSE_MOUNT_DIR_PREFIX ".gostcrypt_aux_mnt"
#define HANDLE_REQUEST(requestName, fct) \
	if(r.canConvert<QSharedPointer<requestName ## Request>>()) { \
		QSharedPointer<requestName ## Request> request; \
		request = r.value<QSharedPointer<requestName ## Request>>(); \
		fct (request); \
	}
#define DEC_REQUEST_SIGNAL(requestName) void send ## requestName (QSharedPointer<requestName ## Response> r)
#define UPDATE_PROGRESS(p) emit sendProgressUpdate(QSharedPointer<ProgressUpdateResponse>(new ProgressUpdateResponse(params->id.requestId,params->id.end*p+params->id.start*(1-p))))



namespace GostCrypt {
	namespace NewCore {
		/**
		 * @brief Abstract class defining the core object which manage any action handled by GostCrypt (mount, dismount, create a volume, etc)
		 *
		 */
		class CoreBase : public QObject
		{
			Q_OBJECT
		public:
			/**
			 * @brief Default Constructor
			 *
			 * @param parent parent object
			 */
			explicit CoreBase(QObject *parent = nullptr);
		public slots:
			/**
			 * @brief Execute the given action request (mount, dismount, create a volume, etc)
			 *
			 * @param request QVariant containing the request object which is a child of CoreRequest
			 * @sa CoreRequest
			 */
			virtual void request(QVariant request) = 0;
			/**
			 * @brief Close child processes
			 *
			 */
			virtual void exit() = 0;
			/**
			 * @brief Receive password used to get root priviliges using sudo utility
			 *
			 * @param password
			 */
            		virtual void receiveSudoPassword(QString password) = 0;//QSharedPointer<QByteArray> password) = 0;
		protected:
            /**
             * @brief Give the supported encryption algorithms names
             *
             * @param params Parameters of the function
             * @return QSharedPointer<GetEncryptionAlgorithmsResponse>, the response of the function
             */
            QSharedPointer<GetEncryptionAlgorithmsResponse> getEncryptionAlgorithms(QSharedPointer<GetEncryptionAlgorithmsRequest> params = QSharedPointer<GetEncryptionAlgorithmsRequest>(), bool emitResponse = true);
            /**
             * @brief Give the supported key derivation functions names
             *
             * @param params Parameters of the function
             * @return QSharedPointer<GetDerivationFunctionsResponse>, the response of the function
             */
            QSharedPointer<GetDerivationFunctionsResponse> getDerivationFunctions(QSharedPointer<GetDerivationFunctionsRequest> params = QSharedPointer<GetDerivationFunctionsRequest>(), bool emitResponse = true);
			/**
			 * @brief Give the availables storage devices on the computer (Disks, USB keys, etc)
			 *
			 * @param params Parameters of the function
			 * @return QSharedPointer<GetHostDevicesResponse>, the response of the function
			 */
			QSharedPointer<GetHostDevicesResponse> getHostDevices(QSharedPointer<GetHostDevicesRequest> params = QSharedPointer<GetHostDevicesRequest>(), bool emitResponse = true);
			/**
			 * @brief Give the list of currently mounted volumes
			 *
			 * @param params Parameters of the function
			 * @return QSharedPointer<GetMountedVolumesResponse>, the response of the function
			 */
			QSharedPointer<GetMountedVolumesResponse> getMountedVolumes(QSharedPointer<GetMountedVolumesRequest> params = QSharedPointer<GetMountedVolumesRequest>(), bool emitResponse = true);
            /**
             * @brief Create a random key file
             *
             * @param params Parameters of the function
             * @return QSharedPointer<CreateKeyFileResponse>, the response of the function
             */
            QSharedPointer<CreateKeyFileResponse> createKeyFile(QSharedPointer<CreateKeyFileRequest> params, bool emitResponse = true);

			/**
			 * @brief Give the list of mounted filesystems. It can filter only filesystems referring to the given device, or mounted in a specific mountpoint.
			 *
			 * @param devicePath Path of the device we want to get the list of corresponding mounted filesystems.
			 * @param mountPoint Path of the mountpoint we want to get the list of corresponding mounted filesystems.
			 * @return QList<QSharedPointer<MountedFilesystem> > the list of mounted filesystems
			 */
			QList<QSharedPointer<MountedFilesystem>> getMountedFilesystems(const QFileInfo &devicePath = QFileInfo(), const QFileInfo &mountPoint = QFileInfo());
            /**
             * @brief Return the encryption algorithm object corresponding to the given name
             *
             * @param algorithm encryption algorithm name
             * @return QSharedPointer<EncryptionAlgorithm> encryption algorithm object
             */
            QSharedPointer<EncryptionAlgorithm> getEncryptionAlgorithm(QString algorithm);
            /**
             * @brief Return the key derivation function object corresponding to the given name
             *
             * @param function key derivation function name
             * @return QSharedPointer<Pkcs5Kdf> key derivation function object
             */
            QSharedPointer<Pkcs5Kdf> getDerivationKeyFunction(QString function);
            /**
             * @brief Return the directory where the given device is mounted
             *
             * @param devicePath Path of the device file (/dev/loopN)
             * @return QSharedPointer<QFileInfo> Path where the given device is mounted
             * @exception DeviceNotMounted
             */
            QSharedPointer<QFileInfo> getDeviceMountPoint(const QSharedPointer<QFileInfo> &devicePath);
            /**
             * @brief Check if the given path correspond to an host device on the computer
             *
             * @param path Path of the file to check
             * @return bool True if the given path correspond to an host device on the computer.
             */
            bool isDevice(QString path);
			/**
			 * @brief Check if the given volume is currently mounted
			 *
			 * @param volumeFile Path of the volume file
			 * @return bool True if the given volume is currently mounted
			 */
			bool isVolumeMounted(QSharedPointer<QFileInfo> volumeFile);
            /**
             * @brief Give the next available fuse mountpoint (/tmp/.gostcrypt_aux_mntN)
             *
             * @return QSharedPointer<QFileInfo> Path of the next available fuse mountpoint
             */
            QSharedPointer<QFileInfo> getFreeFuseMountPoint();
            /**
             * @brief Create a random file. (Used for volume and key file creation)
             *
             * @param path Path of the random file to create
             * @param size Size of the random file to create
             * @param algorithm Name of the cipher algorithm to use for random generation
             * @param random If False null bytes will be used instead of random data
             */
            void createRandomFile(QSharedPointer<QFileInfo> path, quint64 size, QString algorithm = "", bool random = true);
            /**
             * @brief Set a random key for the given cipher algorithm. This function is used by createRandomFile
             *
             * @param encryptionAlgorithm Name of the cipher algorithm for which to change the key
             * @sa CoreBase::createRandomFile
             */
            void randomizeEncryptionAlgorithmKey (QSharedPointer <EncryptionAlgorithm> encryptionAlgorithm) const;
            /**
             * @brief Give the next available default mountpoint for the given user
             *
             * @param userId UserID of the user for which we want the next available default mountpoint
             * @return QSharedPointer<QFileInfo> Path of the next available default mountpoint
             */
            QSharedPointer<QFileInfo> getFreeDefaultMountPoint(uid_t userId);
            /**
             * @brief Try to process the given request. Only requests which don't need root privilileges will be handled.
             *
             * @param r QVariant containing the request to process (child of CoreRequest)
             * @return bool True if the request has been processed, False otherwise. (Can return False if the request need root privileges)
             */
            bool processNonRootRequest(QVariant r);

			uid_t getUserId(QString username);
			gid_t getGroupId(QString groupname);

		signals:
			DEC_REQUEST_SIGNAL(CreateVolume);
			DEC_REQUEST_SIGNAL(MountVolume);
			DEC_REQUEST_SIGNAL(DismountVolume);
			DEC_REQUEST_SIGNAL(ChangeVolumePassword);
			DEC_REQUEST_SIGNAL(GetEncryptionAlgorithms);
			DEC_REQUEST_SIGNAL(GetDerivationFunctions);
			DEC_REQUEST_SIGNAL(GetHostDevices);
			DEC_REQUEST_SIGNAL(GetMountedVolumes);
			DEC_REQUEST_SIGNAL(CreateKeyFile);
            DEC_REQUEST_SIGNAL(ProgressUpdate);
			/**
			 * @brief Signal emitted when the program can exit (when the coreservice is closed)
			 *
			 */
			void exited();
			/**
			 * @brief Signal emited when the password used to get root priviliges using sudo utility is necessary, that is to say when we need to launch the coreservice through sudo. This event can be emitted several times if the password is wrong.
			 *
			 */
			void askSudoPassword();
        };
        /**
         * @brief Give the core object to use (CoreRoot instance if current user is root, CoreUser instance otherwise).
         *
         * @return QSharedPointer<CoreBase> core object to use
         */
        QSharedPointer<CoreBase> getCore();
	}
}

#endif // COREBASE_H
