import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4

Item {
    id: connectSudo_Form
    anchors.fill : parent
    signal connection(string password)
    property string title: ""
    property string contentText: ""
    property bool isVisible: false

    MouseArea {
        anchors.fill:parent
    }

    Connections {
        target: connectSudo_Form
        onConnection: {
            ConnectSignals.connectSudo(password);
        }
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

    Text {
        id: password_txt
        anchors.horizontalCenter: connectSudo_Form.horizontalCenter
        y: 60
        text: contentText
        font.pointSize: 11
        lineHeightMode: Text.FixedHeight
        color: '#719c24'
    }

    TextField {
        id: password_value
        anchors.horizontalCenter: connectSudo_Form.horizontalCenter
        width: connectSudo_Form.width / 2
        y: password_txt.y + 30
        horizontalAlignment: TextInput.AlignHCenter
        echoMode: TextInput.Password
        style: TextFieldStyle {
            textColor: "#e1e1e1"
            background: Rectangle {
                radius: 5
                implicitWidth: 100
                implicitHeight: 24
                border.color: "#333"
                border.width: 1
                color: '#181818'
            }
        }
        onAccepted:
        {
            app.toggleSudo(0)
            connectSudo_Form.connection(password_value.text);
        }
        focus: true
    }
}
