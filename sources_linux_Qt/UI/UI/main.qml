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
import "LoadVolume.js" as LoadVolume
import "./frames" as GSFrame

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
    QtObject {
        id: palette
        property color shadow: "#000000"
        property color dark: "#303030"
        property color darkSecond: "#2a2a2a"
        property color darkThird: "#272727"
        property color textLowOpacity: "#454545"
        property color text: "#e1e1e1"
        property color textLight: "#e1e1e1"
        property color green: "#719c24"
        property color greenHover: "#7ba430"
        property color greenDark: "#597d1c"
        property color blue: "#2f88a7"
        property color blueHover: "#3d97b6"
        property color bluePress: "#266f88"
        property color border: "#202020"
        property color darkInput: "#181818"
        property color hoverItemMenu: "#404040"
        property color bkCheckBox: "#191919"
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

    signal signalQML(variant content)



    /*************************************
     **********  Window content **********
     *************************************/

    signal menuChanged(string name, int index)

    Loader {
        id: pageLoader
        source: "frames/HomeFrame.qml"
        y:40
        x:0
        onSourceChanged: animation.running = true
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
      \signal mountVolume : sending a signal
        to Gostcrypt to mount a volume
     */
    signal qmlTest(variant a)
    signal mountVolume(string path, string password)

    /*!
        \brief Receive all the mounted volumes after
        the window is successfully loaded
     */
    Component.onCompleted: {
       // app.signalQML({"content": "getAllMountedVolumes"})
       // console.log("TODO: get all mounted volumes")//ConnectSignals.getAllMountedVolumes()
    }

    //TODO : add all the signals QML->C++ here
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
                toggleMenu()
            }
        }
    }

    /*!
        \class GSMenuButton
        \brief The menu button with three horizontal bars
        that changes to an arrow when the menu is opened.
     */
    GSMenuButton {
        id: menuButton
        x: 20
        y: 55
        value: rotate
        Behavior on x {
            NumberAnimation {
                duration: app.duration;
                easing.type: Easing.OutQuad
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
        \class GSMenu
        \brief Menu zone on the left
     */
    GSMenu{
        id: gs_Menu
        height: app.height-40
        y:40
        selected: 0
        Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }



    TitleBar  {
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
        x: 0
        y: 40
        GSSubWindow {
            id: subWindow
            width:app.width
            y:40
            height:429
            visible: false
            opacity: 0.0
            heightSubWindow: 429
            isOpen: false
        }
    }

    GSSudo{
        id: sudo_
        isVisible: false
        visible:false
        opacity : 0.0
        title: "Sudo password"
        contentText: "Enter your admin password"
    }

    GSErrorMessage {
        id: errorMessage
        isVisible: false
        visible: false
        opacity: 0.0
        title: qsTr("Message d'erreur")
        contentError: qsTr("Description du message d'erreur.\n Le message spécifique s'affichera donc ici.")
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
    function toggleMenu() {
        gs_Menu.selected = 0
        gs_Menu.toggleSubMenu(false)
        gs_Menu.x = app.shown ? -gs_Menu.width-1 : 0
        pageLoader.x = app.shown ? 0 : 85
        menuButton.x = app.shown ? 20 : 185
        app.shown = !app.shown
        menuButton.value = !menuButton.value;
        //gs_Menu.selected = 0 TODO
        //TODO setting home as active is nothing was selected
    }

    function openSubWindow(path, title, name, height, parameter) {
        if(subWindow.isOpen == false)
        {
            subWindow.opacity = 1.0
            subWindow.visible = true
            subWindow.isOpen = true
            subWindow.w = path
            subWindow.name = name
            subWindow.title = title
            subWindow.parameter = parameter
            subWindow.loadForm()
            subWindow.changeSubWindowHeight(height);
        }
        else
        {
            subWindow.w = path
            subWindow.title = title
            subWindow.name = name
            subWindow.parameter = parameter
            subWindow.loadForm()
            subWindow.changeSubWindowHeight(height);
            console.log("Taille changée en "+height+"px !")
        }
    }

    function openErrorMessage(title, content, size) {
        if(size !== undefined) errorMessage.size = size
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
        }
    }

    function toggleSudo(choice) {
        if(choice == true)
        {
            sudo_.opacity = 1.0
            sudo_.isVisible = true
            sudo_.visible = true
        }else{
            sudo_.opacity = 0.0
            sudo_.isVisible = false
        }
    }


}
