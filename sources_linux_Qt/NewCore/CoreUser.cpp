#include "CoreUser.h"
#include <QThread>

namespace GostCrypt {
namespace NewCore {
        QSharedPointer<MountVolumeResponse> CoreUser::mountVolume(QSharedPointer<MountVolumeParams> params)
        {
            return forwardRoot(QVariant::fromValue(params)).value<QSharedPointer<MountVolumeResponse>>();
        }

        QSharedPointer<DismountVolumeResponse> CoreUser::dismountVolume(QSharedPointer<DismountVolumeParams> params)
        {
            return forwardRoot(QVariant::fromValue(params)).value<QSharedPointer<DismountVolumeResponse>>();
        }

        QSharedPointer<CreateVolumeResponse> CoreUser::createVolume(QSharedPointer<DismountVolumeParams> params)
        {
            return forwardRoot(QVariant::fromValue(params)).value<QSharedPointer<CreateVolumeResponse>>();
        }

        QSharedPointer<CreateKeyFileResponse> CoreUser::createKeyFile(QSharedPointer<CreateKeyFileParams> params)
        {
            return forwardRoot(QVariant::fromValue(params)).value<QSharedPointer<CreateKeyFileResponse>>();
        }

        QSharedPointer<ChangeVolumePasswordResponse> CoreUser::changeVolumePassword(QSharedPointer<ChangeVolumePasswordParams> params)
        {
            return forwardRoot(QVariant::fromValue(params)).value<QSharedPointer<ChangeVolumePasswordResponse>>();
        }

        QVariant CoreUser::forwardRoot(const QVariant params)
        {
            QVariant v;

            if(rootProcess.state() != QProcess::Running)
            {}    //start it
            rootProcessStream << params;

            while(v.isNull()) {
                rootProcessStream >> v;
                QThread::msleep(100);
                app->processEvents();
            }
            if(v.canConvert<QSharedPointer<CoreResponse>>())
                return v;
            if(v.canConvert<CoreException>())
                throw v.value<CoreException>();
            throw UnrecognisedResponseException(v);
        }

        void CoreUser::launchRootProcess()
        {
            char *password = "a";
            rootProcess.setProgram("/usr/bin/sudo");
            rootProcess.setArguments({QCoreApplication::applicationFilePath(), "coreservice"});
            rootProcess.setProcessChannelMode(QProcess::ForwardedErrorChannel);
            rootProcess.start();
            rootProcess.waitForStarted();
            rootProcess.write(password);
            rootProcessStream.setDevice(&rootProcess);
        }

        void CoreUser::sendAdminPassword()
        {

        }
	}
}
