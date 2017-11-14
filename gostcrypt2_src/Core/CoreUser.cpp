#include "CoreUser.h"
#include <QThread>
#include "Service.h"


namespace GostCrypt {
	namespace Core {
		CoreUser::CoreUser(QObject *parent) : CoreBase(parent)
		{
			connect(&csh, SIGNAL(sendResponse(QVariant&)), this, SLOT(receiveResponse(QVariant&)));
			connect(&csh, SIGNAL(askSudoPassword()), this, SIGNAL(askSudoPassword()));
			connect(this, SIGNAL(sendSudoPassword(QSharedPointer<QByteArray>)), &csh, SLOT(receiveSudoPassword(QSharedPointer<QByteArray>)));
			connect(&csh, SIGNAL(exited()), this, SIGNAL(exited()));

		}

		void CoreUser::exit()
		{
			if(csh.isRunning()) {
				csh.exit();
			} else {
				// The main loop was not started, so an imediate call to app.quit() would not be working.
				QMetaObject::invokeMethod(this, "exited", Qt::QueuedConnection);
			}
		}

		void CoreUser::request(QVariant r)
		{
		//TODO add other non-root requests
			if(!processNonRootRequest(r))
				csh.sendToService(r);
		}

        //TODO : remove the QSTRING with a c++ password class
        void CoreUser::receiveSudoPassword(QString password)//QSharedPointer<QByteArray> password)
		{
            emit sendSudoPassword(QSharedPointer<QByteArray>(new QByteArray(password.toLocal8Bit())));
        }

		void CoreUser::receiveResponse(QVariant &r)
		{
			HANDLE_RESPONSE(MountVolume)
			else HANDLE_RESPONSE(DismountVolume)
			else HANDLE_RESPONSE(CreateVolume)
			else HANDLE_RESPONSE(ChangeVolumePassword)
			else HANDLE_RESPONSE(ProgressUpdate)
			else throw UnknowResponseException(r.typeName());
		}
	}
}
