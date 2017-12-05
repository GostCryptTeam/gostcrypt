import QtQuick 2.0

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
            //Getting the list of mounted volumes, algorithm(s), filesystem(s) and hash(s)
            qmlRequest("mountedvolumes", "");
            qmlRequest("algorithms", "");
            qmlRequest("hashs", "");
            qmlRequest("filesystems", "");
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
            openErrorMessage(l[0], l[1]);
        }

        onSprintGetFileSystem: {
            app.filesystems = l
        }

        onSprintGetDerivationFunctions: {
            app.hashs = l
        }

        onSprintGetEncryptionAlgorithms:
        {
            app.algorithms = l
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
            console.log(l[0]["progress"]*100);
            if(l === undefined) return;
            notifs.updateNotification(l[0]["id"],l[0]["progress"]*100,notifications[l[0]["id"]-1][0],notifications[l[0]["id"]-1][1]);
            if(notifs.visible === false) title.showIcon(true);
            notifPreview.n = notifications[l[0]["id"]-1][0]
            notifPreview.p = l[0]["progress"]*100
            if(timerNotifPreview.running === false) timerNotifPreview.start();
        }

        onSprintBackupHeaderComplete:
        {
            openErrorMessage(qsTr("Backup complete !"), qsTr("Volume header backup has been successfully created;<br><br>IMPORTANT: "+
                                                             "Restoring the volume header using this backup will also restore the current "+
                                                             "volume password. Moreover, if keyfile(s)are/is necessary to mount the volume, "+
                                                             "the same keyfile(s) will be necessary to mount the volume again when the volume "+
                                                             "header is restored.<br><br>Warning: This volume header backup may be used to restore "+
                                                             "the header ONLY of this particular volume. If you use this header backup to resotre a "+
                                                             "header of a different volume, you will be able to mount the volume, but you will NOT be "+
                                                             "able to decrypt any data stored in the volume (because you will change its master key)."))
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
