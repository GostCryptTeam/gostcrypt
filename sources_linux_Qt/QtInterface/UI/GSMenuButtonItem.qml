import QtQuick 2.6
import QtQuick.Controls 2.0

Button {
    id: buttonGreen
    signal clicked()
    width: 150
    height: 60
    property string iconPath_
    property bool selected_: false

    Image {
        x:10
        y:14
        fillMode: Image.PreserveAspectFit
        source: iconPath_
        height:32
    }

    //Partie texte
    contentItem: Text {
        text: buttonGreen.text
        font.pixelSize: 12
        color: palette.text
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        leftPadding: 47
        font.capitalization: Font.AllUppercase
    }

    //Fond + icon
    background: Rectangle {
        id: buttonGreen_rect
        color: (selected_ == true) ? palette.darkInput : palette.darkThird
        width:parent.width
        height:59
    }

    Rectangle {
        anchors.top: buttonGreen_rect.bottom
        width: 150
        height: 1
        color: palette.border
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
                color : palette.border
            }
        },
        State {
            name: "pressed"
            when: buttonGreen_mouseArea.pressed
            PropertyChanges {
                target: buttonGreen_rect
                color : palette.darkInput
            }
        },
        State {
            name: "exit"
            when: !buttonGreen_mouseArea.containsMouse
            PropertyChanges {
                target: buttonGreen_rect
                color: (selected_ == true) ? palette.darkInput : palette.darkThird
            }
        }
    ]

    transitions: Transition {
        ColorAnimation { duration:100 }
    }
}
