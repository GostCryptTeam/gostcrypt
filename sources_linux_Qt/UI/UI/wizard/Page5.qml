import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import "../" as UI

Item {
    id: top
    anchors.centerIn: parent
    property string password: ""
    //password part
    TextField {
        id: password_value
        x: 52
        y: 10
        width: top.width-100
        horizontalAlignment: TextInput.AlignHCenter
        echoMode: TextInput.Password
        height: 40
        focus: true
        Keys.onPressed: password = password_value.text+event.text
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

    UI.GSCheckBox {
        id: display
        text_: qsTr("Display password")
        x: 50
        y: 50
        height: 40
        checked: {
            var isChecked = UserSettings.getSetting("MountV-ShowPassword")
            return (isChecked == 1) ? true : false;
        }
        onCheckedChanged: {
            if(display.checked == true) {
                UserSettings.setSetting("MountV-ShowPassword", 1)
                password_value.echoMode = TextInput.Normal;
            } else {
                UserSettings.setSetting("MountV-ShowPassword", 0)
                password_value.echoMode = TextInput.Password;
            }
        }

    }

    UI.GSCheckBox {
        id: use_Keyfiles
        text_: qsTr("Use keyfiles")
        x: 50
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
        x: password_value.x + password_value.width - 150
        y: 60
        height: 40
        text: qsTr("Keyfiles...")
        width: 150
        color_: palette.green

    }

    Text {
        id:description2
        width: top.width-120
        font.pixelSize: 12
        text: qsTr("Please enter the password for the volume within which you wish to create a hidden volume."
                   +" <br><br>After you click Next, GostCrypt will attempt to mount the volume. As soon as the volume is mounted"
                   +", its cluster bitmap will be scanned to determine the size of the uninterrupted area of free space (if"
                   +" there is any) whose end is aligned with the end of the volume. This area will accommodate"
                   +" the hidden volume and therefore will limit its maximum possible size. Cluster map scanning is"
                   +" necessary to ensure that no data on the outer volume will be overwritten by the hidden volume.") + Translation.tr
        y: use_Keyfiles.y + 45
        x: 60
        color: palette.text
        horizontalAlignment: Text.AlignJustify
        wrapMode: Text.WordWrap
    }
}
