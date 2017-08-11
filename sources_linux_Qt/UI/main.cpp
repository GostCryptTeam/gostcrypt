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
#include "CmdLineInterface.h"
#include "NewCore/CoreService.h"

int main(int argc, char *argv[])
{
    qSetMessagePattern("%{appname}: %{message}");
    if (argc > 1)
    {
        if(strcmp (argv[1], GST_CORE_SERVICE_CMDLINE_OPTION) == 0){
            // Process elevated requests
			GostCrypt::NewCore::CoreService cs;
			return cs.start(argc, argv);
        } else {
            #ifdef QT_DEBUG // QML debbuger has its own arguments
                if(!(argc == 2 && strncmp(argv[argc-1], "-qmljs", 6) == 0)){
            #endif
                    CmdLineInterface interface;
                    return interface.start(argc, argv);;
            #ifdef QT_DEBUG
                }
            #endif
        }
    }

    // no arguments, starting GUI

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
