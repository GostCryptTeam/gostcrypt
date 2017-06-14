import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4

Item {
    id: connectSudo_Form
    property var childOf
    anchors.fill: childOf
    signal connection(string password)

    Connections {
        target: connectSudo_Form
        onConnection: {
            ConnectSignals.connectSudo(password);
            childOf.catchClose();
        }
    }

    Behavior on y { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad; } }

    Text {
        id: password_txt
        anchors.horizontalCenter: connectSudo_Form.horizontalCenter
        y: 60
        text: "Enter your sudo password: "
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
        onAccepted: connectSudo_Form.connection(password_value.text);
    }
}
