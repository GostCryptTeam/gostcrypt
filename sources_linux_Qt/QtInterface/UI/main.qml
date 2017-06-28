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

/*!
  \class main.qml (Window)
  \brief Main window with volumes, menu and
  subwindow management
*/
Window {
    /*!
        \property Window::id
        \brief Setting the Class id
     */
    id: app

    /*!
        \property title
        \brief GostCrypt program name
     */
    title: qsTr("GostCrypt 2.0")
    /*!
        \property visible
        \brief Displaying main window
     */
    visible: true

    SignalManager {
        id: signalManager
    }

    /*!
        \brief Receive all the mounted volumes after
        the window is successfully loaded
     */
    Component.onCompleted: ConnectSignals.getAllMountedVolumes()

    /*!
        \class QtObject
        \brief All the colors used in the program
     */
    QtObject {
        id: palette
        property color shadow: "#000000"
        property color dark: "#303030"
        property color darkSecond: "#2a2a2a"
        property color darkThirdb: "#272727"
        property color text: "#e1e1e1"
        property color textLight: "#e1e1e1"
        property color green: "#FFEB3B"
        property color blue: "#2f88a7"
    }

    //Window's maximum dimension
    /*!
        \property minimumWidth
        \brief Window's minimum width
     */
    minimumWidth: 790
    /*!
        \property minimumHeight
        \brief Window's minimum height
     */
    minimumHeight: 530
    /*!
        \property maximumHeight
        \brief Window's maximum height
     */
    maximumHeight: minimumHeight
    /*!
        \property maximumWidth
        \brief Window's maximum width
     */
    maximumWidth: minimumWidth
    /*!
        \property width
        \brief The window's default width
     */
    width: 790
    /*!
        \property height
        \brief The window's default height
     */
    height: 530
    /*!
        \property color
        \brief The window's default
        background color
     */
    color: palette.dark

    /*!
        \property isPC
        \brief Whether the device is PC or Phone
     */
    property bool isPC: true
    /*!
        \property isLandscape
        \brief Whether the device is in landscape mode or not
     */
    property bool isLandscape: false
    /*!
        \property duration
        \brief Duration of animations
     */
    property int duration: 500
    /*!
        \property menuWidth
        \brief The width of the left-sided menu
     */
    property int menuWidth: 150
    /*!
        \property shown
        \brief Whether the menu is shown or not
     */
    property bool shown: false
    /*!
        \property rotate
        \brief The current rotation of the bars
        of menuButton
     */
    property real rotate

    /*!
        \qmlsignal onWidthChanged
        \brief When the width of the 'app'
        object changes, updating the screen type
     */
    onWidthChanged: {
        if( width > height ) {
            app.isLandscape = true
        } else {
            app.isLandscape = false
        }
    }

    /*!
        \class GSMainForm
        \brief Contains the background of the
        main window
     */
    GSMainForm {
        id: mainWindow
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
        anchors.horizontalCenter: parent.horizontalCenter
        topPadding: 18
        /*!
            \class GSButtonGreen_icon
            \brief Button that allow the user to mount
            a volume by opening a subwindow.
         */
        GSButtonGreen_icon {
            text: qsTr("Mount Volume")
            /*!
                \qmlsignal onClicked
                \brief Open the subwindow by clicking on the "Mount volume"
                button. Animation when subwindow appears.
             */
            onClicked: {
                subWindow.opacity = (subWindow.opacity == 1.0) ? 0.0 : 1.0
                subWindow.visible = (subWindow.visible == true) ? false : true
                subWindow.isOpen = true
                if(subWindow.isOpen == true) {
                    subWindow.w = "../dialogs/GSOpenVolume.qml"
                    subWindow.title = 'Open a GostCrypt volume'
                    subWindow.loadForm()
                    subWindow.changeSubWindowHeight(429);
                }
            }
        }
        /*!
            \class GSButtonBlue_icon
            \brief Button that opens the Gostcrypt volume creation
            wizard to create a volume.
         */
        GSButtonBlue_icon {
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
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 28
        /*!
            \class GSButtonGreen
            \brief Auto-mount a device.
         */
        GSButtonGreen {
            text: qsTr("Auto Mount Device")
            onClicked: ConnectSignals.connectReceiveAutoMount("dummy")
        }
        /*!
            \class GSButtonGreen
            \brief Dismount all the devices currently mounted
         */
        GSButtonGreen {
            text: qsTr("Dismount All")
            onClicked: {
                if(!isSudo) {
                    isSudo = true;
                    ConnectSignals.connectReceiveDismountAll()
                    listOfVolumes.clear()
                    isSudo = false;
                }
            }
        }
        /*!
            \class GSButtonGreen
            \brief Opens the volume tools options.
         */
        GSButtonGreen {
            text: qsTr("Volume Tools")
            //TODO : supprimer (tests)
            onClicked: LoadVolume.loadVolume("/media/volume", "GOST Grasshopper", "/home/user/myVolumes/volume", "5 MB");
        }
        Behavior on anchors.horizontalCenterOffset { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }
    /*!
        \class GSMenu
        \brief Menu zone on the left
     */
    GSMenu{
        id: gs_Menu
        height: app.height
        Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }
    /*!
        \fn toggleMenu(void)
        \brief Closing/Opening the menu by opening on the button
        Returns nothing.
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
    /*!
        \class GSMenuButton
        \brief The menu button with three horizontal bars
        that changes to an arrow when the menu is opened.
     */
    GSMenuButton {
        id: menuButton
        x: 20
        y: 15
        value: rotate
        Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
        //Changing the rotation value following the width
        onXChanged: {
            rotate = (1-Math.abs(gs_Menu.x/gs_Menu.width))
            menuButton.value = rotate
        }
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
            onCurrentIndexChanged: {
                console.log("Item select = " + currentIndex);
            }
        }
        Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }

    /*!
        \class Item
        \brief Sub window inside of the main window
     */
    GSSubWindow {
        id: subWindow
        width:app.width
        height:429
        visible: false
        opacity: 0.0
        heightSubWindow: 429
        isOpen: false
    }
}
