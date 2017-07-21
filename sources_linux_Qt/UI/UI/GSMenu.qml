import QtQuick 2.7
import QtQuick.Controls 2.2

Item {
    id: top
    x:-150
    width: 150
    property variant icons: [
        "ressource/menu_home.png",
        "ressource/menu_volume.png",
        "ressource/menu_system.png",
        "ressource/menu_tools.png",
        "ressource/menu_settings.png",
        "ressource/menu_favorite.png",
        "ressource/menu_help.png"]
    property variant menus: {
        "HOME": 0,
        "FAVORITE": 1,
        "TOOLS": 2,
        "SETTINGS": 3,
        "HELP": 4
    }

    property int selected: 0
    signal menuChanged(string name, int index)

    /********************
         SUBMENU PART
     ********************/
    Rectangle {
        id: subMenu
        x:0
        width: 120
        height: parent.height
        color: palette.border
        border.color: palette.darkInput
        border.width: 1
        Behavior on x {
            NumberAnimation {
                duration: app.duration/2;
                easing.type: Easing.OutQuad;
            }
        }

        Component {
                id: subMenuDelegate
                Item {
                    id: elementSubMenu
                    width: 100;
                    height: 50;
                    anchors.horizontalCenter: parent.horizontalCenter
                    Rectangle {
                        id: elementSubMenu_Rect
                        x: 5
                        width: 90
                        height: 50
                        radius: 2
                        color: palette.darkThird
                        Text {
                            horizontalAlignment: Text.AlignHCenter
                            text: message
                            color: palette.text
                            font.pixelSize: 11
                            wrapMode: Text.WordWrap
                            anchors.centerIn: parent
                            width: 80
                        }
                    }
                    MouseArea {
                        id: elementSubMenu_MouseArea
                        anchors.fill: elementSubMenu_Rect
                        hoverEnabled: true
                        onClicked: {
                            console.log("SubMenu button clicked")
                        }
                    }
                    states: [
                        State {
                            name: "hover"
                            when: elementSubMenu_MouseArea.containsMouse && !elementSubMenu_MouseArea.pressed
                            PropertyChanges {
                                target: elementSubMenu_Rect
                                color : palette.dark
                            }
                        },
                        State {
                            name: "pressed"
                            when: elementSubMenu_MouseArea.pressed

                        },
                        State {
                            name: "exit"
                            when: !elementSubMenu_MouseArea.containsMouse
                            PropertyChanges {
                                target: elementSubMenu_Rect
                                color: palette.darkThird
                            }
                        }
                    ]

                    transitions: Transition {
                        ColorAnimation { duration:100 }
                    }

                }
            }

        ListModel {
             id: listSubMenuModel
        }

        ListView {
            id: listOfSubMenu
            anchors.fill: parent
            delegate: subMenuDelegate
            model: listSubMenuModel
            focus: true
            ScrollBar.vertical:
                ScrollBar {
                snapMode: ScrollBar.SnapOnRelease
                contentItem.opacity: 0.2
            }
            snapMode: GridView.SnapToRow
            clip: false
            spacing:10
            anchors.topMargin: 10
            anchors.bottomMargin: 10
            boundsBehavior: Flickable.DragOverBounds
        }


    }
    function toggleSubMenu(open){
        //Toggle case
        if(open === undefined)
        {
            if(subMenu.x === 150) {
                subMenu.x = 0
                top.x = 0
            }else{
                top.x = -100
                subMenu.x = 150
            }
            return
        }
        //Forced case
        if(open === true) {
            top.x = -100
            subMenu.x = 150
        }
        else {
            top.x = 0
            subMenu.x = 0
        }
    }

    function manageSubMenu(index) {
        //filling the submenu zone
        listSubMenuModel.clear()
        switch(index) {
        case menus.HOME:
            //nothing here (?)
            break;
        case menus.FAVORITE:
            listSubMenuModel.append({message: qsTr("Add mounted volume to Favorites...")})
            //listSubMenuModel.append({message: qsTr("Add mounted volume to System Favorites...")}) not needed now
            listSubMenuModel.append({message: qsTr("Organize favorite volumes...")})
            //listSubMenuModel.append({message: qsTr("Organize System favorite volumes...")}) not needed now
            listSubMenuModel.append({message: qsTr("Mount favorite volumes")})
            break;
        case menus.TOOLS:
            listSubMenuModel.append({message: qsTr("Benchmark...")})
            listSubMenuModel.append({message: qsTr("Test vectors...")})
            listSubMenuModel.append({message: qsTr("Traveler Disk Setup...")})
            //listSubMenuModel.append({message: qsTr("Volume Creation Wizard...")}) not needed now
            listSubMenuModel.append({message: qsTr("Keyfile Generator")})
            listSubMenuModel.append({message: qsTr("Manage Security Token Keyfiles...")})
            listSubMenuModel.append({message: qsTr("Close All Security Token Sessions")})
            listSubMenuModel.append({message: qsTr("Backup Volume Header...")})
            listSubMenuModel.append({message: qsTr("Restore Volume Header...")})
            listSubMenuModel.append({message: qsTr("Refresh Drive Letters")}) //Windows ?
            listSubMenuModel.append({message: qsTr("Clear Volume History")})
            listSubMenuModel.append({message: qsTr("Wipe Cached Passwords")}) //when enable ?
            break;
        case menus.SETTINGS:
            listSubMenuModel.append({message: qsTr("Language...")})
            listSubMenuModel.append({message: qsTr("Hot Keys...")})
            //listSubMenuModel.append({message: qsTr("System Encryption...")}) not needed now
            //listSubMenuModel.append({message: qsTr("System Favorite Volumes...")}) not needed now
            listSubMenuModel.append({message: qsTr("Performance...")})
            listSubMenuModel.append({message: qsTr("Default Keyfiles...")})
            listSubMenuModel.append({message: qsTr("Security Tokens...")})
            listSubMenuModel.append({message: qsTr("Preferences...")})
            break;
        case menus.HELP:
            listSubMenuModel.append({message: qsTr("User's Guide")})
            listSubMenuModel.append({message: qsTr("Online Help")})
            listSubMenuModel.append({message: qsTr("Beginner's Tutorial")})
            listSubMenuModel.append({message: qsTr("Frequently Asked Questions")})
            listSubMenuModel.append({message: qsTr("GostCrypt Website")})
            listSubMenuModel.append({message: qsTr("Downloads")})
            listSubMenuModel.append({message: qsTr("News")})
            listSubMenuModel.append({message: qsTr("Version History")})
            listSubMenuModel.append({message: qsTr("Analyse a System Crash...")})
            listSubMenuModel.append({message: qsTr("Contact")})
            listSubMenuModel.append({message: qsTr("Legal Notices")})
            listSubMenuModel.append({message: qsTr("About")})
            break;
        }
    }


    /********************
          MENU PART
     ********************/
    Rectangle {
        id: rect
        color: palette.darkThird
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        radius: 0
    }

    GSMenuButtonItem {
        id: home
        y: 1
        text: qsTr("Home")
        iconPath_: icons[0]
        selected_: (selected == 0) ? true : false
        onClicked: {
            selected = menus.HOME
            manageSubMenu(selected)
            menuChanged("frames/HomeFrame.qml", menus.HOME)
            toggleSubMenu(false);
        }
    }

    GSMenuButtonItem {
        id: buttonFavorites
        y: home.y + 60
        text: qsTr("Favorites")
        iconPath_: icons[3]
        selected_: (selected === menus.FAVORITE) ? true : false
        onClicked: {
            if(selected === menus.FAVORITE) {
                toggleSubMenu();
                menuChanged("", menus.HOME)
            }else{

                selected = menus.FAVORITE
                manageSubMenu(selected)
                toggleSubMenu(true);
                menuChanged("", menus.FAVORITE)
            }

        }
    }
    GSMenuButtonItem {
        id: buttonTools
        y: buttonFavorites.y + 60
        text: qsTr("Tools")
        iconPath_: icons[4]
        selected_: (selected === menus.TOOLS) ? true : false
        onClicked: {
            if(selected === menus.TOOLS) {
                toggleSubMenu();
                menuChanged("", menus.HOME)
            }else{
                selected = menus.TOOLS
                manageSubMenu(selected)
                toggleSubMenu(true);
                menuChanged("", menus.TOOLS)
            }
        }
    }
    GSMenuButtonItem {
        id: buttonSettings
        y: buttonTools.y + 60
        text: qsTr("Settings")
        iconPath_: icons[5]
        selected_: (selected === menus.SETTINGS) ? true : false
        onClicked: {
            if(selected === menus.SETTINGS) {
                toggleSubMenu();
                menuChanged("", menus.HOME)
            }else{
                selected = menus.SETTINGS
                manageSubMenu(selected)
                toggleSubMenu(true);
                menuChanged("", menus.SETTINGS)
            }
        }
    }
    GSMenuButtonItem {
        id: buttonHelp
        y: buttonSettings.y + 60
        text: qsTr("Help")
        iconPath_: icons[6]
        selected_: (selected === menus.HELP) ? true : false
        onClicked: {
            if(selected === menus.HELP) {
                toggleSubMenu();
                menuChanged("", menus.HOME)
            }else{
                selected = menus.HELP
                manageSubMenu(selected)
                toggleSubMenu(true);
                menuChanged("", menus.HELP)
            }
        }
    }
}
