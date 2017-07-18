import QtQuick 2.5

Rectangle {
    width: 46
    height: 46
    color: "transparent"//"#2f88a7"
    radius: 46
    border.color: palette.blue
    property real value

    Item {
        anchors.fill: parent
        rotation: (state == "menu") ? value*180 : -value*180
        Rectangle {
            color: palette.text
            id: bar1
            x: 14 + 7.5*value
            y: 14 + 5*value
            width: 18 - 7*value
            height: 2
            rotation: 45*value
            antialiasing: true
        }

        Rectangle {
            color: palette.text
            id: bar2
            x: 14 + value
            y: 22 + 1*value
            width: 18 - 3*value
            height: 2
            antialiasing: true
        }

        Rectangle {
            color: palette.text
            id: bar3
            x: 14 + 7.5*value
            y: 30 - 4*value
            width: 18 - 7*value
            height: 2
            rotation: -45*value
            antialiasing: true
        }

    }

    MouseArea {
        id: menuButton_mouseArea
        hoverEnabled: true
        anchors.fill: parent

        onClicked: {
          app.toggleMenu()
        }
    }

    state: "menu"
    states: [
        State {
            name: "hover"
            when: menuButton_mouseArea.containsMouse && !menuButton_mouseArea.pressed
            PropertyChanges {
                target: menuButton
                color : palette.darkInput
            }
        },
        State {
            name: "pressed"
            when: menuButton_mouseArea.pressed
            PropertyChanges {
                target: menuButton
                color : palette.border
            }
        },
        State {
            name: "exit"
            when: !menuButton_mouseArea.containsMouse
            PropertyChanges {
                target: menuButton
                color : "transparent"
            }
        },
        State { name: "menu" },
        State { name: "back" }
    ]

    transitions: Transition {
        ColorAnimation { duration:100 }
    }


    onValueChanged: {
        if(value == 1) state = "back"
        else if(value == 0) state = "menu"
    }
}

