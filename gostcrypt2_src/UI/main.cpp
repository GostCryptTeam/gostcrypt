#include "GraphicInterface.h"
#include "Platform/SystemLog.h"
#include "CmdLineInterface.h"
#include "NewCore/CoreService.h"
#include "NewCore/CoreException.h"
#include "NewFuseService/FuseService.h"

int main(int argc, char *argv[])
{
    qSetMessagePattern("%{appname}: %{message}");
    if (argc > 1)
    {
        if(strcmp (argv[1], "coreservice") == 0){
            // Process elevated requests
			GostCrypt::NewCore::CoreService cs;
			return cs.start(argc, argv);
        } else if(strcmp(argv[1], "fuseservice") == 0) {
			GostCrypt::NewFuseDriver::NewFuseService fs;
			return fs.start(argc, argv);
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
    }else{
        MyGuiApplication app(argc,argv);
        GraphicInterface gi(&app);
        return gi.start();
    }
}
