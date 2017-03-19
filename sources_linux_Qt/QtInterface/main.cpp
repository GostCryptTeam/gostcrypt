/*!
 * \file main.cpp
 * \brief Main file to launch GostCrypt's UI
 * \author Antoine Hébert, Louis Béclair, William Lardier
 * \version 0.1
 * \date 02/07/2017
 *
 * GUI for GostCrypt 2.0
 *
 */

#include "Window.h"

#include <QApplication>
#include <QFile>
/*
#include <QIODevice>
#include <QStyleFactory>
#include <QFont>
//*/

int main(int argc, char *argv[])
{
    QApplication GC(argc, argv); /*!< Main application */
    Window interface; /*!< GostCrypt UI*/

    interface.show();
    return GC.exec();
}
