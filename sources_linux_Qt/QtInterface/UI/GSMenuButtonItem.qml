import QtQuick 2.6
import QtQuick.Controls 2.0

Button {
    id: buttonGreen
    signal clicked()
    width: 140
    height: 37

    //Partie texte
    contentItem: Text {
        text: buttonGreen.text
        font.pixelSize: 13
        color: buttonGreen.down ? "#ffffff" : "#ffffff"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    //Fond + icon
    background: Rectangle {
        id: buttonGreen_rect
        color: "#303030"
        implicitWidth: 160
        implicitHeight: 37
        border.width: 0
        radius: 4
        border.color: "#00000000"
    }

    //Capture du curseur
    MouseArea {
        id: buttonGreen_mouseArea
        onClicked: buttonGreen.clicked()
        anchors.fill: parent
        hoverEnabled: true
    }

    states: [
        State {
            name: "hover"
            when: buttonGreen_mouseArea.containsMouse && !buttonGreen_mouseArea.pressed
            PropertyChanges {
                target: buttonGreen_rect
                color : "#2a2a2a"
            }
        },
        State {
            name: "pressed"
            when: buttonGreen_mouseArea.pressed
            PropertyChanges {
                target: buttonGreen_rect
                color : "#1e1e1e"
            }
        },
        State {
            name: "exit"
            when: !buttonGreen_mouseArea.containsMouse
            PropertyChanges {
                target: buttonGreen_rect
                color : "#303030"
            }
        }
    ]

    transitions: Transition {
        ColorAnimation { duration:100 }
    }
}
