#include "GC_graphicuserinterface.h"
#include <iostream>
#include <QDebug>

GC_GraphicUserInterface::GC_GraphicUserInterface(QObject * parent)
    : QObject(parent)
{

}

void GC_GraphicUserInterface::GC_receive(const QString& str)
{
#ifdef QT_DEBUG
    qDebug() << str;
#endif
}


void GC_GraphicUserInterface::GC_receiveMount()
{
#ifdef QT_DEBUG
    qDebug() << "Monter";
#endif
}

void GC_GraphicUserInterface::GC_receiveAutoMount()
{
#ifdef QT_DEBUG
    qDebug() << "Monter auto";
#endif
	// Voir Main/Forms/MainFrame.cpp:530
}

void GC_GraphicUserInterface::GC_receiveDismountAll()
{
#ifdef QT_DEBUG
    qDebug() << "Tout démonter";
#endif
}
