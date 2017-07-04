#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <connectSignals.h>
#include <QIcon>
#include <GraphicUserInterface.h>
#include "UserSettings.h"
#include "Core/Core.h"
#include "Core/Unix/CoreService.h"
#include "Platform/SystemLog.h"
#include "DragWindowProvider.h"
#include "volumecreation.h"

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp (argv[1], GST_CORE_SERVICE_CMDLINE_OPTION) == 0)
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
    }

    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/logo_gostcrypt.png"));
    GraphicUserInterface ui;
    ConnectSignals cs(&ui);
    UserSettings settings;
    DragWindowProvider drag;
    VolumeCreation wizard;

    QQmlApplicationEngine engine;
    QQmlContext* ctx = engine.rootContext();
    ctx->setContextProperty("ConnectSignals", &cs);
    ctx->setContextProperty("UserSettings", &settings);
    ctx->setContextProperty("DragWindowProvider", &drag);
    ctx->setContextProperty("Wizard", &wizard);

    engine.load(QUrl(QStringLiteral("qrc:/UI/main.qml")));
    cs.init(engine.rootObjects().first());

    return app.exec();
}
