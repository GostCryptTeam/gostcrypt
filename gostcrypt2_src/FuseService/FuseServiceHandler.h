#ifndef FUSESERVICEHANDLER_H
#define FUSESERVICEHANDLER_H
//#define FUSE_SERVICE_DEBUG
#include "Core/ServiceHandler.h"
#include "Core/CoreRequest.h"
#include "Core/CoreRequest.h"
#include "FuseService/FuseService.h"
#include <QSharedPointer>

namespace GostCrypt {
	namespace FuseDriver {
		/**
		 * @brief Class used to create and communicate with the FUSE service in charge of creating the FUSE mount using the FUSE library
		 *
		 */
		class FuseServiceHandler : public GostCrypt::Core::ServiceHandler
		{
			Q_OBJECT
		public:
			FuseServiceHandler();
			/**
			 * @brief Create the FUSE mount to start mounting the volume
			 *
			 * @param request Mount request containing all necessary information to mount the volume
			 */
			void mount(QSharedPointer<Core::MountVolumeRequest> request);
		private slots:
			/**
			 * @brief Slot called by the FUSE service when it finished creating the FUSE mountpoint
			 *
			 * @param response Response from the FUSE service containing some information about the FUSE mount
			 */
			void receiveResponse(QVariant &response);
			/**
			 * @brief Slot called when the FUSE service exited
			 *
			 */
			void done();
		private:
			QSharedPointer<Core::MountVolumeResponse> response; /**< TODO: Response from the FUSE service containing some information about the FUSE mount */
			QSharedPointer<Core::MountVolumeRequest> params; /**<  Mount request containing all necessary information to mount the volume */
			bool succeed; /**< TODO: describe */
		signals:
			/**
			 * @brief Signal emitted when FuseServiceHandler finished to create the FUSE mount, used to launch next steps to mount the volume
			 *
			 * @param params Mount request containing all necessary information to mount the volume
			 * @param response Response from the FUSE service containing some information about the FUSE mount
			 */
			void continueMountVolume(QSharedPointer<Core::MountVolumeRequest> params, QSharedPointer<Core::MountVolumeResponse> response);
		};
	}
}

#endif // FUSESERVICEHANDLER_H
