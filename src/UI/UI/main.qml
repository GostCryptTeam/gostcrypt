/*! \file main.qml
    \brief The GostCrypt's main window.
    \authors The GostCrypt development team.

    The interface of GostCrypt is developed in QML to be compatible for phones.
*/
import QtQml 2.0 as QML
import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import "./frames" as GSFrame
import "./wizard" as W

/*!
  \class main.qml (Window)
  \brief Main window with volumes, menu and
  subwindow management
*/
Window {
    /*************************************
     ************* Properties ************
     *************************************/
    /*!
        \property Window::id
        \brief Setting the Class id
     */
    id: app

    /*!
        \property title
        \brief GostCrypt program name
     */
    title: qsTr("GostCrypt 2.0") + Translation.tr
    /*!
        \property visible
        \brief Displaying main window
     */
    visible: true

    /*!
      \flags Setting the window frameless
      */
    flags: Qt.FramelessWindowHint | Qt.Window

    /*!
        \class QtObject
        \brief All the colors used in the program
     */
    AbstractTheme {
        id: palette
        type: 0
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
    minimumHeight: 570
    /*!
        \property maximumHeight
        \brief Window's maximum height
     */
    maximumHeight: 570
    /*!
        \property maximumWidth
        \brief Window's maximum width
     */
    maximumWidth: 790
    /*!
        \property width
        \brief The window's default width
     */
    width: 790
    /*!
        \property height
        \brief The window's default height
     */
    height: 570
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
    property int duration: 250
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
      \property model
      \brief The list of mounted volumes
    */
    property variant model

    /*!
      \property notifications
      \brief Table of the notifications with id, etc
    */
    property variant notifications: []

    /*!
      \property hashs
      \brief Table of hashes usable in the current version of gostcrypt
    */
    property variant hashs: []

    /*!
      \property algorithms
      \brief Table of algorithms usable in the current version of gostcrypt
    */
    property variant algorithms: []

    /*!
      \property filesystems
      \brief Table of filesystems usable in the current version of gostcrypt
    */
    property variant filesystems: []

    /*!
      \property exit requested
      \brief used to know when to close gostcrypt after dismounting all volumes
    */
    property bool exitRequested: false

    signal sendQmlRequest(string command, variant params)
    signal sendSudoPassword(string password)
    signal appQuit()

    /*************************************
     **********  Window content **********
     *************************************/

    signal menuChanged(string name, int index)

    Loader {
        id: pageLoader
        source: "frames/HomeFrame.qml"
        onSourceChanged: animation.running = true
        anchors.fill: parent
        anchors.topMargin: 40
        onLoaded: {
            pageLoader.item.mainWindow_ = app
        }
        NumberAnimation {
            id: animation
            target: pageLoader.item
            property: "opacity"
            from: 0
            to: 1.0
            duration: app.duration/4
            easing.type: Easing.InExpo
        }
        Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }

    Connections {
        target: gs_Menu
        onMenuChanged: {
            if(name !== "")
                pageLoader.source = name
            gs_Menu.selected = index
        }
    }

    Connections {
        target: pageLoader.item
        onMenuChanged: {
            if(pageLoader.source !== name)
                pageLoader.source = name
            if(name === "frames/Home.qml")
            {
                pageLoader.item.clearVolumes()
                ConnectSignals.getAllMountedVolumes();
            }
        }
    }



    /*************************************
     *************  Signals  *************
     *************************************/
    /*!
        \brief Receive all the mounted volumes after
        the window is successfully loaded
     */


    /*!
        \class SignalManager
        \brief Manages the received signals
     */
    SignalManager {
        id: signalManager
    }


    /*************************************
     ***** Frameless window dragging *****
     *************************************/
    MouseArea {
        id: titleBarMouseRegion
        x:0
        y:0
        height: 40
        width: parent.width
        property var clickPos
        onPressed: {
            clickPos = { x: mouse.x, y: mouse.y }
        }
        onPositionChanged: {
            app.x = DragWindowProvider.cursorPos().x - clickPos.x
            app.y = DragWindowProvider.cursorPos().y - clickPos.y
        }
    }


    /*************************************
     *********  Notif Preview  ***********
     *************************************/

    Rectangle { //body
        id: notifPreview
        y: 48
        x: 620
        height: 80
        width: 150
        color: palette.border
        opacity: 0.0

        property string n: ""
        property string p: "0"

        Text {
            text: "<b>"+ notifPreview.n
            wrapMode: Text.WordWrap
            width: parent.width - 50
            leftPadding: 10
            rightPadding: 20
            x: 60
            anchors.verticalCenter: parent.verticalCenter
            color: palette.text
        }

        CircleLoadingBar {
            id: circle
            anchors.verticalCenter: parent.verticalCenter
            x: 10
            size: 50
            percent: Number(notifPreview.p)
        }

        SequentialAnimation {
            id: timerNotifPreview
            running: false
            NumberAnimation { target: notifPreview; property: "opacity"; to: 1.0; duration: 1000 }
            NumberAnimation { target: notifPreview; property: "opacity"; to: 1.0; duration: 8000 }
            NumberAnimation { target: notifPreview; property: "opacity"; to: 0.0; duration: 1000 }
        }

    }



    /*************************************
     *********  Static window  ***********
     *************************************/


    Rectangle {
        id: fullSizeSubMenu
        x:0
        y:40
        width: app.width
        height: app.height-40
        color: "#000000"
        opacity: 0.0
        Behavior on opacity {
            NumberAnimation {
                duration: app.duration/2;
                easing.type: Easing.OutQuad
            }
        }
        MouseArea {
            id: mouseAreaMenu
            anchors.fill: parent
            enabled: false
            visible: false
            propagateComposedEvents: false
            hoverEnabled: true
            onClicked: {
                pageLoader.item.enableAnimation();
                toggleMenu(-1)
            }
        }
    }

    /*!
        \class MenuButton
        \brief The menu button with three horizontal bars
        that changes to an arrow when the menu is opened.
     */
    MenuButton {
        id: menuButton
        x: 20
        y: 55
        value: rotate
        Behavior on x {
            NumberAnimation {
                duration: app.duration;
                easing.type: Easing.OutQuad
                onRunningChanged: {
                    if (!running) {
                        if(menuButton.x === 20)
                            menuButton.value = 0.0
                        else menuButton.value = 1.0
                    }
                }
            }
        }
        //Changing the rotation value following the width
        onXChanged: {
            if(x<=185) {
                rotate = (1-Math.abs(gs_Menu.x/gs_Menu.width))
                menuButton.value = rotate
            }
        }
    }


    /*!
        \class Menu
        \brief Menu zone on the left
     */
    MainMenu {
        id: gs_Menu
        height: app.height-40
        y:40
        selected: 0
        Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }


    TitleBar {
        id: title
        x:0
        y:0
        height:40
        height_: 40
    }


    /*************************************
     **********  On-top layers ***********
     *************************************/
    /*!
        \class Item
        \brief Sub window inside of the main window
     */
    Item {
        id: containSub
        y: 40
        anchors.fill: parent
        SubWindow {
            id: subWindow
            width: 790; height:429
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            y: 40
            visible: false
            opacity: 0.0
            isOpen: false
        }
    }

    Sudo {
        id: sudo_
        isVisible: false
        width: 350
        visible:false
        opacity : 0.0
        title: "Sudo password"
        contentText: "Enter your admin password"
    }

    ErrorMessage {
        id: errorMessage
        isVisible: false
        visible: false
        focus: true
        opacity: 0.0
        title: qsTr("Message d'erreur")
        contentError: qsTr("Description du message d'erreur.\n Le message spécifique s'affichera donc ici.")
    }

    /*************************************
     *************  Notifs ***************
     *************************************/

    MouseArea {
        id: bk_notifs
        x:0
        y:40
        width: app.width
        height: app.height - 40
        enabled: false
        onClicked: {
            title.changeNotif();
        }
    }

    NotificationBox {
        id: notifs
        x: app.width - 290
        y: 35
        opacity: 0
        visible: false
    }


    /*************************************
     **************  Style ***************
     *************************************/
    Rectangle{
        anchors.fill:parent
        color: "transparent"
        border.color: palette.darkThird
        border.width: 1
    }


    /*************************************
     *******  JavaScript functions *******
     *************************************/

    /*!
        \fn toggleMenu(void)
        \brief Closing/Opening the menu by opening on the button
        Returns nothing.
     */
    function toggleMenu(idMenu) {
        pageLoader.item.enableAnimation();
        if(idMenu === undefined)
        {
            gs_Menu.selected = 0
            gs_Menu.toggleSubMenu(false)
            gs_Menu.x = app.shown ? -gs_Menu.width-1 : 0
            if(app.shown) pageLoader.item.slide(false);
            else pageLoader.item.slide(true);
            menuButton.x = app.shown ? 20 : 185
            app.shown = !app.shown
            menuButton.value = !menuButton.value;
        }
        //Select menu if there is an argument
        else if(idMenu !== undefined && idMenu !== -1)
        {
            gs_Menu.selected = idMenu
            gs_Menu.toggleSubMenu(true)
            menuButton.x = 185
            menuButton.value = 1.0
            gs_Menu.x = -gs_Menu.width+50
            app.shown = true
            gs_Menu.manageSubMenu(idMenu)
        }
        else if(idMenu === -1)
        {
            gs_Menu.selected = 0
            gs_Menu.toggleSubMenu(false)
            gs_Menu.x = -gs_Menu.width-1
            pageLoader.item.slide(false);
            menuButton.x = 20
            app.shown = false
            menuButton.value = 0.0
        }
    }

    function openSubWindow(path, title, name, height, parameter) {
        if(subWindow.isOpen == false)
        {
            subWindow.opacity = 1.0
            subWindow.visible = true
            subWindow.isOpen = true
            subWindow.w = path
            subWindow.name = name
            subWindow.height = height
            subWindow.parameter = parameter
            subWindow.loadForm(height, title)
        }
        else
        {
            subWindow.w = path
            subWindow.name = name
            subWindow.parameter = parameter
            subWindow.loadForm(height, title)
        }
    }

    function openErrorMessage(title, content, size, type, callback, callback_params) {
        if(size !== undefined) errorMessage.size = size
        if(type !== undefined) errorMessage.type = type
        if(callback !== undefined) errorMessage.callback = callback
        if(callback_params !== undefined) errorMessage.callback_params = callback_params
        if(errorMessage.isVisible == false){
            errorMessage.isVisible = true;
            errorMessage.opacity = 1.0;
            errorMessage.visible = true;
            errorMessage.title = title;
            errorMessage.contentError = content;
        }else{
            errorMessage.visible = true;
            errorMessage.title = title;
            errorMessage.contentError = content;
        }
    }

    function closeErrorMessage() {
        if(errorMessage.isVisible == true){
            errorMessage.isVisible = false;
            errorMessage.opacity = 0.0;
            errorMessage.button1Name = ""
            errorMessage.button2Name = ""
            errorMessage.callback = ""
            errorMessage.callback_params = {}
        }
    }

    function toggleSudo(choice) {
        if(choice === true || choice === 1)
        {
            sudo_.opacity = 1.0
            sudo_.isVisible = true
            sudo_.visible = true
            sudo_.setFocus();
        }else{
            sudo_.opacity = 0.0
            sudo_.isVisible = false
        }
    }

    function qmlRequest(type, content)
    {
        //If we want to add a notification
        if(content.name !== undefined && content.desc !== undefined && content.name !== "")
            content.id = addNotification("progress", content.name, content.desc);

        sendQmlRequest(type, content);
    }

    function addNotification(type, title, description)
    {
        if(type === "progress")
            notifications.push([title, description, Number(notifications.length)+1, 0, 0]);
        else
            notifications.push([title, description, Number(notifications.length)+1, -1, 0]);

        notifs.drawNotification();

        return Number(notifications.length);
    }

    function refreshFavorite()
    {
        gs_Menu.updateFavorites();
    }
}
