import QtQuick 2.5

Rectangle {
    width: 46
    height: 46
    color: "#2f88a7"
    radius: 6
    border.color: "#00000000"
    property real value

    Item {
        anchors.fill: parent
        rotation: (state == "menu") ? value*180 : -value*180
        Rectangle {
            id: bar1
            x: 11 + 7.5*value
            y: 12 + 5*value
            width: 24 - 7*value
            height: 3
            rotation: 45*value
            antialiasing: true
        }

        Rectangle {
            id: bar2
            x: 11 + value
            y: 22 + 1*value
            width: 24 - 3*value
            height: 3
            antialiasing: true
        }

        Rectangle {
            id: bar3
            x: 11 + 7.5*value
            y: 32 - 4*value
            width: 24 - 7*value
            height: 3
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
                color : "#3d97b6"
            }
        },
        State {
            name: "pressed"
            when: menuButton_mouseArea.pressed
            PropertyChanges {
                target: menuButton
                color : "#266f88"
            }
        },
        State {
            name: "exit"
            when: !menuButton_mouseArea.containsMouse
            PropertyChanges {
                target: menuButton
                color : "#2f88a7"
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

