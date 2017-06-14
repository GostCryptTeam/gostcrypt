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
        //font: buttonGreenIcon.font
        color: buttonBlueIcon.down ? "#ffffff" : "#ffffff"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    //Fond + icon
    background: Rectangle {
        id: buttonBlueIcon_rect
        Image {
            id: buttonBlueIcon_menuBars
            x: parent.width - 42
            y: 0
            width: 42
            height: 37
            enabled: false
            smooth: false
            source: "../ressource/buttonBlueRight.png"
        }
        color: "#2f88a7"
        implicitWidth: 160
        implicitHeight: 37
        border.width: 0
        radius: 2
        border.color: "#00000000"
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
                color : "#479cb9"
            }
        },
        State {
            name: "pressed"
            when: buttonBlueIcon_mouseArea.pressed
            PropertyChanges {
                target: buttonBlueIcon_rect
                color : "#256a82"
            }
        },
        State {
            name: "exit"
            when: !buttonBlueIcon_mouseArea.containsMouse
            PropertyChanges {
                target: buttonBlueIcon_rect
                color : "#2f88a7"
            }
        }
    ]

    transitions: Transition {
        ColorAnimation { duration:100 }
    }
}
