#include "CoreUser.h"
#include <QThread>

namespace GostCrypt {
	namespace NewCore {
		CoreUser::CoreUser(QObject *parent) : CoreBase(parent)
		{
			connect(&csh, SIGNAL(sendResponse(QVariant&)), this, SLOT(receiveResponse(QVariant&)));
			connect(&csh, SIGNAL(askSudoPassword()), this, SIGNAL(askSudoPassword()));
			connect(this, SIGNAL(sendSudoPassword(QSharedPointer<QByteArray>)), &csh, SLOT(receiveSudoPassword(QSharedPointer<QByteArray>)));
		}

		void CoreUser::exit()
		{
			ExitParams request;

			if(csh.isRunning()) {
				qDebug() << "Sending exit request";
				connect(&csh, SIGNAL(workerProcessFinished()), this, SIGNAL(exited()));
				csh.sendToCoreService(QVariant::fromValue(request));
			} else {
				// The main loop was not started, so an imediate call to app.quit() would not be working.
				QMetaObject::invokeMethod(this, "exited", Qt::QueuedConnection);
			}
		}

		void CoreUser::request(QVariant r)
		{
			//TODO add other non-root requests
			if(r.canConvert<QSharedPointer<GetMountedVolumesParams>>()) {
				QSharedPointer<GetMountedVolumesParams> request;
				QSharedPointer<GetMountedVolumesResponse> response;
				request = r.value<QSharedPointer<GetMountedVolumesParams>>();
				response = getMountedVolumes(request);
				emit sendGetMountedVolumes(response);
			} else {
				csh.sendToCoreService(r);
			}
		}

		void CoreUser::receiveSudoPassword(QSharedPointer<QByteArray> password)
		{
			emit sendSudoPassword(password);
		}

		void CoreUser::receiveResponse(QVariant &r)
		{
			//TODO add all responses
			if(r.canConvert<QSharedPointer<GetMountedVolumesResponse>>()) {
				QSharedPointer<GetMountedVolumesResponse> response;
				response = r.value<QSharedPointer<GetMountedVolumesResponse>>();
				emit sendGetMountedVolumes(response);
			} else if(r.canConvert<QSharedPointer<MountVolumeResponse>>()) {
				QSharedPointer<MountVolumeResponse> response;
				response = r.value<QSharedPointer<MountVolumeResponse>>();
				emit sendMountVolume(response);
			} else if(r.canConvert<QSharedPointer<DismountVolumeResponse>>()) {
				QSharedPointer<DismountVolumeResponse> response;
				response = r.value<QSharedPointer<DismountVolumeResponse>>();
				emit sendDismountVolume(response);
			} else if(r.canConvert<QSharedPointer<CreateVolumeResponse>>()) {
				QSharedPointer<CreateVolumeResponse> response;
				response = r.value<QSharedPointer<CreateVolumeResponse>>();
				emit sendCreateVolume(response);
			} else {
				qDebug() << "Unknow object : " << r.typeName();
			}
		}
	}
}
