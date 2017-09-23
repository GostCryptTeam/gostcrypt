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
            qmlRequest("mountedvolumes", "");
        }

        //Receiving the list of mounted volumes
        onSPrintGetMountedVolumes: {
            subWindow.catchClose();
            //clear all the volumes
            pageLoader.item.clearVolumes();
            //Updating the model
            manageModel(volumes);
        }

        onGetSudoPassword: {
            app.toggleSudo(1)
        }

        onSPrintDismountVolume: {
            qmlRequest("mountedvolumes", "")
        }

        onSendError: {
            openErrorMessage(aTitle, aContent);
        }
    }

    function manageModel(volumes)
    {
        app.model = volumes;
        //Delete all the model entries that are not in the volumes array
        for(var k in volumes)
        {
            pageLoader.item.loadVolume(
                        volumes[k]["mountPoint"],
                        volumes[k]["algo"],
                        volumes[k]["volumePath"],
                        volumes[k]["volumeSize"]);
        }
    }
}
