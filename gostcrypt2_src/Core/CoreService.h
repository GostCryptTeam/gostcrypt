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

#define DEF_RESPONSE_SLOT(request) \
	void send ## request (QSharedPointer< request ## Response> response) { \
		sendResponse(QVariant::fromValue(response)); \
	}
#define CONNECT_RESPONSE_SLOT(request) \
	connect(core.data(), SIGNAL(send ## request (QSharedPointer< request ## Response>)), this, SLOT(send ## request (QSharedPointer< request ## Response>)))

namespace GostCrypt {
    namespace Core {
        /**
         * @brief Class corresponding to CoreService process, executing request which require root privileges. It is in charge of parsing request coming from its standard input, execute the action and forward back its results to the standard output.
         *
         */
        class CoreService : public Service
		{
			Q_OBJECT
		public:
            /**
             * @brief Default constructor
             *
             */
			CoreService() : Service("CoreService") {}
		private:
            QSharedPointer<CoreRoot> core; /**< Core object containing methods for each action handled by GostCrypt (mount, dismount, create a volume, etc) The CoreRoot implementation is used since the CoreService process is launched by the root user. */
            /**
             * @brief Connect the signals to the slots between CoreService and the core object. This signals/slots are in charge of dealing with the received requests and the responses to send back.
             *
             */
            virtual void connectRequestHandlingSignals();
            /**
             * @brief Make all the object that need to be transmitted between the processes serializables ( Requests, Responses, Exceptions)
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
