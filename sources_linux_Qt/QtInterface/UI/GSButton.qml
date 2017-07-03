import QtQuick 2.6
import QtQuick.Controls 2.0

Button {
    id: buttonGreen
    signal clicked()
    width: 160
    height: 37
    property color color_
    property color colorHover_
    property color colorPress_

    //Partie texte
    contentItem: Text {
        text: buttonGreen.text
        //font: buttonGreenIcon.font
        font.pixelSize: 13
        color: buttonGreen.down ? "#ffffff" : "#ffffff"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    //Fond + icon
    background: Rectangle {
        id: buttonGreen_rect
        color: color_
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
                color : colorPress_
            }
        },
        State {
            name: "exit"
            when: !buttonGreen_mouseArea.containsMouse
            PropertyChanges {
                target: buttonGreen_rect
                color : color_
            }
        }
    ]

    transitions: Transition {
        ColorAnimation { duration:100 }
    }
}
