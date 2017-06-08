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
#include "Core/Core.h"
#include "Core/Unix/CoreService.h"
#include "Platform/SystemLog.h"

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
    QApplication GC(argc, argv); /*!< Main application */
    Window interface; /*!< GostCrypt UI*/

    interface.show();
    return GC.exec();
}
