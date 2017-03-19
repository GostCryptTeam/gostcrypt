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


void GraphicUserInterface::receiveMount()
{
#ifdef QT_DEBUG
    qDebug() << "Monter";
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
