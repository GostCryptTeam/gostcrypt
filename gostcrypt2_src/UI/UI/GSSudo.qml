import QtQuick 2.7
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4

Item {
    id: connectSudo_Form
    anchors.centerIn: parent
    property string title: ""
    property string contentText: ""
    property bool isVisible: false

    MouseArea {
        anchors.fill:parent
    }

    Behavior on y {
        NumberAnimation {
            duration: app.duration;
            easing.type: Easing.OutQuad;
        }
    }

    Behavior on opacity {
        NumberAnimation {
            id: animation
            duration: app.duration;
            easing.type: Easing.OutQuad;
            onRunningChanged: {
                if(!animation.running && connectSudo_Form.isVisible == false) {
                    connectSudo_Form.visible = false
                }
            }
        }
    }

    Rectangle {
        width: app.width
        height: app.height
        x: -parent.x
        y: -parent.y + 40
        color: "#000000"
        opacity: 0.9
    }

    Rectangle {
        color: palette.darkThird
        anchors.centerIn: parent
        width: connectSudo_Form.width
        height: 165
        Rectangle {
            width: parent.width
            height: 3
            color: palette.blue
            x: 0
            y: 0
        }

        Text {
            id: password_txt
            anchors.horizontalCenter: parent.horizontalCenter
            y: 20
            text: contentText
            font.pointSize: 13
            lineHeightMode: Text.FixedHeight
            color: palette.text
        }

        SecureTextField {
            id: password_value
            y: password_txt.y + 40
            anchors.horizontalCenter: parent.horizontalCenter
            width: connectSudo_Form.width - 50
            horizontalAlignment: TextInput.AlignHCenter
            height: 40
            onValidated: {
                app.toggleSudo(0)
                sendSudoPassword(password_value.text);
            }
            onShiftPressed: {
                if(isPressed)
                    shift.opacity = 1.0
                else
                    shift.opacity = 0.0
            }
        }

        Image {
            id: shift
            fillMode: Image.PreserveAspectFit
            source: "ressource/caps.png"
            height: 40
            width: 40
            anchors.top: password_value.top
            anchors.right: password_value.left
            anchors.rightMargin: 5
            opacity: 0.0
            Behavior on opacity {
                NumberAnimation {
                    duration: app.duration/2;
                    easing.type: Easing.OutQuad;
                }
            }
        }

        GSButtonBordered {
            text: qsTr("Validate") + Translation.tr
            color_:palette.blue
            width:200
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: password_value.bottom
            onClicked: {
                app.toggleSudo(0)
                sendSudoPassword(password_value.text);
            }
            anchors.topMargin : 20
            anchors.bottomMargin: 40
        }
    }



    function setFocus()
    {
        password_value.focus = true;
    }
}
