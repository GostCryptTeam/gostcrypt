import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    x:-150
    width: 150

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#303030"
    }

    RectangularGlow {
        id: effect
        anchors.fill: rect
        glowRadius: 10
        spread: 0.2
        color: "#272727"
        cornerRadius: rect.radius + glowRadius
    }

    Rectangle {
        id: rect
        color: "#303030"
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        radius: 0
    }
    //logo
    Rectangle {
        id: logoTop
        x:17
        y:10
        width:150
        //height: 100
        Image {
            fillMode: Image.PreserveAspectFit
            source: "/ressource/logoMenu.png"
        }
    }

    GSMenuButtonItem {
        id: buttonVolumes
        x : 5
        y: logoTop.y + 70
        text: qsTr("Volumes")
    }
    GSMenuButtonItem {
        id: buttonSystem
        x : 5
        y: buttonVolumes.y + buttonVolumes.height + 5
        text: qsTr("System")
    }
    GSMenuButtonItem {
        id: buttonFavorites
        x : 5
        y: buttonSystem.y + buttonSystem.height + 5
        text: qsTr("Favorites")
    }
    GSMenuButtonItem {
        id: buttonTools
        x : 5
        y: buttonFavorites.y + buttonFavorites.height + 5
        text: qsTr("Tools")
    }
    GSMenuButtonItem {
        id: buttonSettings
        x : 5
        y: buttonTools.y + buttonTools.height + 5
        text: qsTr("Settings")
    }
    GSMenuButtonItem {
        id: buttonHelp
        x : 5
        y: buttonSettings.y + buttonSettings.height + 5
        text: qsTr("Help")
    }
}
