#ifndef CORESERVICE_H
#define CORESERVICE_H

#include <QObject>
#include <QCoreApplication>
#include <QDataStream>
#include <QFile>
#include <QVariant>
#include "CoreResponse.h"
#include "CoreRoot.h"
#include "Service.h"

//#define DEBUG_CORESERVICE

#define DEF_RESPONSE_SLOT(request) \
	void send ## request (QSharedPointer< request ## Response> response) { \
		sendResponse(QVariant::fromValue(response)); \
	}
#define CONNECT_RESPONSE_SLOT(request) \
	connect(core.data(), SIGNAL(send ## request (QSharedPointer< request ## Response>)), this, SLOT(send ## request (QSharedPointer< request ## Response>)))

namespace GostCrypt {
    namespace NewCore {
        /**
         * @brief Class corresponding to CoreService process, handling request which require root permission
         *
         */
        class CoreService : public Service
		{
			Q_OBJECT
		public:
			CoreService() : Service("CoreService") {}
		private:
            QSharedPointer<CoreRoot> core; /**< TODO: describe */
            /**
             * @brief
             *
             */
            virtual void connectRequestHandlingSignals();
            /**
             * @brief
             *
             */
            virtual void initSerializables();
		public slots:
			DEF_RESPONSE_SLOT(CreateVolume)
			DEF_RESPONSE_SLOT(MountVolume)
			DEF_RESPONSE_SLOT(DismountVolume)
			DEF_RESPONSE_SLOT(ChangeVolumePassword)
			DEF_RESPONSE_SLOT(GetEncryptionAlgorithms)
			DEF_RESPONSE_SLOT(GetDerivationFunctions)
			DEF_RESPONSE_SLOT(GetHostDevices)
			DEF_RESPONSE_SLOT(GetMountedVolumes)
			DEF_RESPONSE_SLOT(CreateKeyFile)
		};
	}
}

#endif // CORESERVICE_H
