#include "GraphicUserInterface.h"
#include <iostream>
#include <QDebug>

GraphicUserInterface::GraphicUserInterface(QObject * parent)
    : QObject(parent)
{

}

void GraphicUserInterface::receive(const QString& str)
{
#ifdef QT_DEBUG
    qDebug() << str;
#endif
}


void GraphicUserInterface::receiveMount(const QString& aPath, const QString& aPassword)
{
#ifdef QT_DEBUG
    qDebug() << "Monter : " << aPath << " " << aPassword;
#endif
}

void GraphicUserInterface::receiveAutoMount()
{
#ifdef QT_DEBUG
    qDebug() << "Monter auto";
#endif
	// Voir Main/Forms/MainFrame.cpp:530
}

void GraphicUserInterface::receiveDismountAll()
{
#ifdef QT_DEBUG
    qDebug() << "Tout démonter";
#endif
}

void GraphicUserInterface::receiveDismount(const QString& aStr)
{
#ifdef QT_DEBUG
    qDebug() << "On démonte " << aStr;
#endif
}
