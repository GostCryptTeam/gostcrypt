import QtQuick 2.6
import QtQuick.Controls 2.0

Button {
    id: buttonGreen
    signal clicked()
    width: 160
    height: 37

    //Partie texte
    contentItem: Text {
        text: buttonGreen.text
        //font: buttonGreenIcon.font
        font.pointSize: 11
        color: buttonGreen.down ? "#ffffff" : "#ffffff"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    //Fond + icon
    background: Rectangle {
        id: buttonGreen_rect
        color: "#719c24"
        implicitWidth: 160
        implicitHeight: 37
        border.width: 0
        radius: 2
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
                color : "#7ba430"
            }
        },
        State {
            name: "pressed"
            when: buttonGreen_mouseArea.pressed
            PropertyChanges {
                target: buttonGreen_rect
                color : "#506f19"
            }
        },
        State {
            name: "exit"
            when: !buttonGreen_mouseArea.containsMouse
            PropertyChanges {
                target: buttonGreen_rect
                color : "#719c24"
            }
        }
    ]

    transitions: Transition {
        ColorAnimation { duration:100 }
    }
}
