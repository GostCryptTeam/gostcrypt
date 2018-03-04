#ifndef COREUSER_H
#define COREUSER_H

#include <QSharedPointer>
#include "CoreResponse.h"
#include "CoreRequest.h"
#include "CoreBase.h"
#include "CoreServiceHandler.h"

#define HANDLE_RESPONSE(requestName) \
    if(r.canConvert<QSharedPointer<requestName ## Response>>()) { \
		QSharedPointer<requestName ## Response> response; \
        response = r.value<QSharedPointer<requestName ## Response>>(); \
		emit send ## requestName (response); \
	}

namespace GostCrypt {
	namespace Core {
        /**
         * @brief Class defining the core object containing methods for each action handled by GostCrypt (mount, dismount, create a volume, etc). This class is used by the process if it does not have root privileges. Therefore this class does not directly implement all GostCrypt actions. GostCrypt actions that do not require root privileges are inherited from the parent class CoreBase and the GostCrypt actions that require root privileges are forwarded to the CoreService process (executed with root privileges) thanks to the CoreServiceHandler.
         *
         */
        class CoreUser : public CoreBase
		{
		Q_OBJECT
		public:
            /**
             * @brief Default constructor
             *
             * @param parent Parent object (See Qt documentation)
             */
            explicit CoreUser(QObject *parent = nullptr);
        public slots:
            /**
             * @brief Terminate all processes launched by this object and emit the signal exited when it is done.
             *
             */
            virtual void exit();
            /**
             * @brief Execute the GostCrypt action corresponding to the type of the CoreRequest sub-class given in r
             *
             * @param r QVariant containing the request object which is a child of CoreRequest. Its type indicate the action type to execute and its content provide the parameters for this action
             */
            virtual void request(QVariant r);
            /**
             * @brief Slot called when the password of the current user has been gathered by the UI. This password will be used with the sudo utility in order to launch the CoreService with root privileges. The password will be directly written to the master terminal in which the CoreService process is launched
             *
             * @param password Password of the current user as raw bytes.
             */
            virtual void receiveSudoPassword(QString password); //TODO
        private slots:
            /**
             * @brief Emit the signal corresponding to the CoreResponse stored in r to inform the UI. Slot called when a reponse from an action requiring root is received from the CoreService process.
             *
             * @param r QVariant containing the response object which is a child of CoreResponse. Its type indicate the action type executed and its content provide the results from this action
             */
            void receiveResponse(QVariant &r);
        private:
            CoreServiceHandler csh; /**< Oject managing the CoreService process to launch, communicate and stop the CoreService process. This process is in charge of executing the GostCrypt actions that require root privileges.
 */
		signals:
            /**
             * @brief Signal emitted to send the user password to the CoreServiceHandler when it is received from the UI
             * TODO is it really usefull ?
             * @param password Password of the current user as raw bytes.
             */
            void sendSudoPassword(QSharedPointer<QByteArray> password);
		};

	}
}

#endif // COREUSER_H
