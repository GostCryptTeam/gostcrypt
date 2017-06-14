/*! \file main.qml
    \brief The GostCrypt's main window.
    \authors The GostCrypt development team.

    The interface of GostCrypt is developed in QML to be compatible for phones.
*/
import QtQml 2.0 as QML
import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import "../UI/LoadVolume.js" as LoadVolume

/*
 * Class : GostCrypt Main Window
 * Name : Window
 * Role : Main window with volumes, menu and subwindow management
 */
Window {
    //Properties
    id: app

    //QML slots that receive C++ signals
    Connections {
        target: ConnectSignals;
        onSendSubWindowVolumeInfos: {
            subWindow.catchClose();
            LoadVolume.loadVolume(aMount, aAlgo, aPath, aSize);
        }
        onSendSubWindowAskSudoPassword: {
            subWindow.opacity = subWindow.opacity = 1.0
            subWindow.visible = subWindow.visible = true
        }
        onSendSubWindowConfirmSudoPassword:  {
            subWindow.catchClose();
        }
    }
    //GostCrypt program name
    title: qsTr("GostCrypt 2.0")
    visible: true

    Component.onCompleted: ConnectSignals.getAllMountedVolumes()

    //Window's maximum dimension
    minimumWidth: 790
    minimumHeight: 530
    maximumHeight: minimumHeight
    maximumWidth: minimumWidth
    width: 790
    height: 530
    color: "#303030"

    //Class variables
    property bool isPC: true
    property bool isLandscape: false
    property int duration: 500
    property int menuWidth: 150
    property int widthOfSeizure: 15
    property bool shown: false
    property real rotate

    //colors used in the UI
    QtObject {
        id: palette
        property color dark: "#303030"
        property color darkSecond: "#2a2a2a"
        property color darkThirdb: "#272727"
        property color text: "#e1e1e1"
        property color textLight: "#e1e1e1"
        property color green: "#FFEB3B"
        property color blue: "#212121"
    }

    //Auto-detect screen type
    onWidthChanged: {
        if( width > height ) {
            app.isLandscape = true
        } else {
            app.isLandscape = false
        }
    }

    //Main window's background
    GSMainForm {
        id: mainWindow
        Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }

    //Centered buttons : "Mount volume" and "Create volume"
    Row {
        id: rowWindow
        spacing: 70
        anchors.horizontalCenter: parent.horizontalCenter
        topPadding: 18
        //Button that allow the user to mount a volume by opening a subwindow.
        GSButtonGreen_icon {
            text: qsTr("Mount Volume")
            onClicked: {
                subWindow.opacity = (subWindow.opacity == 1.0) ? 0.0 : 1.0
                subWindow.visible = (subWindow.visible == true) ? false : true
                if(subWindow.visible == true) {
                    subWindow.w = "../dialogs/GSOpenVolume.qml"
                    subWindow.title = 'Open a GostCrypt volume'
                    subWindow.loadForm()
                    subWindow.changeSubWindowHeight(429);
                }
            }
        }
        //Button that opens the Gostcrypt volume creation wizard to create a volume.
        GSButtonBlue_icon {
            text: qsTr("Create Volume")
            onClicked: {
                // testt.setText(text)
                ConnectSignals.debug_connectReceiveCreate()
            }
        }
        //Smooth fade-in/fade-out animation
        Behavior on anchors.horizontalCenterOffset { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }
    //Centered buttons : "Auto Mount Device", "Dismount All" and "Volume Tools"
    Row {
        id: rowBottom
        spacing: 22
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 28
        GSButtonGreen {
            text: qsTr("Auto Mount Device")
            onClicked: ConnectSignals.connectReceiveAutoMount("dummy")
        }
        GSButtonGreen {
            text: qsTr("Dismount All")
            onClicked: ConnectSignals.connectReceiveDismountAll()
        }
        GSButtonGreen {
            text: qsTr("Volume Tools")
            //TODO : supprimer (tests)
            onClicked: LoadVolume.loadVolume("/media/volume", "GOST Grasshopper", "/home/user/myVolumes/volume", "5 MB");
        }
        Behavior on anchors.horizontalCenterOffset { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }

    //Menu zone on the left
    GSMenu{
        id: gs_Menu
        height: app.height
        Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }
    /*! \fn toggleMenu(void)
     *  \brief Closing/Opening the menu by opening on the button
     */
    function toggleMenu() {
        gs_Menu.x = app.shown ? -gs_Menu.width : 0
        rowWindow.anchors.horizontalCenterOffset = app.shown ? 0 : 75
        rowBottom.anchors.horizontalCenterOffset = app.shown ? 0 : 75
        mainWindow.x = app.shown ? 0 : 75
        menuButton.x = app.shown ? 20 : 165
        volumeListElement.x = app.shown ? 98 : 173
        app.shown = !app.shown
        menuButton.value = !menuButton.value;
    }
    //Button that open or close the left-sided menu
    GSMenuButton {
        id: menuButton
        x: 20
        y: 15
        value: rotate
        Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
        onXChanged: {
            rotate = (1-Math.abs(gs_Menu.x/gs_Menu.width))
            menuButton.value = rotate
        }
    }

    //View of all the loaded volumes
    Item {
        id: volumeListElement
        x: 98
        y: 111
        width: 594
        height: 303

        GSVolumeItem {
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
        }
        Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }

    //Sub window inside of the main window
    GSSubWindow {
        id: subWindow
        width:app.width
        height:429
        visible: false
        opacity: 0.0
        heightSubWindow: 429
    }
}
