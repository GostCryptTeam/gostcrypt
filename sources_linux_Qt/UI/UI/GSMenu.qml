import QtQuick 2.7
import QtQuick.Controls 2.2

Item {
    id: top
    x: -170
    width: 170
    property variant icons: [
        "ressource/menu_home.png",
        "ressource/menu_volume.png",
        "ressource/menu_system.png",
        "ressource/menu_favorite.png",
        "ressource/menu_tools.png",
        "ressource/menu_settings.png",
        "ressource/menu_help.png"]
    property variant menus: {
        "HOME": 0,
        "FAVORITE": 1,
        "TOOLS": 2,
        "SETTINGS": 3,
        "HELP": 4
    }

    property int selected: 0
    property string titleSubMenuText_: ""
    signal menuChanged(string name, int index)

    /********************
         SUBMENU PART
     ********************/
    Rectangle {
        id: subMenu
        x:-231
        width: 230
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
        Rectangle {
            color: "transparent"
            width: 230
            x:1
            height: 50
            Text {
                id: text_
                text: titleSubMenuText_ + Translation.tr
                color: palette.text
                anchors.centerIn: parent
                font.pixelSize: 15
                font.capitalization: Font.AllUppercase
            }
            Rectangle {
                height: 1
                width: parent.width-2
                color: palette.dark
                y:49
            }
        }

        Component {
            id: subMenuDelegateHead
            Rectangle {
                width: 228
                x: 1
                height:25
                color:palette.darkInput
                Text {
                    x:15
                    anchors.verticalCenter: parent.verticalCenter
                    text: section
                    font.pixelSize: 11
                    color: "#C9C9C9"
                    font.capitalization: Font.AllUppercase
                }
                Rectangle {
                    height: 1
                    width: parent.width
                    color: palette.dark
                    y:24
                }
            }
        }

        Component {
            id: subMenuDelegate
            Item {
                width: 228;
                x: 1
                height: {
                    if(size === "small")
                        if(finale !== undefined) 28;
                        else return 32
                    else if(size ==="medium")
                        if(finale !== undefined) 39;
                        else return 43
                    else
                        if(finale !== undefined) 50;
                        else return 54
                }
                Rectangle {
                    id: elementSubMenu
                    width: parent.width
                    height: {
                        if(finale !== undefined) parent.height;
                        else return parent.height - 4
                    }
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: palette.darkThird
                    Text {
                        horizontalAlignment: Text.AlignLeft
                        text: message + Translation.tr
                        color: palette.text
                        font.pixelSize: 14
                        wrapMode: Text.WordWrap
                        anchors.verticalCenter: parent.verticalCenter
                        x: 20
                        width: 200
                    }
                    MouseArea {
                        id: elementSubMenu_MouseArea
                        anchors.fill: elementSubMenu
                        hoverEnabled: true
                        onClicked: {
                            openSubWindow("dialogs/"+fileName+".qml", titleDialog, description, 429, {"name" : "", "value" : ""})
                            toggleMenu()
                        }
                    }
                    states: [
                        State {
                            name: "hover"
                            when: elementSubMenu_MouseArea.containsMouse && !elementSubMenu_MouseArea.pressed
                            PropertyChanges {
                                target: elementSubMenu
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
                                target: elementSubMenu
                                color: palette.darkThird
                            }
                        }
                    ]

                    transitions: Transition {
                        ColorAnimation { duration:100 }
                    }

                }
                Rectangle {
                    height: {
                        if(finale !== undefined) return 0
                        else return 4
                    }
                    width: 228
                    anchors {
                        left: elementSubMenu.left
                        right: elementSubMenu.right
                        top: elementSubMenu.bottom
                    }
                    color: palette.darkThird
                    Image {
                        height: parent.height
                        fillMode: Image.TileHorizontally
                        x:10
                        width: parent.width-20
                        horizontalAlignment: Image.AlignLeft
                        verticalAlignment: Image.AlignTop
                        source: "ressource/separator.png"
                    }
                }
            }

        }

        ListModel {
            id: listSubMenuModel
        }

        ListView {
            id: listOfSubMenu
            y:50
            width: parent.width
            height: parent.height-50
            delegate: subMenuDelegate
            model: listSubMenuModel
            focus: true
            snapMode: ScrollBar.SnapAlways
            clip: true
            anchors.topMargin: 50
            anchors.bottomMargin: 10
            boundsBehavior: Flickable.DragOverBounds
            ScrollBar.vertical:
                ScrollBar {
                snapMode: ScrollBar.SnapOnRelease
              //  contentItem.opacity: 0.1
                policy: ScrollBar.AsNeeded
                parent: listOfSubMenu.parent
                anchors.top: listOfSubMenu.top
                anchors.left: listOfSubMenu.left
                anchors.bottom: listOfSubMenu.bottom
                }
            section.property: "type"
            section.criteria: ViewSection.FullString
            section.delegate: subMenuDelegateHead
        }
    }


    function toggleSubMenu(open){
        //Toggle case
        if(open === undefined)
        {
            fullSizeSubMenu.opacity = fullSizeSubMenu.opacity === 0.0 ? 0.5 : 0.0
            mouseAreaMenu.enabled = mouseAreaMenu.enabled === true ? false : true
            mouseAreaMenu.visible = mouseAreaMenu.visible === true ? false : true
            if(subMenu.x > 0) {
                subMenu.x = -231
                top.x = 0
            }else{
                top.x = -120
                subMenu.x = 170
            }
            return
        }
        //Forced case
        if(open === true) {
            fullSizeSubMenu.opacity = 0.5
            mouseAreaMenu.enabled = true
            mouseAreaMenu.visible = true
            top.x = -120
            subMenu.x = 170
        }
        else {
            fullSizeSubMenu.opacity = 0.0
            mouseAreaMenu.enabled = false
            mouseAreaMenu.visible = false
            top.x = 0
            subMenu.x = -231
        }
    }

    function manageSubMenu(index) {
        //filling the submenu zone
        listSubMenuModel.clear()
        switch(index) {
        case menus.HOME:
            //nothing here (?)
            app.toggleMenu()
            break;
        case menus.FAVORITE:
            titleSubMenuText_ = qsTr("favorite")
            listSubMenuModel.append({message: qsTr("Add mounted volume to Favorites..."), size: "big", type: qsTr("menu"), fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            //listSubMenuModel.append({message: qsTr("Add mounted volume to System Favorites..."), size: "small"}) not needed now
            listSubMenuModel.append({message: qsTr("Organize favorite volumes..."), size: "big", type: qsTr("menu"), fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            //listSubMenuModel.append({message: qsTr("Organize System favorite volumes..."), size: "small"}) not needed now
            listSubMenuModel.append({message: qsTr("Mount favorite volumes"), size: "medium", type: qsTr("menu"), finale:"true", fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            break;
        case menus.TOOLS:
            titleSubMenuText_ = qsTr("tools")
            listSubMenuModel.append({message: qsTr("Benchmark..."), size: "small", type: qsTr("tests"), fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            listSubMenuModel.append({message: qsTr("Test vectors..."), size: "small", type: qsTr("tests"), finale:"true", fileName: "", titleDialog: qsTr(""), description: qsTr("")})

            //listSubMenuModel.append({message: qsTr("Volume Creation Wizard..."), size: "small"}) not needed now

            listSubMenuModel.append({message: qsTr("Keyfile Generator"), size: "medium", type: qsTr("keyfiles"), fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            listSubMenuModel.append({message: qsTr("Manage Security Token Keyfiles..."), size: "big", type: qsTr("keyfiles"), fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            listSubMenuModel.append({message: qsTr("Close All Security Token Sessions"), size: "big", type: qsTr("keyfiles"), finale:"true", fileName: "", titleDialog: qsTr(""), description: qsTr("")})

            listSubMenuModel.append({message: qsTr("Backup Volume Header..."), size: "big", type: qsTr("volume header"), fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            listSubMenuModel.append({message: qsTr("Restore Volume Header..."), size: "big", type: qsTr("volume header"), finale:"true", fileName: "", titleDialog: qsTr(""), description: qsTr("")})

            listSubMenuModel.append({message: qsTr("Clear Volume History"), size: "medium", type: qsTr("other"), finale:"true", fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            break;
        case menus.SETTINGS:
            titleSubMenuText_ = qsTr("settings")
            listSubMenuModel.append({message: qsTr("Language..."), size: "small", type: qsTr("user settings"), fileName: "GSLanguage", titleDialog: qsTr("GostCrypt")+ Translation.tr, description: qsTr("Change GostCrypt Language")})
            listSubMenuModel.append({message: qsTr("Hot Keys..."), size: "small", type: qsTr("user settings"), finale:"true", fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            //listSubMenuModel.append({message: qsTr("System Encryption..."), size: "small"}) not needed now
            //listSubMenuModel.append({message: qsTr("System Favorite Volumes..."), size: "small"}) not needed now

            listSubMenuModel.append({message: qsTr("Performance..."), size: "medium", type: qsTr("performance"), finale:"true", fileName: "", titleDialog: qsTr(""), description: qsTr("")})

            listSubMenuModel.append({message: qsTr("Default Keyfiles..."), size: "medium", type: qsTr("keyfiles"), fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            listSubMenuModel.append({message: qsTr("Security Tokens..."), size: "medium", type: qsTr("keyfiles"), finale:"true", fileName: "", titleDialog: qsTr(""), description: qsTr("")})

            listSubMenuModel.append({message: qsTr("Preferences..."), size: "small", type: qsTr(" "), finale:"true", fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            break;
        case menus.HELP:
            titleSubMenuText_ = qsTr("help")
            listSubMenuModel.append({message: qsTr("User's Guide"), size: "small", type: qsTr("help"), fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            listSubMenuModel.append({message: qsTr("Online Help"), size: "small", type: qsTr("help"), fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            listSubMenuModel.append({message: qsTr("Beginner's Tutorial"), size: "medium", type: qsTr("help"), fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            listSubMenuModel.append({message: qsTr("Frequently Asked Questions"), size: "big", type: qsTr("help"), finale:"true", fileName: "", titleDialog: qsTr(""), description: qsTr("")})

            listSubMenuModel.append({message: qsTr("GostCrypt Website"), size: "medium", type: qsTr("web"), fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            listSubMenuModel.append({message: qsTr("Downloads"), size: "small", type: qsTr("web"), fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            listSubMenuModel.append({message: qsTr("News"), size: "small", type: qsTr("web"), fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            listSubMenuModel.append({message: qsTr("Version History"), size: "medium", type: qsTr("web"), fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            listSubMenuModel.append({message: qsTr("Analyse a System Crash..."), size: "big", type: qsTr("web"), finale:"true", fileName: "", titleDialog: qsTr(""), description: qsTr("")})

            listSubMenuModel.append({message: qsTr("Contact"), size: "small", type: qsTr("information"), fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            listSubMenuModel.append({message: qsTr("Legal Notices"), size: "medium", type: qsTr("information"), fileName: "", titleDialog: qsTr(""), description: qsTr("")})
            listSubMenuModel.append({message: qsTr("About"), size: "small", type: qsTr("information"), finale:"true", fileName: "", titleDialog: qsTr(""), description: qsTr("")})
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
        border {
            color:palette.border
            width: 1
        }
    }

    GSMenuButtonItem {
        id: home
        y: 1
        text: qsTr("Home") + Translation.tr
        iconPath_: icons[0]
        selected_: (selected == 0) ? true : false
        onClicked: {
            selected = menus.HOME
            manageSubMenu(selected)
            menuChanged("frames/HomeFrame.qml", menus.HOME)
        }
    }

    GSMenuButtonItem {
        id: buttonFavorites
        y: home.y + 60
        text: qsTr("Favorites") + Translation.tr
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
        text: qsTr("Tools") + Translation.tr
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
        text: qsTr("Settings") + Translation.tr
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
        text: qsTr("Help") + Translation.tr
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
