#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <modelclass.h>
#include "GraphicUserInterface.h"
#include <QQuickView>
//#include "volumeitemgraphic.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    GraphicUserInterface ui;
    // Register our component type with QML.
    qmlRegisterType<ModelClass>("gostcrypt.modelclass", 1, 0, "ModelClass");
  //  qmlRegisterType<VolumeItemGraphic>("volumeItem", 1, 0, "volumeItem");

    //*/
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/UI/main.qml")));//*/

    //QObject::connect(engine, SIGNAL(mountVolume(QString, QString)), &ui, SLOT(receiveMount(QString,QString)));
    engine.connect(engine, SIGNAL(mountVolume(QString, QString)), &ui, SLOT(receiveMount(QString,QString)));
    return app.exec();
}
