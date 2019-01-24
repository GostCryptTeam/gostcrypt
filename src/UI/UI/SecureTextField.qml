import QtQuick 2.7
import QtQuick.Controls 1.2 as ControlsOld
import QtQuick.Controls.Styles 1.4

ControlsOld.TextField {
    /* ID of the textinput */
    property int id: 0;
    property bool type: false
    property int radius_: 5
    property int height_: 24
    property color bordercolor: palette.borderInput
    signal validated()
    signal shiftPressed(bool isPressed)
    id: top

    echoMode: (type === false) ? TextInput.Password : TextInput.Normal
    focus: true
    style: TextFieldStyle {
        textColor: palette.text
        background: Rectangle {
            id: password_value_style
            radius: radius_
            implicitWidth: 100
            implicitHeight: height_
            border.color: bordercolor
            border.width: 1
            color: palette.darkInput
        }
    }

    /* Keydown management */
    Keys.onReturnPressed: top.validated();
    Keys.onEnterPressed: top.validated();

    Keys.onReleased: {
        if (event.key === Qt.Key_Shift || event.key === Qt.Key_Kana_Shift || event.key === Qt.Key_Eisu_Shift)
          top.shiftPressed(false);
    }

    Keys.onPressed: {
        if ((event.modifiers & Qt.ShiftModifier))
          top.shiftPressed(true);
        else
          top.shiftPressed(false);
        if(event.key === Qt.Key_CapsLock)
            top.shiftPressed(false);
        else if(isNaN(event.text * 1) && event.text.match(/[a-zA-Z]+/g) && event.text === event.text.toUpperCase())
            top.shiftPressed(true);
    }
}
