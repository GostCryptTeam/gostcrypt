#ifndef CORESERVICEHANDLER_H
#define CORESERVICEHANDLER_H

#include <QObject>
#include <QCoreApplication>
#include <QDataStream>
#include <QProcess>
#include <QVariant>
#include <QQueue>
#include "ServiceHandler.h"

namespace GostCrypt {
    namespace Core {
        /**
         * @brief Class handling the CoreService process used in the UI process to launch, communicate and stop the CoreService process. This process is in charge of executing the GostCrypt actions that require root privileges.
         *
         */
        class CoreServiceHandler : public ServiceHandler
		{
			Q_OBJECT
		public:
            /**
             * @brief Default constructor
             */
			CoreServiceHandler();
		public slots:
            /**
             * @brief Slot called when the password of the current user has been gathered by the UI. This password will be used with the sudo utility in order to launch the CoreService with root privileges. The password will be directly written to the master terminal in which the CoreService process is launched
             *
             * @param password Password of the current user as raw bytes.
             */
            void receiveSudoPassword(QSharedPointer<QByteArray> password);
            /**
             * @brief Check that the Core Service is successfully launched (that is to say that the InitResponse has been received). If this is not the case maybe the password provided by the UI was wrong. There for it will ask the UI again for the user password.
             *
             */
            void checkInitReceived();
		private:
            /**
             * @brief Slots called when the core service process exit unexpectedly before the InitResponse has been received. It most likely means that all the user password were wrong and that the number of try has exceeded.
             *
             */
            virtual void processCrashedBeforeInitialization();
		signals:
            /**
             * @brief Signal emitted when the user password is required. Either just after the sudo utility is launched or after the password provided hasn't worked
             *
             */
            void askSudoPassword();
		};
	}
}

#endif // CORESERVICEHANDLER_H
