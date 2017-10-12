import QtQuick 2.7
import QtQuick.Controls 1.2 as ControlsOld
import QtQuick.Controls.Styles 1.4

ControlsOld.TextField {
    /* ID of the textinput */
    property int id: 0;

    echoMode: TextInput.Password
    focus: true
    style: TextFieldStyle {
        textColor: "#e1e1e1"
        background: Rectangle {
            id: password_value_style
            radius: 5
            implicitWidth: 100
            implicitHeight: 24
            border.color: "#333"
            border.width: 1
            color: palette.darkInput
        }
    }
    /* Keydown management */
    Keys.onPressed: {
        if (event.key === Qt.Key_A) {
            console.log("AH");
        }
    }
}
