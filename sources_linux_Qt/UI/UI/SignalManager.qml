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

        onSPrintGetMountVolume: {
            subWindow.catchClose();
            pageLoader.item.clearVolumes();
            manageModel(volumes);
        }

        onGetSudoPassword: {
            app.toggleSudo(1)
        }

        onSPrintDismountVolume: {
            qmlRequest("mountedvolumes", "")
        }

        onSPrintMountVolume: {
            qmlRequest("mountedvolumes", "")
        }

        onSendError: {
            /*switch(aTitle)
            {
            case "badvolumepassword":
                break;
            }*/
            openErrorMessage(aTitle, aContent);
        }

        onSPrintGetEncryptionAlgorithms:
        {
            console.log(algos);
            subWindow.loadedItem.page.getAlgos(algos);
            subWindow.loadedItem.page.used[0] = algos[0];
            subWindow.loadedItem.page.used[1] = algos[1];
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
