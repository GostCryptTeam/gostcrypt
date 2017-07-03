import QtQml 2.0 as QML
import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import "../LoadVolume.js" as LoadVolume
import "../" as UI

Item {
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
            color: "#2a2a2a"
        }

        Rectangle {
            id: rectangle1
            x: 98
            y: 438
            width: 594
            height: 92
            color: "#2a2a2a"
            border.width: 0
        }

        Rectangle {
            id: rectangle2
            x: 98
            y: 111
            width: 594
            height: 303
            color: "#2a2a2a"
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
            text: qsTr("Mount Volume")
            /*!
            \qmlsignal onClicked
            \brief Open the subwindow by clicking on the "Mount volume"
            button. Animation when subwindow appears.
         */
            onClicked: openSubWindow("dialogs/GSOpenVolume.qml", 'Open a GostCrypt volume', 429, {"name" : "", "value" : ""})
        }
        /*!
        \class GSButtonBlue_icon
        \brief Button that opens the Gostcrypt volume creation
        wizard to create a volume.
     */
        UI.GSButtonBlue_icon {
            text: qsTr("Create Volume")
            onClicked: {
                ConnectSignals.debug_connectReceiveCreate()
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
            onClicked: ConnectSignals.connectReceiveAutoMount("dummy")
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
            text: qsTr("Volume Tools")
            color_:palette.blue
            //TODO : supprimer (tests)
            onClicked: {
                LoadVolume.loadVolume("/media/volume", "GOST Grasshopper", "/home/user/myVolumes/volume", "5 MB");
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
            text: qsTr("Drag & Drop GostCrypt volume here !")
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
                        acceptedButtons: Qt.NoButton // we don't want to eat clicks on the Text
                        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                    }
                onLinkActivated: {
                    console.log(link)
                    if(link == "1")
                    {
                        //TODO : right method
                        ConnectSignals.debug_connectReceiveCreate()
                    }else{
                        openSubWindow("dialogs/GSOpenVolume.qml", 'Open a GostCrypt volume', 429, {"name" : "", "value" : ""})
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
                    openSubWindow("dialogs/GSOpenVolume.qml", 'Open a GostCrypt volume', 429, {"name" : "dropVolume", "value" : drop.text.trim()})
                    drop.acceptProposedAction()
                }
            }
        }
    }


}
