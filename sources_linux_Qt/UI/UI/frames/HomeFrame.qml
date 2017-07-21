import QtQml 2.0 as QML
import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import "../LoadVolume.js" as LoadVolume
import "../" as UI

Item {
    signal menuChanged(string name, int index)
    property QtObject mainWindow_
    /*!
        \class GSMainForm
        \brief Contains the background of the
        main window
    */
   /* GSMainForm {
        id: mainWindow
        Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }*/

    Item {
        id: mainWindow
        x: 0
        width: 790
        height: 530

        Rectangle {
            id: topBar
            x: 0
            y: 0
            width: 790
            height: 74
            color: palette.darkSecond
        }

        Rectangle {
            id: rectangle1
            x: 98
            y: 438
            width: 594
            height: 92
            color: palette.darkSecond
            border.width: 0
        }

        Rectangle {
            id: rectangle2
            x: 98
            y: 111
            width: 594
            height: 303
            color: palette.darkSecond
        }
         Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }

    MouseArea {
        id: area_
        x:0
        y:0
        width: mainWindow.width
        height: mainWindow.height
        propagateComposedEvents: true
        onClicked: {
            volumeToolsMenu.opacity = 0.0
        }
    }

    /*!
        \qmlclass Row
        \brief Contains the buttons "Mount volume" and "Create volume"
        These buttons are centered.
     */
    Row {
        id: rowWindow
        spacing: 70
        anchors.horizontalCenter: mainWindow.horizontalCenter
        topPadding: 18
        /*!
            \class GSButtonGreen_icon
            \brief Button that allow the user to mount
            a volume by opening a subwindow.
         */
        UI.GSButtonGreen_icon {
            text: qsTr("Mount Volume")
            /*!
            \qmlsignal onClicked
            \brief Open the subwindow by clicking on the "Mount volume"
            button. Animation when subwindow appears.
         */
            onClicked: {
                volumeToolsMenu.opacity = 0.0
                openSubWindow("dialogs/GSOpenVolume.qml", 'Open a GostCrypt volume', "Mount a volume", 429, {"name" : "", "value" : ""})
            }
        }
        /*!
        \class GSButtonBlue_icon
        \brief Button that opens the Gostcrypt volume creation
        wizard to create a volume.
     */
        UI.GSButtonBlue_icon {
            text: qsTr("Create Volume")
            onClicked: {
                volumeToolsMenu.opacity = 0.0
                openSubWindow("wizard/WizardManager.qml", 'GostCrypt Volume Creation Wizard', "Create a volume", 429, {"name" : "", "value" : ""})
            }
        }
        //Smooth fade-in/fade-out animation
        Behavior on anchors.horizontalCenterOffset { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }


    /*!
    \qmlclass Row
    \brief Contains the buttons "Auto Mount Device",
    "Dismount All" and "Volume Tools".
    These buttons are centered.
 */
    Row {
        id: rowBottom
        spacing: 22
        anchors.horizontalCenter: mainWindow.horizontalCenter
        anchors.bottom: mainWindow.bottom;
        anchors.bottomMargin: 28
        /*!
        \class GSButtonBordered
        \brief Auto-mount a device.
     */
        UI.GSButtonBordered {
            text: qsTr("Auto Mount Device")
            onClicked:
            {
                volumeToolsMenu.opacity = 0.0
                ConnectSignals.connectReceiveAutoMount("dummy")
            }
            color_:palette.green
        }
        /*!
        \class GSButtonBordered
        \brief Dismount all the devices currently mounted
     */
        UI.GSButtonBordered {
            text: qsTr("Dismount All")
            color_:palette.green
            onClicked: {
                volumeToolsMenu.opacity = 0.0
                ConnectSignals.connectReceiveDismountAll()
                listOfVolumes.clear()
                helpStart.visible = true
            }
        }
        /*!
            \class GSButtonBordered
            \brief Opens the volume tools options.
         */
        UI.GSButtonBordered {
            id: volumeTools
            text: qsTr("Volume Tools")
            color_:palette.blue
            //TODO : supprimer (tests)
            onClicked: {
                //LoadVolume.loadVolume("/media/volume", "GOST Grasshopper", "/home/user/myVolumes/volume", "5 MB");
                volumeToolsMenu.opacity = (volumeToolsMenu.opacity == 0.0) ? 1.0 : 0.0
                if(volumeToolsMenu.visible == false) volumeToolsMenu.visible = true
            }

        }
        Behavior on anchors.horizontalCenterOffset { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }



    /*!
        \qmlclass Item
        \brief Contains the gridview with all the mounted volumes.
     */
    Item {
        id: volumeListElement
        x: 98
        y: 111
        width: 594
        height: 303

        UI.GSVolumeItem {
            id:volumeDelegate
        }

        ListModel {
            id: listOfVolumes
        }

        GridView {
            id: grid
            anchors.fill: parent;
            cellWidth: grid.width/2;
            cellHeight: 100
            anchors.leftMargin: 35
            anchors.topMargin: 10
            anchors.bottomMargin: 10
            delegate: volumeDelegate
            focus: true
            model: listOfVolumes
            ScrollBar.vertical: ScrollBar { snapMode: ScrollBar.SnapOnRelease }
            snapMode: GridView.SnapToRow
            clip: true
            onCurrentIndexChanged: {
                console.log("Item select = " + currentIndex);
            }
        }
        Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }

    //Default content on start
    Item {
        id: helpStart
        x: 98
        y: 111
        width: 594
        height: 303
        visible: true
        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true
            onClicked: {
                volumeToolsMenu.opacity = 0.0
            }
        }
        Image {
            width: 594
            fillMode: Image.Pad
            y: 30
            horizontalAlignment: Image.AlignHCenter
            source: "../ressource/dragDrop.png"
        }
        Text {
            y: 120
            font.pixelSize: 20
            text: qsTr("Drag & Drop a GostCrypt volume here !")
            anchors.horizontalCenter: helpStart.horizontalCenter
            color: palette.textLowOpacity
            font.bold: true
        }

        Rectangle {
            id: rect
            width: 594
            anchors.bottom: helpStart.bottom
            height:100
            color: palette.darkThird

            Text {
                id:line1
                y:30
                font.pixelSize: 15
                color: palette.textLowOpacity
                anchors.horizontalCenter: rect.horizontalCenter
                horizontalAlignment: Text.AlignVCenter
                text: qsTr("Thank you for using <font color=\"#719c24\">GostCrypt</font> !")
            }

            Text {
                color: palette.textLowOpacity
                anchors.horizontalCenter: rect.horizontalCenter
                font.pixelSize: 13
                y: line1.y + 20
                horizontalAlignment: Text.AlignVCenter
                text: qsTr("<html>New user ? Try to <font color=\"#2f88a7\"><a href=\"1\">create</a></font>
                            or <font color=\"#719c24\"><a href=\"2\">mount</a></font> a volume</html>")
                MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.NoButton
                        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                    }
                onLinkActivated: {
                    volumeToolsMenu.opacity = 0.0
                    if(link == "1")
                    {
                        //TODO : right method
                        ConnectSignals.debug_connectReceiveCreate()
                    }else{
                        openSubWindow("dialogs/GSOpenVolume.qml", 'Open a GostCrypt volume', "Mount a volume", 429, {"name" : "", "value" : ""})
                    }
                }
            }

        }
    }

    //Volume Tools menu
    Rectangle {
        id: volumeToolsMenu
        color: palette.darkThird
        border.width: 2
        border.color: palette.blue
        width: volumeTools.width * 1.5
        height: 240
        x:455
        y:210
        opacity: 0.0
        visible: false
        ItemVolumeTools {
            id: element1
            x:0
            y:0
            text_: qsTr("Change Volume Password...")
            onClicked: {
                //TODO : signal change volume password
                volumeToolsMenu.opacity = 0.0
            }
        }
        ItemVolumeTools {
            id: element2
            x:0
            y:40
            text_: qsTr("Set header key derivation algorithm...")
            onClicked: {
                //TODO : signal header key derivation algo
                volumeToolsMenu.opacity = 0.0
            }
        }
        ItemVolumeTools {
            id: element3
            x:0
            y:80
            text_: qsTr("Add/Remove Keyfiles from/to Volume...")
            onClicked: {
                //TODO : signal Add/Remove Keyfiles from/to Volume
                volumeToolsMenu.opacity = 0.0
            }
        }
        ItemVolumeTools {
            id: element4
            x:0
            y:120
            text_: qsTr("Remove all keyfiles from volume...")
            onClicked: {
                //TODO : signal Remove all keyfiles from volume
                volumeToolsMenu.opacity = 0.0
            }
        }
        ItemVolumeTools {
            id: element5
            x:0
            y:160
            text_: qsTr("Backup volume header...")
            onClicked: {
                //TODO : signal Backup volume header
                volumeToolsMenu.opacity = 0.0
            }
        }
        ItemVolumeTools {
            id: element6
            x:0
            y:200
            text_: qsTr("Reset volume header...")
            onClicked: {
                //TODO : signal Reset volume header
                volumeToolsMenu.opacity = 0.0
            }
        }
        Behavior on opacity {
            NumberAnimation {
                id: anim_;
                duration: app.duration/2;
                easing.type: Easing.OutQuad;
                onRunningChanged: {
                    if(!anim_.running && volumeToolsMenu.opacity == 0.0) {
                        volumeToolsMenu.visible = false
                    }
                }
            }
        }
    }

    DropArea {
        x: 0; y: 0
        anchors.fill:mainWindow

        Rectangle {
            x: 98
            y: 111
            width: 594
            height: 303
            color: palette.textLowOpacity
            opacity: 0.5
            visible: parent.containsDrag
        }

        onDropped: {
            if (drop.hasText) {
                if (drop.proposedAction == Qt.MoveAction || drop.proposedAction == Qt.CopyAction) {
                    console.log(drop.text)
                    openSubWindow("dialogs/GSOpenVolume.qml", 'Open a GostCrypt volume', "Mount a volume", 429, {"name" : "dropVolume", "value" : drop.text.trim()})
                    drop.acceptProposedAction()
                }
            }
        }
    }

    function loadVolume(MountPoint, EncryptionAlgorithmName, Path, Size) {
        console.log("Ajout du volume");
        if(helpStart !== undefined && helpStart.visible == true)
        {
            helpStart.visible = false
        }
        listOfVolumes.append({MountPoint_: MountPoint, EncryptionAlgorithmName_: EncryptionAlgorithmName, Path_: Path, Size_: Size})
    }

    function clearVolumes() {
        listOfVolumes.clear();
    }

}
