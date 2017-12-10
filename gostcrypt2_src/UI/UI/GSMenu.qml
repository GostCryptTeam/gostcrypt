import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

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
        Item {
            id: loaderSub
            anchors.fill:parent
            visible: false
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
                                //Subwindow mode
                                if(fileName !== undefined)
                                {
                                    var s = 429;
                                    if(size_ !== undefined)
                                        openSubWindow("dialogs/"+fileName+".qml", titleDialog, description, size_, {"name" : "", "value" : ""})
                                    else
                                        openSubWindow("dialogs/"+fileName+".qml", titleDialog, description, s, {"name" : "", "value" : ""})

                                    toggleMenu()
                                }
                                //Action mode
                                else if(action !== undefined)
                                {
                                    qmlRequest(action, {"name": name_, "desc": desc_})
                                }
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
                snapMode: ScrollBar.NoSnap
                clip: true
                anchors.topMargin: 50
                boundsBehavior: Flickable.DragOverBounds
                ScrollBar.vertical:
                    ScrollBar {
                        id: scrollbar
                        policy: ScrollBar.AlwaysOn
                        snapMode: ScrollBar.SnapOnRelease
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
        Item {
            id: loaderFavoriteSub
            visible: false
            anchors.fill:parent
            Rectangle {
                y: subMenu.y + subMenu.height - 129
                x: 1
                width: 1
                height:100
                color: palette.darkThird
            }
            Rectangle {
                y: subMenu.y + subMenu.height - 130
                x: 1
                width: parent.width-2
                height:1
                color: palette.dark
            }
            Rectangle {
                x:2
                width: parent.width-2
                height:149
                y: subMenu.y + subMenu.height - 129
                color: palette.darkInput
                Text {
                    y: 10
                    x: 10
                    text: "Double-click on a path to<br>mount the volume.<br>Or mount all favorite volumes :"
                    color: palette.text
                    width: parent.width - 20
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 13
                }
                GSButtonBordered {
                    text: qsTr("Mount favorite volumes") + Translation.tr
                    color_: palette.green
                    anchors.horizontalCenter: parent.horizontalCenter
                    y: 75
                    onClicked: {
                        //Mount all favorite volumes
                        var volumes = UserSettings.getFavoritesVolumes()
                        //TODO
                    }
                }

            }
            Rectangle {
                y: subMenu.y + subMenu.height - 130
                x: parent.width-2
                width: 1
                height:100
                color: palette.darkThird
            }


            Component {
                id: subMenuDelegateFavorite
                Item {
                    y:1
                    width: 228;
                    x: 1
                    height: 60
                    Rectangle {
                        id: favoriteElement
                        anchors.fill:parent;
                        color: {
                            if(mounted === false) return palette.darkThird
                            return palette.darkInput
                        }
                        Text {
                            id: textInfos
                            anchors.verticalCenter: parent.verticalCenter
                            x: 10
                            width: 180
                            text: (name !== undefined && name !== "") ? name + "  (\"" + path + "\")" : path
                            color: palette.text
                            elide: Text.ElideRight
                        }
                        Text {
                            visible: {
                                if(mounted === false) return false
                                else return true
                            }
                            text : qsTr("(Mounted)")
                            anchors {
                                bottom: parent.bottom
                                left: parent.left
                                leftMargin: 10
                                bottomMargin: 10
                            }
                            horizontalAlignment: Text.AlignRight
                            font.pixelSize: 11
                            color: palette.text
                            opacity: 0.5
                        }

                        ToolTip {
                            parent: favoriteElement
                            text: path
                            visible: favoriteElementMouseArea.containsMouse && !unFavButtonArea.containsMouse && !toolFavArea.containsMouse
                            delay: 500
                            timeout: 5000
                        }
                        Text {
                            id: unFavButton
                            anchors.right: parent.right
                            y: -5
                            anchors.rightMargin: 15
                            text: "×"
                            color: palette.text
                            font.pixelSize: 30
                            opacity: 0.0

                            Behavior on opacity {
                                NumberAnimation {
                                    duration: app.duration/2;
                                    easing.type: Easing.OutQuad;
                                }
                            }
                            ToolTip {
                                parent: unFavButton
                                text: qsTr("Remove from favorites...")
                                visible: unFavButtonArea.containsMouse
                                delay: 500
                                timeout: 5000
                            }
                        }

                        Image {
                            id: propertiesFav
                            source: "ressource/volumeTools.png"
                            anchors.right: parent.right
                            anchors.rightMargin: 12
                            x:0
                            y:32
                            width:20
                            height:20
                            opacity: 0.0
                            Behavior on opacity {
                                NumberAnimation {
                                    duration: app.duration/2;
                                    easing.type: Easing.OutQuad;
                                }
                            }
                            ToolTip {
                                parent: toolFavArea
                                text: qsTr("Favorite Volume options...")
                                visible: toolFavArea.containsMouse
                                delay: 500
                                timeout: 5000
                            }
                        }

                        ColorOverlay{
                            id: overlayImage
                            anchors.fill: propertiesFav
                            source:propertiesFav
                            color: palette.blue
                            visible: false
                            antialiasing: true
                        }
                    }

                    MouseArea {
                        id: favoriteElementMouseArea
                        anchors.fill: favoriteElement
                        hoverEnabled: true
                        propagateComposedEvents: false
                        onEntered: {
                            unFavButton.opacity = 1.0
                            propertiesFav.opacity = 1.0
                        }
                        onExited: {
                            unFavButton.opacity = 0.0
                            propertiesFav.opacity = 0.0
                        }
                        onDoubleClicked: {
                            //TODO
                            if(mounted === false){
                                openSubWindow("dialogs/OpenVolume.qml", qsTr('Open a GostCrypt volume'), qsTr("Mount a volume"), 429, {"name" : "dropVolume", "value" : path})
                                toggleMenu();
                            }
                            else qmlRequest("openmountpoint", {"path": mountPoint});
                         }

                        MouseArea {
                            id:unFavButtonArea
                            width: 50
                            height: 30
                            x: 178
                            hoverEnabled: true
                            onEntered: unFavButton.color = palette.blue
                            onExited: unFavButton.color = palette.text
                            onClicked: {
                                //TODO : unfav path
                                UserSettings.setFavoritesVolumes(path);
                                listSubMenuModelFavorite.clear();
                                manageSubMenu(menus.FAVORITE)
                                //check if favorite un homeframe
                            }
                        }
                        MouseArea {
                            id: toolFavArea
                            width:50
                            height: 30
                            x: 178
                            y: 30
                            hoverEnabled: true
                            onEntered: overlayImage.visible = true
                            onExited: overlayImage.visible = false
                            onClicked: {
                                openSubWindow("dialogs/FavoriteVolumeOptions.qml", qsTr("Manage favorite volume"), qsTr("Favorites"), 429, {"name" : "favorite-volumes", "value" : path})
                            }
                        }

                    }
                    states: [
                        State {
                            name: "hover"
                            when: favoriteElementMouseArea.containsMouse && !favoriteElementMouseArea.pressed
                            PropertyChanges {
                                target: favoriteElement
                                color : {
                                    if(mounted === false) return palette.dark
                                    return palette.darkInput
                                }
                            }
                        },
                        State {
                            name: "pressed"
                            when: favoriteElementMouseArea.pressed

                        },
                        State {
                            name: "exit"
                            when: !favoriteElementMouseArea.containsMouse
                            PropertyChanges {
                                target: favoriteElement
                                color: {
                                    if(mounted === false) return palette.darkThird
                                    return palette.darkInput
                                }
                            }
                        }
                    ]

                    transitions: Transition {
                        ColorAnimation { duration:100 }
                    }
                }

            }

            ListModel {
                id: listSubMenuModelFavorite
            }

            ListView {
                id: listOfSubMenuFavorite
                y:50
                width: parent.width
                height: parent.height-180
                delegate: subMenuDelegateFavorite
                model: listSubMenuModelFavorite
                focus: true
                snapMode: ScrollBar.SnapAlways
                clip: true
                anchors.topMargin: 50
                anchors.bottomMargin: 10
                boundsBehavior: Flickable.DragOverBounds
                ScrollBar.vertical:
                    ScrollBar {
                    snapMode: ScrollBar.SnapOnRelease
                    policy: ScrollBar.AsNeeded
                    parent: listOfSubMenuFavorite.parent
                    anchors.top: listOfSubMenuFavorite.top
                    anchors.left: listOfSubMenuFavorite.left
                    anchors.bottom: listOfSubMenuFavorite.bottom
                    }
            }
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
        listSubMenuModelFavorite.clear()
        loaderSub.visible = true
        loaderFavoriteSub.visible = false
        switch(index) {
        case menus.HOME:
            //nothing here (?)
            app.toggleMenu()
            break;
        case menus.FAVORITE:
            loaderSub.visible = false
            loaderFavoriteSub.visible = true
            titleSubMenuText_ = qsTr("favorite")
            updateFavorites();
            qmlRequest("mountedvolumes", "");
            break;
        case menus.TOOLS:
            titleSubMenuText_ = qsTr("tools")
            listSubMenuModel.append({message: qsTr("Benchmark..."), size: "small", type: qsTr("tests"), fileName: "Benchmark", titleDialog: qsTr("Benchmark")+Translation.tr, description: qsTr("Benchmark"), size_: 429})
            listSubMenuModel.append({message: qsTr("Test vectors..."), size: "small", type: qsTr("tests"), finale:"true", fileName: "TestVectors", titleDialog: qsTr("Test Vectors"), description: qsTr("Test Vectors"), size_: 429})       

            listSubMenuModel.append({message: qsTr("Keyfile Generator"), size: "medium", type: qsTr("keyfiles"), fileName: "KeyfileGenerator", finale:"true", titleDialog: qsTr("Keyfile Generator"), description: qsTr("Keyfile generator"), size_: 200})

            listSubMenuModel.append({message: qsTr("Clear Volume History"), size: "big", type: qsTr("other"), finale:"true", action: "clearvolumehistory", size_: 429, name_: "Clearing volume paths...", desc_: "Deleting volume paths saved by gostcrypt"})
            break;
        case menus.SETTINGS:
            titleSubMenuText_ = qsTr("settings")
            listSubMenuModel.append({message: qsTr("Language..."), size: "small", type: qsTr("user settings"), finale:"true", fileName: "GSLanguage", titleDialog: qsTr("GostCrypt")+ Translation.tr, description: qsTr("Change GostCrypt Language"), size_: 429})
            listSubMenuModel.append({message: qsTr("Performance..."), size: "medium", type: qsTr("performance"), finale:"true", fileName: "Performance", titleDialog: qsTr("GostCrypt Performances"), description: qsTr("Performance Options"), size_: 429})

            listSubMenuModel.append({message: qsTr("Default Keyfiles..."), size: "medium", type: qsTr("keyfiles"), finale:"true", fileName: "DefaultKeyfile", titleDialog: qsTr("Default Keyfiles"), description: qsTr("Default Keyfiles"), size_: 429})

            listSubMenuModel.append({message: qsTr("Preferences..."), size: "small", type: qsTr(" "), finale:"true", fileName: "Preferences", titleDialog: qsTr("GostCrypt Preferences"), description: qsTr("Preferences"), size_: 429})
            break;
        case menus.HELP:
            titleSubMenuText_ = qsTr("help")
            listSubMenuModel.append({message: qsTr("User's Guide"), size: "small", type: qsTr("help"), action: "openguide", name_:"", desc:""})
            listSubMenuModel.append({message: qsTr("Online Help"), size: "small", type: qsTr("help"), action: "onlinehelp", name_:"", desc:""})
            listSubMenuModel.append({message: qsTr("Beginner's Tutorial"), size: "medium", type: qsTr("help"), action: "beginnertuto"})
            listSubMenuModel.append({message: qsTr("Frequently Asked Questions"), size: "big", type: qsTr("help"), finale:"true", action: "faq", name_:"", desc:""})

            listSubMenuModel.append({message: qsTr("GostCrypt Website"), size: "medium", type: qsTr("web"), action: "website", name_:"", desc:""})
            listSubMenuModel.append({message: qsTr("Downloads"), size: "small", type: qsTr("web"), action: "download", name_:"", desc:""})
            listSubMenuModel.append({message: qsTr("News"), size: "small", type: qsTr("web"), action: "news", name_:"", desc:""})
            listSubMenuModel.append({message: qsTr("Version History"), size: "medium", type: qsTr("web"), finale:"true", action: "versionhistory", name_:"", desc:""})

            listSubMenuModel.append({message: qsTr("Contact"), size: "small", type: qsTr("information"), action: "contact", name_:"", desc:""})
            listSubMenuModel.append({message: qsTr("Legal Notices"), size: "medium", type: qsTr("information"), fileName: "License", titleDialog: qsTr("GostCrypt License"), description: qsTr("License")})
            listSubMenuModel.append({message: qsTr("About"), size: "big", type: qsTr("information"), finale:"true", fileName: "Contributors", titleDialog: qsTr("About GostCrypt"), description: qsTr("About GostCrypt")})
            break;
        }
    }

    function updateFavorites()
    {
        listSubMenuModelFavorite.clear();
        var favorites = UserSettings.getFavoritesVolumes()
        var isMounted = false;
        loop1:
        for(var i = 0; i< favorites.length; i+=2) {
            for(var k in app.model)
            {
                if(app.model[k]["volumePath"] === favorites[i])
                {
                    //The volume is mounted
                    listSubMenuModelFavorite.append({name: favorites[i+1], path: favorites[i], mounted: true, mountPoint: app.model[k]["mountPoint"]});
                    continue loop1;
                }
            }
            //the volume is not mounted
            listSubMenuModelFavorite.append({name: favorites[i+1], path: favorites[i], mounted: false, mountPoint: ""});
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
                loaderSub.visible = false
                loaderFavoriteSub.visible = true
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
