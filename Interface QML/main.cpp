#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "modelclass.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Register our component type with QML.
    qmlRegisterType<ModelClass>("gostcrypt.modelclass", 1, 0, "ModelClass");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/UI/main.qml")));

    return app.exec();
}
