import QtQuick 2.6
import QtQuick.Controls 2.0

Button {
    id: buttonBlueBordered
    signal clicked()
    width: 160
    height: 37

    //Partie texte
    contentItem: Text {
        text: buttonBlueBordered.text
        //font: buttonBlueBorderedIcon.font
        font.pixelSize: 13
        color: "#e1e1e1"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    //Fond + icon
    background: Rectangle {
        id: buttonBlueBordered_rect
        color: "#e1e1e1"
        implicitWidth: 160
        implicitHeight: 37
        border.width: 2
        radius: 8
        border.color: "#2f88a7"
    }

    //Capture du curseur
    MouseArea {
        id: buttonBlueBordered_mouseArea
        onClicked: buttonBlueBordered.clicked()
        anchors.fill: parent
        hoverEnabled: true
    }

    states: [
        State {
            name: "hover"
            when: buttonBlueBordered_mouseArea.containsMouse && !buttonBlueBordered_mouseArea.pressed
            PropertyChanges {
                target: buttonBlueBordered_rect
                color : "#272727"
            }
        },
        State {
            name: "pressed"
            when: buttonBlueBordered_mouseArea.pressed
            PropertyChanges {
                target: buttonBlueBordered_rect
                color : "#1e1e1e"
            }
        },
        State {
            name: "exit"
            when: !buttonBlueBordered_mouseArea.containsMouse
            PropertyChanges {
                target: buttonBlueBordered_rect
                color : "#2a2a2a"
            }
        }
    ]

    transitions: Transition {
        ColorAnimation { duration:100 }
    }
}
