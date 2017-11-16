import QtQuick 2.7
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4

Item {
    id: connectSudo_Form
    anchors.fill : parent
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
        anchors.fill: connectSudo_Form
        color: "#000000"
        opacity: 0.9
    }

    Rectangle {
        color: palette.darkThird
        anchors.centerIn: parent
        width: connectSudo_Form.width / 2
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
            width: connectSudo_Form.width / 2 - 50
            horizontalAlignment: TextInput.AlignHCenter
            height: 25
            onValidated: {
                app.toggleSudo(0)
                sendSudoPassword(password_value.text);
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
