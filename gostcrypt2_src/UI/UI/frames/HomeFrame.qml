import QtQml 2.0 as QML
import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import "../" as UI

Item {
    signal menuChanged(string name, int index)
    property QtObject mainWindow_
    /*!
        \class GSMainForm
        \brief Contains the background of the
        main window
    */

    Item {
        id: mainWindow
        x: 0
        width: 790
        height: 530

        Rectangle {
            id: topBar
            x: -100
            y: 0
            width: 890
            height: 74
            color: palette.darkSecond
        }

        Rectangle {
            id: rectangle1
            anchors {
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
            }
            y: 438
            width: 500
            height: 70
            color: palette.darkSecond
            border.width: 1
            border.color: palette.darkInput
        }

        Rectangle {
            id: rectangle2
            anchors.horizontalCenter: parent.horizontalCenter
            y: 111
            width: 550
            height: 315
            color: palette.darkSecond
        }
         Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
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
            text: qsTr("Mount Volume") + Translation.tr
            /*!
            \qmlsignal onClicked
            \brief Open the subwindow by clicking on the "Mount volume"
            button. Animation when subwindow appears.
         */
            onClicked: {
                openSubWindow("dialogs/OpenVolume.qml", qsTr('Open a GostCrypt volume'), qsTr("Mount a volume"), 429, {"name" : "", "value" : ""})
            }
        }
        /*!
        \class GSButtonBlue_icon
        \brief Button that opens the Gostcrypt volume creation
        wizard to create a volume.
     */
        UI.GSButtonBlue_icon {
            text: qsTr("Create Volume") + Translation.tr
            onClicked: {
                openSubWindow("wizard/WizardManager.qml", qsTr('GostCrypt Volume Creation Wizard'), qsTr("Create a volume"), 429, {"name" : "", "value" : "", "type": true})
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
        spacing: 30
        anchors.horizontalCenter: mainWindow.horizontalCenter
        anchors.bottom: mainWindow.bottom;
        anchors.bottomMargin: 15
        /*!
          \class GSButtonBordered
          \brief Auto-mount a device.
        */
        UI.GSButtonBordered {
            text: qsTr("Auto Mount Device") + Translation.tr
            width:200
            onClicked:
            {
                qmlRequest("automount", "dummy");
            }
            color_:palette.green
        }
        /*!
          \class GSButtonBordered
          \brief Dismount all the devices currently mounted
        */
        UI.GSButtonBordered {
            text: qsTr("Dismount All") + Translation.tr
            color_:palette.blue
            width:200
            onClicked: {
                qmlRequest("dismountall", "dummy");
            }
        }
        /*!
            \class GSButtonBordered
            \brief Opens the volume tools options.
         */
        /*UI.GSButtonBordered {
            id: volumeTools
            text: qsTr("Volume Tools") + Translation.tr
            color_:palette.blue
            //TODO : supprimer (tests)
            onClicked: {
                //LoadVolume.loadVolume("/media/volume", "GOST Grasshopper", "/home/user/myVolumes/volume", "5 MB");
                volumeToolsMenu.opacity = (volumeToolsMenu.opacity == 0.0) ? 1.0 : 0.0
                if(volumeToolsMenu.visible == false) volumeToolsMenu.visible = true
            }

        }*/
        Behavior on anchors.horizontalCenterOffset { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }



    /*!
        \qmlclass Item
        \brief Contains the gridview with all the mounted volumes.
     */
    Item {
        id: volumeListElement
        x: rectangle2.x
        y: rectangle2.y
        width: rectangle2.width
        height: rectangle2.height

        UI.GSVolumeItem {
            id:volumeDelegate
        }

        ListModel {
            id: listOfVolumes
        }

        GridView {
            id: grid
            y: 20
            x: 25
            height: volumeListElement.height - 40
            width: volumeListElement.width - 40
            //anchors.horizontalCenter: volumeListElement.horizontalCenter
            cellWidth: grid.width/2;
            cellHeight: 100
            delegate: volumeDelegate
            focus: true
            model: listOfVolumes
            ScrollBar.vertical: ScrollBar { snapMode: ScrollBar.SnapOnRelease }
            snapMode: GridView.SnapToRow
            clip: true
        }
        Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }

        //Volume Tools menu
        Rectangle {
            id: volumeToolsMenuLayer
            color: "black"
            x: 0
            y: 0
            width: rectangle2.width
            height: rectangle2.height
            opacity: 0.0
            visible: false
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    volumeToolsMenu.opacity = 0.0
                    volumeToolsMenuLayer.opacity = 0.0
                }
            }
            Behavior on opacity {
                NumberAnimation {
                    id: animLayer_;
                    duration: app.duration/2;
                    easing.type: Easing.OutQuad;
                    onRunningChanged: {
                        if(!animLayer_.running && volumeToolsMenuLayer.opacity == 0.0) {
                            volumeToolsMenuLayer.visible = false
                        }
                    }
                }
            }
        }

        Rectangle {
            id: volumeToolsMenu
            color: palette.darkThird
            border.width: 2
            border.color: palette.blue
            width: 250
            height: 240
            anchors.centerIn: parent
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
                    volumeToolsMenuLayer.opacity = 0.0
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
                    volumeToolsMenuLayer.opacity = 0.0
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
                    volumeToolsMenuLayer.opacity = 0.0
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
                    volumeToolsMenuLayer.opacity = 0.0
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
                    volumeToolsMenuLayer.opacity = 0.0
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
                    volumeToolsMenuLayer.opacity = 0.0
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
    }

    //Default content on start
    Item {
        id: helpStart
        x: 98
        y: 111
        width: 594
        height: 315
        visible: true
        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true
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
            text: qsTr("Drag & Drop a GostCrypt volume here !") + Translation.tr
            anchors.horizontalCenter: helpStart.horizontalCenter
            color: palette.textLowOpacity
            font.bold: true
        }

        Rectangle {
            id: rect
            width: rectangle2.width
            anchors.horizontalCenter: parent.horizontalCenter
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
                text: qsTr("Thank you for using <font color=\"#719c24\">GostCrypt</font> !") + Translation.tr
            }

            Text {
                color: palette.textLowOpacity
                anchors.horizontalCenter: rect.horizontalCenter
                font.pixelSize: 13
                y: line1.y + 20
                horizontalAlignment: Text.AlignVCenter
                text: qsTr("New user ? Try to <font color=\"#2f88a7\"><a href=\"1\">create</a></font>
                            or <font color=\"#719c24\"><a href=\"2\">mount</a></font> a volume") + Translation.tr
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
                onLinkActivated: {
                    if(link == "1")
                    {
                        openSubWindow("wizard/WizardManager.qml", qsTr('GostCrypt Volume Creation Wizard'), qsTr("Create a volume"), 429, {"name" : "", "value" : ""})
                    }else{
                        openSubWindow("dialogs/OpenVolume.qml", qsTr('Open a GostCrypt volume'), qsTr("Mount a volume"), 429, {"name" : "", "value" : ""})
                    }
                }
            }

        }
    }

    DropArea {
        x: 0; y: 0
        anchors.fill:mainWindow

        Rectangle {
            x: rectangle2.x
            y: rectangle2.y
            width: rectangle2.width
            height: rectangle2.height
            color: palette.textLowOpacity
            opacity: 0.5
            visible: parent.containsDrag
        }

        onDropped: {
            if (drop.hasText) {
                if (drop.proposedAction == Qt.MoveAction || drop.proposedAction == Qt.CopyAction) {
                    openSubWindow("dialogs/OpenVolume.qml", qsTr('Open a GostCrypt volume'), qsTr("Mount a volume"), 429, {"name" : "dropVolume", "value" : drop.text.trim()})
                    drop.acceptProposedAction()
                }
            }
        }
    }

    function loadVolume(MountPoint, EncryptionAlgorithmName, Path, Size) {
        if(helpStart !== undefined && helpStart.visible == true)
            helpStart.visible = false
        var isFavorite = UserSettings.isFavorite(Path);
        listOfVolumes.append({MountPoint_: MountPoint, EncryptionAlgorithmName_: EncryptionAlgorithmName, Path_: Path, Size_: Size, Favorite: isFavorite})
    }

    function clearVolumes() {
        listOfVolumes.clear();
        helpStart.visible = true;
    }

    function dismountVolume(path) {
        for(var i =0; i<listOfVolumes.count; i++)
            if(listOfVolumes.get(i).Path_ == path)
                listOfVolumes.remove(i);
        if(listOfVolumes.count == 0) helpStart.visible = true;
    }

}
