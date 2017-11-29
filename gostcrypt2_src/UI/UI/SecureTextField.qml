import QtQuick 2.7
import QtQuick.Controls 1.2 as ControlsOld
import QtQuick.Controls.Styles 1.4

ControlsOld.TextField {
    /* ID of the textinput */
    property int id: 0;
    property bool type: false
    property int radius_: 5
    property int height_: 24
    signal validated()
    id: top

    echoMode: (type === false) ? TextInput.Password : TextInput.Normal
    focus: true
    style: TextFieldStyle {
        textColor: "#e1e1e1"
        background: Rectangle {
            id: password_value_style
            radius: radius_
            implicitWidth: 100
            implicitHeight: height_
            border.color: "#333"
            border.width: 1
            color: palette.darkInput
        }
    }

    /* Keydown management */
    Keys.onReturnPressed: top.validated();
    Keys.onEnterPressed: top.validated();
}
