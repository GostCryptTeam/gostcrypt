import QtQuick 2.6
import QtQuick.Controls 2.0

Button {
    id: buttonMenu
    signal clicked()
    width: 150
    height: 60
    property string iconPath_
    property bool selected_: false

    Image {
        id: form
        x:parent.width-42
        y:14
        fillMode: Image.PreserveAspectFit
        source: iconPath_
        height:32
        ToolTip {
            parent: form
            text: buttonMenu.text
            visible: buttonMenu_mouseArea.containsMouse
            delay: 500
            timeout: 5000
        }
    }

    //Partie texte
    contentItem: Text {
        text: buttonMenu.text
        font.pixelSize: 12
        color: palette.text
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        rightPadding: 47
        font.capitalization: Font.AllUppercase
    }

    //Fond + icon
    background: Rectangle {
        id: buttonMenu_rect
        color: (selected_ == true) ? palette.darkInput : palette.darkThird
        width:parent.width
        height:59
    }

    Rectangle {
        anchors.top: buttonMenu_rect.bottom
        width: 150
        height: 1
        color: palette.border
    }

    //Capture du curseur
    MouseArea {
        id: buttonMenu_mouseArea
        onClicked: buttonMenu.clicked()
        anchors.fill: parent
        hoverEnabled: true
    }

    states: [
        State {
            name: "hover"
            when: buttonMenu_mouseArea.containsMouse && !buttonMenu_mouseArea.pressed
            PropertyChanges {
                target: buttonMenu_rect
                color : palette.border
            }
        },
        State {
            name: "pressed"
            when: buttonMenu_mouseArea.pressed
            PropertyChanges {
                target: buttonMenu_rect
                color : palette.darkInput
            }
        },
        State {
            name: "exit"
            when: !buttonMenu_mouseArea.containsMouse
            PropertyChanges {
                target: buttonMenu_rect
                color: (selected_ == true) ? palette.darkInput : palette.darkThird
            }
        }
    ]

    transitions: Transition {
        ColorAnimation { duration:100 }
    }
}
