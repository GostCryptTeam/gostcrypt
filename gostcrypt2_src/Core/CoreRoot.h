#ifndef COREROOT_H
#define COREROOT_H

#include <QSharedPointer>
#include <fstream>
#include <QProcess>
#include <QByteArray>
#include "CoreBase.h"
#include "CoreRequest.h"
#include "CoreResponse.h"
#include "FuseService/FuseServiceHandler.h"
#include "RandomNumberGenerator.h"

namespace GostCrypt {
	namespace Core {
        /**
         * @brief Class defining the core object which manage any action requiring root permissions handled by GostCrypt (mount, dismount, create a volume, etc)
         *
         */
        class CoreRoot : public CoreBase
		{
		Q_OBJECT
		public:
            /**
             * @brief Default Constructor
             *
             * @param parent parent Object
             */
            explicit CoreRoot(QObject *parent = nullptr);
		public slots:
            /**
             * @brief call when coreRoot have to be closed
             *
             */
            virtual void exit();
            /**
             * @brief Execute the given action request
             *
             * @param r QVariant containing the request object which is a child of CoreRequest
             */
            virtual void request(QVariant r);
            /**
             * @brief Receive the Sudo password to execute an action requiring root permission
             *
             * @param password GString containning the sudoer's password
             */
            virtual void receiveSudoPassword(QString password); //TODO
        private slots:
            /**
             * @brief Continue to mount the volume after the Fuze process being successfully launch
             *
             * @param params
             * @param response
             */
            void continueMountVolume(QSharedPointer<Core::MountVolumeRequest> params, QSharedPointer<Core::MountVolumeResponse> response);
            /**
             * @brief
             *
             * @param params
             * @param response
             */
            void continueFormatHidden(QSharedPointer<CreateVolumeRequest> params, QSharedPointer<CreateVolumeResponse> response);
            /**
             * @brief
             *
             * @param params
             * @param response
             */
            void finishCreateVolume(QSharedPointer<CreateVolumeRequest> params, QSharedPointer<CreateVolumeResponse> response);
            /**
             * @brief
             *
             * @param mountResponse
             */
            void continueMountFormat(QSharedPointer<MountVolumeResponse> mountResponse);
            /**
             * @brief
             *
             * @param dismountResponse
             */
            void continueDismountFormat(QSharedPointer<DismountVolumeResponse> dismountResponse);
		private:
            /**
             * @brief Mount a volume
             *
             * @param params Parameters of the function
             */
            void mountVolume(QSharedPointer<MountVolumeRequest> params);
            /**
             * @brief
             *
             * @param params
             * @return QSharedPointer<DismountVolumeResponse>
             */
            QSharedPointer<DismountVolumeResponse> dismountVolume(QSharedPointer<DismountVolumeRequest> params = QSharedPointer<DismountVolumeRequest>());
            /**
             * @brief
             *
             * @param params
             */
            void createVolume(QSharedPointer<CreateVolumeRequest> params);
            /**
             * @brief
             *
             * @param params
             * @return QSharedPointer<ChangeVolumePasswordResponse>
             */
            QSharedPointer<ChangeVolumePasswordResponse> changeVolumePassword(QSharedPointer<ChangeVolumePasswordRequest> params);

            /**
             * @brief
             *
             * @param file
             * @param params
             * @param layout
             * @param containersize
             */
            void writeHeaderToFile(fstream &file, QSharedPointer<CreateVolumeRequest::VolumeParams> params, QSharedPointer<VolumeLayout> layout, quint64 containersize);
            /**
             * @brief
             *
             * @param volume
             * @param password
             * @param keyfiles
             * @param filesystem
             * @param parentParams
             * @param parentResponse
             */
            void mountFormatVolume(QSharedPointer<QFileInfo> volume, QSharedPointer<QByteArray> password, QSharedPointer<QList<QSharedPointer<QFileInfo>>> keyfiles, QString filesystem, QSharedPointer<CreateVolumeRequest> parentParams, QSharedPointer<CreateVolumeResponse> parentResponse);
            uid_t realUserId; /**< TODO: describe */
            gid_t realGroupId; /**< TODO: describe */
            GostCrypt::FuseDriver::FuseServiceHandler fuseServiceHandler; /**< TODO: describe */
        signals:
            /**
             * @brief
             *
             * @param params
             * @param response
             */
            void formatVolumeDone(QSharedPointer<CreateVolumeRequest> params, QSharedPointer<CreateVolumeResponse> response);

		};
	}
}

#endif // COREROOT_H
