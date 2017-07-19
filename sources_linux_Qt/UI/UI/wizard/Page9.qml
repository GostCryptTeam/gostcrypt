import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import "../" as UI

Item {
    id: top
    anchors.centerIn: parent
    property variant password: ["", ""]
    property int type: 0
    //password part

    Text {
        x: 52
        y:5
        color: palette.text
        text: qsTr("Please enter a password twice :")
    }

    TextField {
        id: password_value
        x: 52
        y: 30
        width: top.width*0.5
        horizontalAlignment: TextInput.AlignHCenter
        echoMode: TextInput.Password
        height: 40
        focus: true
        Keys.onReleased: password[0] = password_value.text
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
    }
    TextField {
        id: password_value2
        x: 52
        y: 80
        width: top.width*0.5
        horizontalAlignment: TextInput.AlignHCenter
        echoMode: TextInput.Password
        height: 40
        focus: true
        Keys.onReleased: password[1] = password_value2.text
        style: TextFieldStyle {
            textColor: "#e1e1e1"
            background: Rectangle {
                id: password_value_style2
                radius: 5
                implicitWidth: 100
                implicitHeight: 24
                border.color: "#333"
                border.width: 1
                color: palette.darkInput
            }
        }
    }

    UI.GSCheckBox {
        id: display
        text_: qsTr("Display password")
        x: password_value.x + password_value.width + 10
        y: 10
        height: 20
        checked: {
            var isChecked = UserSettings.getSetting("MountV-ShowPassword")
            return (isChecked == 1) ? true : false;
        }
        onCheckedChanged: {
            if(display.checked == true) {
                UserSettings.setSetting("MountV-ShowPassword", 1)
                password_value.echoMode = TextInput.Normal;
                password_value2.echoMode = TextInput.Normal;
            } else {
                UserSettings.setSetting("MountV-ShowPassword", 0)
                password_value.echoMode = TextInput.Password;
                password_value2.echoMode = TextInput.Password;
            }
        }

    }

    UI.GSCheckBox {
        id: use_Keyfiles
        text_: qsTr("Use keyfiles")
        x: password_value.x + password_value.width + 10
        y: display.y + 35
        height: 40
        checked: {
            var isChecked = UserSettings.getSetting("MountV-UseKeyFiles")
            return (isChecked == 1) ? true : false;
        }
        onCheckedChanged: {
            //TODO : action
            if(use_Keyfiles.checked == true) {
                UserSettings.setSetting("MountV-UseKeyFiles", 1)
            } else {
                UserSettings.setSetting("MountV-UseKeyFiles", 0)
            }
        }
    }

    UI.GSButtonBordered {
        id: buttonKeyfiles
        x: use_Keyfiles.x + 15
        y: use_Keyfiles.y + 45
        height: 40
        text: qsTr("Keyfiles...")
        width: 150
        color_: palette.green

    }

    Text {
        id:description2
        width: top.width-120
        font.pixelSize: 12
        text: qsTr("     It is very important that you choose a good password. You should avoid"
                   +" choosing one that contains only a single word that can be found in a dictionary (or a combination of 2, 3 or 4 such words)."
                   +" It should not contain any names or dates of birth. It sould not be easy to guess."
                   +" A good password is a random combination of upper and lower case letters, numbers, and special characters"
                   +", such as @  = $ * + etc. We recommend choosing a password consisting of more than 20 characters (the longer, the better)."
                   +" The maximum possible length is 64 characters.")
        y: use_Keyfiles.y + 95
        x: 60
        color: palette.text
        horizontalAlignment: Text.AlignJustify
        wrapMode: Text.WordWrap
    }
}
