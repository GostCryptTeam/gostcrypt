import QtQuick 2.6
import QtQuick.Controls 2.0

Button {
    id: buttonBordered
    signal clicked()
    width: 160
    height: 37
    property color color_

    //Partie texte
    contentItem: Text {
        text: buttonBordered.text + Translation.tr
        //font: buttonBorderedIcon.font
        font.pixelSize: 13
        color: (buttonBordered.enabled) ? palette.textLight : palette.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    //Fond + icon
    background: Rectangle {
        id: buttonBordered_rect
        color: palette.darkSecond
        implicitWidth: 160
        implicitHeight: 37
        border.width: 2
        radius: 8
        border.color: color_
    }

    //Capture du curseur
    MouseArea {
        id: buttonBordered_mouseArea
        onClicked: buttonBordered.clicked()
        anchors.fill: parent
        hoverEnabled: true
    }

    states: [
        State {
            name: "hover"
            when: buttonBordered_mouseArea.containsMouse && !buttonBordered_mouseArea.pressed
            PropertyChanges {
                target: buttonBordered_rect
                color : palette.darkThird
            }
        },
        State {
            name: "pressed"
            when: buttonBordered_mouseArea.pressed
            PropertyChanges {
                target: buttonBordered_rect
                color : "#1e1e1e"
            }
        },
        State {
            name: "exit"
            when: !buttonBordered_mouseArea.containsMouse
            PropertyChanges {
                target: buttonBordered_rect
                color : (buttonBordered.enabled) ? palette.darkSecond : palette.dark
                border.color: (buttonBordered.enabled) ? color_ : palette.border
            }
        }
    ]

    transitions: Transition {
        ColorAnimation { duration:100 }
    }

    function setDisable(bool)
    {
        if(bool){
            buttonBordered_rect.border.color = palette.darkThird
        }else{
            buttonBordered_rect.border.color = color_
        }
    }
}
