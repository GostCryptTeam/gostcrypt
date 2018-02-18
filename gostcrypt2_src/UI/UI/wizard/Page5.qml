import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import "../" as UI

Item {
    id: top
    anchors.centerIn: parent
    property string password: ""
    property variant listKeyfiles: []

    FileDialog {
        id: addKeyfiles
        title: qsTr("Please choose a keyfile") + Translation.tr
        folder: shortcuts.home
        selectMultiple: true
        onAccepted: {
            use_Keyfiles.checked = true
            var text = "";
            if(addKeyfiles.fileUrls.length > 0)
            {
                UserSettings.setSetting("MountV-UseKeyFiles", 1)
                use_Keyfiles.checked = true;
            }
            for(var path in addKeyfiles.fileUrls) {
                listKeyfiles.push(addKeyfiles.fileUrls[path]);
            }
            for(var i in listKeyfiles) {
                text = text + listKeyfiles[i] + "; ";
            }
            keyfiles_paths.model = listKeyfiles;
        }
        onRejected: {
        }
    }


    Row {
        id: buttonsOpenVolume
        spacing: 25
        anchors.horizontalCenter: parent.horizontalCenter
        y: 10

            //password part

            UI.SecureTextField {
                id: password_value
                width: use_Keyfiles.checked ? (top.width-100)/2 : top.width-100
                horizontalAlignment: TextInput.AlignHCenter
                echoMode: TextInput.Password
                height: 40
                focus: true
                Keys.onReleased: password = password_value.text
                bordercolor: palette.darkInput
                radius_: 3
                Keys.onReturnPressed: manageWizard(1)
                Keys.onEnterPressed: manageWizard(1)
            }

            UI.CustomComboBox {
                id: keyfiles_paths
                model: []
                width: (top.width-100)/2
                height: 40
                borderWidth: 0
                visible: use_Keyfiles.checked ? true : false
            }

    }

    UI.CheckBox {
        id: display
        text_: qsTr("Display password")
        x: 50
        y: 50
        height: 40
        checked: {
            var isChecked = UserSettings.getSetting("MountV-ShowPassword")
            return (isChecked === "1") ? true : false;
        }
        onCheckedChanged: {
            if(display.checked === true) {
                UserSettings.setSetting("MountV-ShowPassword", 1)
                password_value.echoMode = TextInput.Normal;
            } else {
                UserSettings.setSetting("MountV-ShowPassword", 0)
                password_value.echoMode = TextInput.Password;
            }
        }

    }

    UI.CheckBox {
        id: use_Keyfiles
        text_: qsTr("Use keyfiles")
        x: 50
        y: display.y + 35
        height: 40
        checked: {
            var isChecked = UserSettings.getSetting("MountV-UseKeyFiles")
            return (isChecked === "1") ? true : false;
        }
        onCheckedChanged: {
            //TODO : action
            if(use_Keyfiles.checked === true) {
                UserSettings.setSetting("MountV-UseKeyFiles", 1)
            } else {
                UserSettings.setSetting("MountV-UseKeyFiles", 0)
            }
        }
    }

    UI.ButtonBordered {
        id: buttonKeyfiles
        x: buttonsOpenVolume.x + buttonsOpenVolume.width - 150
        y: 60
        height: 40
        text: qsTr("Keyfiles...")
        width: 150
        color_: palette.green
        onClicked: addKeyfiles.open()
    }

    Text {
        id:description2
        width: parent.width - 20
        font.pixelSize: 11
        text: qsTr("Please enter the password for the volume within which you wish to create a hidden volume."
                   +" <br>After you click Next, GostCrypt will attempt to mount the volume. As soon as the volume is mounted"
                   +", its cluster bitmap will be scanned to determine the size of the uninterrupted area of free space"
                   +" whose end is aligned with the end of the volume. This area will accommodate"
                   +" the hidden volume and therefore will limit its maximum possible size.") + Translation.tr
        y: use_Keyfiles.y + 45
        anchors.horizontalCenter: parent.horizontalCenter
        color: palette.text
        horizontalAlignment: Text.AlignJustify
        wrapMode: Text.WordWrap
    }
}
