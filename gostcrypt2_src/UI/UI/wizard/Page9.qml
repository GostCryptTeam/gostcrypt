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
    property int isFile: {
        var isChecked = UserSettings.getSetting("MountV-UseKeyFiles")
        return isChecked
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
    Item {
        id: modeFile
        visible: (isFile === 1) ? true : false
        anchors.top: titre.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        UI.GSButtonBordered {
            id: buttonKeyfiles
            anchors.horizontalCenter: parent.horizontalCenter
            height: 40
            text: qsTr("Open a Keyfile...")
            width: 150
            color_: palette.green
            onClicked: fileDialog.open()
        }
        UI.GSCustomComboBox {
            id: combo
            width: 250
            anchors.top: buttonKeyfiles.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            model: [""]
        }

    }

    Item {
        id: modePassword
        visible: (isFile === 1) ? false : true
        anchors.top: titre.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        TextField {
            id: password_value
            width: top.width*0.5
            horizontalAlignment: TextInput.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
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
            anchors.top: password_value.bottom
            anchors.topMargin: 5
            width: top.width*0.5
            horizontalAlignment: TextInput.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
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
            Keys.onReturnPressed: manageWizard(1)
            Keys.onEnterPressed: manageWizard(1)
        }

    }

    UI.GSCheckBox {
        id: display
        text_: qsTr("Display password")
        y: 150
        anchors.horizontalCenter: parent.horizontalCenter
        height: 20
        size_: 20
        checked: {
            var isChecked = UserSettings.getSetting("MountV-ShowPassword")
            return (isChecked === 1) ? true : false;
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
        text_: qsTr("Use a File Key...") + Translation.tr
        anchors.top: display.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        height: 20
        size_: 20
        checked: (isFile === 1) ? true : false
        onCheckedChanged: {
            //TODO : action
            if(use_Keyfiles.checked == true) {
                UserSettings.setSetting("MountV-UseKeyFiles", 1)
                modeFile.visible = true
                modePassword.visible = false
            } else {
                UserSettings.setSetting("MountV-UseKeyFiles", 0)
                modeFile.visible = false
                modePassword.visible = true
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Please choose a file") + Translation.tr
        folder: shortcuts.home
        selectExisting: {
            if(type !== 2)
                return false
            else
                return true
        }
        onAccepted: {
            combo.model = [fileDialog.fileUrl]
        }
        onRejected: {
        }
    }


    UI.GSHelpButton {
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
