#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <connectSignals.h>
#include <GraphicUserInterface.h>
#include "Core/Core.h"
#include "Core/Unix/CoreService.h"
#include "Platform/SystemLog.h"

int main(int argc, char *argv[])
{
   /* if (argc > 1 && strcmp (argv[1], GST_CORE_SERVICE_CMDLINE_OPTION) == 0)
    {
        // Process elevated requests
        try
        {
            GostCrypt::CoreService::ProcessElevatedRequests();
            return 0;
        }
        catch (exception &e)
        {
    #ifdef QT_DEBUG
            GostCrypt::SystemLog::WriteException (e);
    #endif
        }
        catch (...)	{ }
        return 1;
    }*/

    QGuiApplication app(argc, argv);
    GraphicUserInterface ui;
    ConnectSignals cs(&ui);

    QQmlApplicationEngine engine;
    QQmlContext* ctx = engine.rootContext();
    ctx->setContextProperty("ConnectSignals", &cs);
    engine.load(QUrl(QStringLiteral("qrc:/UI/main.qml")));

    return app.exec();
}
