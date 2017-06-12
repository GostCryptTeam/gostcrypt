#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <modelclass.h>
//#include "volumeitemgraphic.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Register our component type with QML.
    qmlRegisterType<ModelClass>("gostcrypt.modelclass", 1, 0, "ModelClass");
  //  qmlRegisterType<VolumeItemGraphic>("volumeItem", 1, 0, "volumeItem");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/UI/main.qml")));

    return app.exec();
}
