import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import "../" as UI

Item {
    id: top
    anchors.centerIn: parent
    property variant password: ["", ""]
    property int type: 0
    property string text_
    property variant listKeyfiles: []
    property int isFile: UserSettings.getSetting("MountV-UseKeyFiles")

    FileDialog {
        id: addKeyfiles
        title: qsTr("Please choose a keyfile") + Translation.tr
        folder: shortcuts.home
        selectMultiple: true
        onAccepted: {
            var text = "";
            if(addKeyfiles.fileUrls.length > 0) use_Keyfiles.checked = true;
            for(var path in addKeyfiles.fileUrls) {
                listKeyfiles.push(addKeyfiles.fileUrls[path]);
            }
            for(var i in listKeyfiles) {
                text = text + listKeyfiles[i] + "; ";
            }
            combo.model = listKeyfiles;
        }
        onRejected: {
        }
    }

    Text {
        id:titre
        font.pointSize: 13
        font.family: "Helvetica"
        text: qsTr("Please enter a password twice or use a Keyfile:") + Translation.tr
        anchors.horizontalCenter: parent.horizontalCenter
        color: palette.text
        wrapMode: Text.WordWrap
    }

    Row {
        width: parent.width - 50
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10
        anchors.top: titre.bottom
        anchors.topMargin: 20

        Item {
            id: modePassword
            width: (isFile === 1) ? 260 : parent.width
            height: 150
            UI.SecureTextField {
                id: password_value
                width: (isFile === 1) ? top.width*0.5 : top.width
                horizontalAlignment: TextInput.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
                echoMode: TextInput.Password
                height: 40
                focus: true
                Keys.onReleased: password[0] = password_value.text
                bordercolor: palette.darkInput
                radius_: 3
            }

            UI.SecureTextField {
                id: password_value2
                anchors.top: password_value.bottom
                anchors.topMargin: 10
                width: (isFile === 1) ? top.width*0.5 : top.width
                horizontalAlignment: TextInput.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
                echoMode: TextInput.Password
                height: 40
                focus: true
                bordercolor: palette.darkInput
                radius_: 3
                Keys.onReleased: password[1] = password_value2.text
                Keys.onReturnPressed: manageWizard(1)
                Keys.onEnterPressed: manageWizard(1)
            }
        }

        Item {
            id: modeFile
            width: 260
            height: 150
            visible: (isFile === 1) ? true : false
            UI.ButtonBordered {
                id: buttonKeyfiles
                anchors.horizontalCenter: parent.horizontalCenter
                height: 40
                text: qsTr("Open a Keyfile...")
                width: 150
                color_: palette.green
                onClicked: addKeyfiles.open()
            }
            UI.CustomComboBox {
                id: combo
                width: 250
                anchors.top: buttonKeyfiles.bottom
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter
                model: []
            }

        }
    }

    UI.CheckBox {
        id: display
        text_: qsTr("Display password")
        y: 150
        anchors.horizontalCenter: parent.horizontalCenter
        height: 20
        size_: 20
        checked: {
            var isChecked = UserSettings.getSetting("MountV-ShowPassword")
            return (isChecked === "1") ? true : false;
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

    UI.CheckBox {
        id: use_Keyfiles
        text_: qsTr("Use a File Key...") + Translation.tr
        anchors.top: display.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        height: 20
        size_: 20
        checked: (isFile === 1) ? true : false
        onCheckedChanged: {
            if(use_Keyfiles.checked == true) {
                isFile = true
                UserSettings.setSetting("MountV-UseKeyFiles", 1)
            } else {
                isFile = false
                UserSettings.setSetting("MountV-UseKeyFiles", 0)
            }
        }
    }

    UI.HelpButton {
        id:description
        size: 30
        anchors.left: titre.right
        anchors.leftMargin: 10
        y: titre.y
        onClicked: {
            openErrorMessage("Information", text_)
        }
    }
}
