import QtQml 2.0 as QML
import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import "../" as UI

Item {
    signal menuChanged(string name, int index)
    property QtObject mainWindow_
    id: top

    FileDialog {
        id: fileDialog
        title: qsTr("Please choose a file") + Translation.tr
        folder: shortcuts.home
        onAccepted: {
            openSubWindow("dialogs/OpenVolume.qml", qsTr('Open a GostCrypt volume'), qsTr("Mount a volume"), 429, {"name" : "dropVolume", "value" : fileDialog.fileUrl})
        }
        onRejected: {
        }
    }

    /*!
        \class GSMainForm
        \brief Contains the background of the
        main window
    */
    Item {
        id: mainWindow
        x: 0
        anchors.fill: parent

        Rectangle {
            id: topBar
            x: -100
            y: 0
            width: parent.width + 100
            height: 74
            color: palette.darkSecond
        }

        Rectangle {
            id: rectangle1
            anchors {
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
            }
            width: parent.width
            height: 70
            color: palette.darkSecond
            border.width: 1
            border.color: palette.darkInput
        }
        Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }

    Item {
        id: containerHome
        width: 790
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        //Volume container
        Rectangle {
            id: rectangle2
            x: (isEmpty === true) ? 380 : 120
            y: 111
            width: (isEmpty === true) ? 350 : 550
            height: 350
            color: palette.darkSecond
            property bool isEmpty: true
            //Borders
            Rectangle {
                height: 1
                y: 0
                x: 0
                width: parent.width
                color: palette.darkInput
            }
            Rectangle {
                height: parent.height
                y: 0
                x: 0
                width: 1
                color: palette.darkInput
            }
            Rectangle {
                height: parent.height
                y: 0
                x: parent.width-1
                width: 1
                color: palette.darkInput
            }
            Behavior on x { id: behaviorVolumeContainer; enabled: false; NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
        }

        /*!
            \qmlclass Item
            \brief Contains the gridview with all the mounted volumes.
         */
        Item {
            id: volumeListElement
            anchors.fill: rectangle2

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
                height: rectangle2.height-80
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
                property string path: ""

                ItemVolumeTools {
                    id: element1
                    x:0
                    y:0
                    text_: qsTr("Change Volume Password...")
                    onClicked: {
                        //TODO : signal change volume password
                        openSubWindow("dialogs/ChangePassword.qml", qsTr("Change Volume Password"), qsTr("Change Volume Password"), 429, {"name" : "volume-settings", "value" : volumeToolsMenu.path})
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
                        openErrorMessage(qsTr("Information"), qsTr("For security reasons, you will have to enter the correct password "+
                                                                   "(and/or supply the correct keyfiles) for the volume.<br><br>Note: "+
                                                                   "If the volume contains a hidden volume, you will have to enter the "+
                                                                   "correct password (and/or supply the correct keyfiles) for the outer "+
                                                                   "volume first. Afterwards, if you choose to back up the header of "+
                                                                   "the hidden volume, you will have to enter the correct password "+
                                                                   "(and/or supply the correct keyfiles) for the hidden volume."))

                        openSubWindow("dialogs/BackupHeaderVolume.qml", qsTr("Backup Volume Header"), qsTr("Backup Volume Header"), 429, {"name" : "volume-backup", "value" : volumeToolsMenu.path})
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
                        openSubWindow("dialogs/RestoreHeaderVolume.qml", qsTr("Restore Volume Header"), qsTr("Restore Volume Header"), 429, {"name" : "volume-backup", "value" : volumeToolsMenu.path})
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

        Item {
            id: helpStart
            anchors.fill: parent
            //VOLUME PART
            //Content
            Column {
                id: rightPart
                spacing: 20
                x: rectangle2.x
                y: rectangle2.y + 10
                width: rectangle2.width
                height: rectangle2.height
                Text {
                    id: text
                    color: palette.text
                    width: parent.width
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("Mount or create your GostCrypt volume now!") + Translation.tr
                    font.pointSize: 14
                    rightPadding: 10
                    leftPadding: 10
                }

                //Image drag&Drop
                Image {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 276
                    height: 215
                    fillMode: Image.Pad
                    anchors.topMargin: 55
                    horizontalAlignment: Image.AlignHCenter
                    source: "../ressource/dragDrop.png"
                    Image {
                        id: arrow
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 92
                        height: 79
                        y: 75
                        fillMode: Image.Pad
                        anchors.topMargin: 55
                        horizontalAlignment: Image.AlignHCenter
                        source: "../ressource/arrow.png"

                        SequentialAnimation {
                            running: true;
                            loops: Animation.Infinite;
                            NumberAnimation { target: arrow; property: "y"; to: 85; duration: 5000; easing.type: Easing.InOutCubic; }
                            NumberAnimation { target: arrow; property: "y"; to: 75; duration: 5000; easing.type: Easing.InOutCubic; }
                        }
                    }
                }

                Text {
                    id: text2
                    color: palette.text
                    width: parent.width
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("Drag&Drop your GostCrypt volume here!") + Translation.tr
                    font.pointSize: 10
                }
                Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
            }

            //Left part
            Column {
                id: leftPart
                width: 250
                height: rightPart.height
                x: 80
                y: rightPart.y - 20
                spacing: 10
                Text {
                    id: title
                    color: palette.text
                    width: parent.width
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHLeft
                    text: qsTr("Welcome to GostCrypt.") + Translation.tr
                    font.pointSize: 28
                    lineHeight: 0.8
                    font.weight: Font.Medium
                    font.letterSpacing: -2
                }
                Text {
                    id: version
                    x: 5;
                    color: palette.text
                    width: parent.width
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHLeft
                    text: qsTr("version " + UserSettings.getVersion()) + Translation.tr
                    font.pointSize: 9
                }
                Text {
                    id: descriptionHome
                    x: 5;
                    color: palette.text
                    width: parent.width
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHLeft
                    text: qsTr("GostCrypt allows you to create encrypted data "+
                               "volume within a file or an external device. It "+
                               "use GOST Grasshopper & SM-4 Encryption algorithms.<br><br>"+
                               "You can visit our website at <font color=\"#719c24\"><a href=\"http://gostcrypt.org\">GostCrypt</a></font>") + Translation.tr
                    font.pointSize: 9
                    onLinkActivated: Qt.openUrlExternally("http://www.gostcrypt.org/")
                }
                Rectangle {
                    id: separator
                    height: 40
                    width: 1
                    color: "transparent"
                }

                Row {
                    spacing: 10
                    UI.GSButtonBordered {
                        text: qsTr("Favorites Volumes") + Translation.tr
                        width:130
                        onClicked:
                        {
                            toggleMenu(1)
                        }
                        color_: palette.blue
                    }
                    UI.GSButtonBordered {
                        text: qsTr("User Guide") + Translation.tr
                        color_: palette.green
                        width:100
                        onClicked: {
                            qmlRequest("openguide", "");
                        }
                    }
                }

                Behavior on opacity { NumberAnimation { duration: app.duration/2; easing.type: Easing.OutQuad } }
            }
        }
    }

    Rectangle {
        id: horizontalCenters
        x: 0
        width: parent.width
        height: 1
        color: "transparent"
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
        anchors.horizontalCenter: horizontalCenters.horizontalCenter
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
        anchors.horizontalCenter: horizontalCenters.horizontalCenter
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
        Behavior on anchors.horizontalCenterOffset { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }



    DropArea {
        x: rectangle2.x+1;
        y: rectangle2.y+1;
        width: rectangle2.width-2;
        height: rectangle2.height

        Rectangle {
            anchors.fill: parent
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

        MouseArea {
            anchors.fill: parent
            enabled: (helpStart.visible) ? true : false
            cursorShape: containsMouse ? Qt.OpenHandCursor : Qt.ArrowCursor
            onClicked: {
                fileDialog.open();
            }
        }
    }

    function enableAnimation() {
        behaviorVolumeContainer.enabled = true;
    }

    function loadVolume(MountPoint, EncryptionAlgorithmName, Path, Size) {
        if(helpStart !== undefined && helpStart.visible === true) {
            helpStart.visible = false
            rectangle2.isEmpty = false
            if(app.shown === true) rectangle2.x = 200
            else rectangle2.x = 120
            behaviorVolumeContainer.enabled = true;
        }

        var isFavorite = UserSettings.isFavorite(Path);
        listOfVolumes.append({mountPoint_: MountPoint, EncryptionAlgorithmName_: EncryptionAlgorithmName, Path_: Path, Size_: Size, Favorite: isFavorite})
    }

    function clearVolumes() {
        listOfVolumes.clear();
        helpStart.visible = true;
        rectangle2.isEmpty = true;
        if(app.shown) slide(1);
        else slide(0);
        //toggleMenu(-1);
    }

    function dismountVolume(path) {
        for(var i =0; i<listOfVolumes.count; i++)
            if(listOfVolumes.get(i).Path_ === path)
                listOfVolumes.remove(i);
        if(listOfVolumes.count == 0) {
            helpStart.visible = true;
        }
    }

    function slide(bool)
    {
        if(bool)
        {
            mainWindow.x = 20
            if(rectangle2.isEmpty)
                rectangle2.x = 300
            else
                rectangle2.x = 200
            //leftPart.x = 80 + 100
            leftPart.opacity = 0
            horizontalCenters.x = 80
            rightPart.x = 305
        }else
        {
            mainWindow.x = 0
            if(rectangle2.isEmpty)
                rectangle2.x = 380
            else
                rectangle2.x = 120
            //leftPart.x = 80
            leftPart.opacity = 1
            horizontalCenters.x = 0
            rightPart.x = 380
        }
    }
}
