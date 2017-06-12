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
}
