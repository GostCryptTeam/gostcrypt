import QtQuick 2.0
import "LoadVolume.js" as LoadVolume

Item {

    /*!
        \title Connections
        \brief QML slots that receive C++ signals
     */
    Connections {
        /*!
          Connects the C++ object (connectSignals.h) to the window
          */
        target: ConnectSignals;

        onConnectFinished: {
            qmlRequest("mountedvolumes", "dummy");
        }

        //Receiving the list of mounted volumes
        onSPrintGetMountedVolumes: {
            subWindow.catchClose();
            pageLoader.item.loadVolume(aMount, aAlgo, aPath, aSize);
        }

        onGetSudoPassword: {
            app.toggleSudo(1)
        }

        onSPrintDismountVolume: {
            for(var p in mountPoints)
                pageLoader.item.dismountVolume(mountPoints[p]);
        }


    }
}
