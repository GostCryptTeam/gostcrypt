import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
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

    property int selected: 0

    /*
    RectangularGlow {
        id: effect
        anchors.fill: rect
        glowRadius: 10
        spread: 0.2
        color: palette.darkSecond
        cornerRadius: rect.radius + glowRadius
    }*/

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
    }
    GSMenuButtonItem {
        id: buttonVolumes
        y: home.y + 60
        text: qsTr("Volumes")
        iconPath_: icons[1]
        selected_: (selected == 1) ? true : false
    }
    GSMenuButtonItem {
        id: buttonSystem
        y: buttonVolumes.y + 60
        text: qsTr("System")
        iconPath_: icons[2]
        selected_: (selected == 2) ? true : false
    }
    GSMenuButtonItem {
        id: buttonFavorites
        y: buttonSystem.y + 60
        text: qsTr("Favorites")
        iconPath_: icons[3]
        selected_: (selected == 3) ? true : false
    }
    GSMenuButtonItem {
        id: buttonTools
        y: buttonFavorites.y + 60
        text: qsTr("Tools")
        iconPath_: icons[4]
        selected_: (selected == 4) ? true : false
    }
    GSMenuButtonItem {
        id: buttonSettings
        y: buttonTools.y + 60
        text: qsTr("Settings")
        iconPath_: icons[5]
        selected_: (selected == 5) ? true : false
    }
    GSMenuButtonItem {
        id: buttonHelp
        y: buttonSettings.y + 60
        text: qsTr("Help")
        iconPath_: icons[6]
        selected_: (selected == 6) ? true : false
    }
}
