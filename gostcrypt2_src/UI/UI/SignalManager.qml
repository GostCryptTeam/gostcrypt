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

        onSprintGetMountedVolumes: {
            subWindow.catchClose();
            pageLoader.item.clearVolumes();
            manageModel(l);
        }

        onGetSudoPassword: {
            app.toggleSudo(1)
        }

        onSprintDismountVolume: {
            qmlRequest("mountedvolumes", "")
        }

        onSprintMountVolume: {
            qmlRequest("mountedvolumes", "")
        }

        onSprintSendError: {
            /*switch(aTitle)
            {
            case "badvolumepassword":
                break;
            }*/
            openErrorMessage(l[0], l [1]);
        }

        onSprintGetEncryptionAlgorithms:
        {
            subWindow.loadedItem.page.getAlgos(l);
            subWindow.loadedItem.page.used[0] = l[0];
            subWindow.loadedItem.page.used[1] = l[1];
        }

        onSprintGetHostDevices:
        {
            var i = 0
            for(var a in l)
            {
                subWindow.getLoader().item.addHostDevice(
                  {
                    number: i,
                    mountPoint: l[i]["mountPoint"],
                    path: l[i]["path"],
                    size: l[i]["size"]
                  })
                i++;
            }
        }

        onSprintProgressUpdate:
        {
            console.log("Mise à jour avec l'id " + l[0]["id"] + " --->" + l[0]["progress"]*100);
            notifs.updateNotification(l[0]["id"],l[0]["progress"]*100,notifications[l[0]["id"]][0],notifications[l[0]["id"]][1]);
            if(notifs.visible === false) title.showIcon(true);
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
