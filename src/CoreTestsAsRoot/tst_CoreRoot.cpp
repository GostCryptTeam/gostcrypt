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
        core->exit();
    }
}
