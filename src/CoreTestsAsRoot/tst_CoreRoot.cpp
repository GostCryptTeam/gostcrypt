#include <QtTest>
#include <QCoreApplication>
#include "CoreBase.h"
#include "catch/catch.hpp"

using namespace GostCrypt::Core;

TEST_CASE("CoreRoot", "[Core]")
{
    QSharedPointer<CoreBase> core = getCore();
    SECTION(" can exit", "[exit]")
    {
        QSignalSpy spy(core.data(), SIGNAL(exited()));
        core->exit();
        CHECK(spy.wait());
        CHECK(spy.count() == 1);
    }
    SECTION(" can mount volume", "[mountVolume]")
    {
        QSignalSpy spy(core.data(), SIGNAL(sendMountVolume(QSharedPointer<MountVolumeResponse>)));

        QSharedPointer <GostCrypt::Core::MountVolumeRequest> mountRequest(new GostCrypt::Core::MountVolumeRequest);

        mountRequest->doMount = true;
        mountRequest->isDevice = false;
        mountRequest->mountPoint = QFileInfo("/media/gostcrypt");
        mountRequest->password.reset(new QByteArray("password"));
        mountRequest->path = QFileInfo("./volume");


        core->request(QVariant::fromValue(mountRequest));

        CHECK(spy.wait());
        CHECK(spy.count() == 1);
        QSharedPointer<MountVolumeResponse> response = spy.takeFirst().first().value<QSharedPointer<MountVolumeResponse>>();
        CHECK(response->readOnlyFailover == false);
    }
}
