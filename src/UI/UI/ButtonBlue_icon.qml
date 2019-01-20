import QtQuick 2.6
import QtQuick.Controls 2.0

Button {
    id: buttonBlueIcon
    signal clicked()
    width: 160
    height: 37

    //Partie texte
    contentItem: Text {
        leftPadding: -37
        text: buttonBlueIcon.text
        font.pixelSize: 13
        color: palette.textVeryLight
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    //Fond + icon
    background: Rectangle {
        id: buttonBlueIcon_rect
        Image {
            id: buttonBlueIcon_menuBars
            x: parent.width - 37
            y: 0
            width: 37
            height: 37
            enabled: false
            smooth: false
            source: "ressource/buttonBlueRight.png"
        }
        color: palette.dark
        implicitWidth: 160
        implicitHeight: 37
        border.width: 1
        radius: 2
        border.color: palette.blue
    }

    //Capture du curseur
    MouseArea {
        id: buttonBlueIcon_mouseArea
        onClicked: buttonBlueIcon.clicked()
        anchors.fill: parent
        hoverEnabled: true
    }

    states: [
        State {
            name: "hover"
            when: buttonBlueIcon_mouseArea.containsMouse && !buttonBlueIcon_mouseArea.pressed
            PropertyChanges {
                target: buttonBlueIcon_rect
                color : palette.blueHover
            }
        },
        State {
            name: "pressed"
            when: buttonBlueIcon_mouseArea.pressed
            PropertyChanges {
                target: buttonBlueIcon_rect
                color : palette.bluePress
            }
        },
        State {
            name: "exit"
            when: !buttonBlueIcon_mouseArea.containsMouse
            PropertyChanges {
                target: buttonBlueIcon_rect
                color : palette.dark
            }
        }
    ]

    transitions: Transition {
        ColorAnimation { duration:100 }
    }
}
