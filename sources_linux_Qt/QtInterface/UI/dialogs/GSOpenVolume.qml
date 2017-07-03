import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.4
import "../" as UI

Item {
    id: openVolume_Form
    property var childOf
    property string volumePath
    anchors.fill: childOf
    signal incorrectPassword()
    signal sendInfoVolume()

    Connections {
        target: openVolume_Form
        onSendInfoVolume: {
            if(password_value.text.length != 0)
            {
                mountVolume(volumePath, password_value.text);
                var password_blank = Array(password_value.length+1).join('#');
                console.log("Mot de passe : "+password_blank);
                password_value.text = password_blank
                password_value.text = ""
            }else{
                app.openErrorMessage("Empty password", "Please enter a password.");
            }
        }

    }

    Connections {
        target: ConnectSignals
        onSendSubWindowMountVolumePasswordIncorrect: {
            console.log("bad password dans QML")
            app.openErrorMessage(qsTr("Bad password"),qsTr("Incorrect password or not a GostCrypt volume."))
            password_value.style = Qt.createComponent("textFieldRed.qml");
        }
    }

    Item {
        id: item
        anchors.fill: parent
        anchors.topMargin: 0
        Image {
            id: img
            x: 50
            asynchronous: true
            source: "../ressource/logo_gostcrypt.png"
        }

        UI.GSCustomComboBox {
            id: combo
            x: 100
            y: 10
            width: parent.width - 250
            model: {
                var paths = UserSettings.getVolumePaths(1)
                return paths;
            }
            onActivated: {
                if(currentText != "")
                    openVolume_Form.moving(currentText)
            }
        }

        UI.GSButton {
            id: buttonOpen
            x: combo.x + combo.width + 15
            y: combo.y
            height: combo.height
            text: "Open..."
            width: 100
            onClicked: fileDialog.open()
            color_: palette.green
            colorHover_: palette.green
            colorPress_: palette.greenDark
        }

        GSCheckBox {
            id: historique
            text_: qsTr("Never save history")
            checked: {
                var isChecked = UserSettings.getSetting("MountV-SaveHistory")
                return (isChecked == 1) ? true : false;
            }
            x: combo.x
            y: img.y + 60
            height: combo.height
            onCheckedChanged: {
                if(historique.checked == true)
                    UserSettings.setSetting("MountV-SaveHistory", 1)
                else
                    UserSettings.setSetting("MountV-SaveHistory", 0)
                if(historique.checked === true) {
                    UserSettings.erasePaths()
                    UserSettings.getVolumePaths(1)
                }
            }
        }

        UI.GSButton {
            id: buttonTools
            x: combo.x + combo.width - 140
            y: historique.y
            height: combo.height
            text: qsTr("Volume Tools")
            width: 120
            color_: palette.green
            colorHover_: palette.greenHover
            colorPress_: palette.greenDark
        }

        UI.GSButton {
            id: buttonDevide
            x: buttonTools.x + buttonTools.width + 15
            y: buttonTools.y
            height: combo.height
            text: qsTr("Select Device")
            width: 120
            color_: palette.green
            colorHover_: palette.greenHover
            colorPress_: palette.greenDark
        }

        FileDialog {
            id: fileDialog
            title: "Please choose a file"
            folder: shortcuts.home
            onAccepted: {
                console.log("You chose: " + fileDialog.fileUrl)
                openVolume_Form.moving(fileDialog.fileUrl)
                if(historique.pressed === false)
                    UserSettings.addVolumePath(fileDialog.fileUrl)
                combo.model = UserSettings.getVolumePaths(0)
            }
            onRejected: {
                console.log("Canceled")
            }
        }
        Behavior on anchors.topMargin { NumberAnimation { id: anim; duration: app.duration; easing.type: Easing.OutQuad; onRunningChanged: {if (!anim.running) { appendPassword(); } } } }
    }

    Item {
        id: password
        visible: false
        y: buttonTools.y + buttonTools.height - 15
        anchors.topMargin: -20
        opacity: 0.0

        Text {
            id: password_txt
            y: 10 //TODO lineheight
            leftPadding: 15
            text: "Password: "
            font.pointSize: 11
            lineHeightMode: Text.FixedHeight
            lineHeight: combo.height
            color: '#719c24'
        }

        TextField {
            id: password_value
            x: password_txt.x + password_txt.width + 15
            width: combo.width + 115
            horizontalAlignment: TextInput.AlignHCenter
            echoMode: TextInput.Password
            height: combo.height
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
        }

        GSCheckBox {
            id: cache
            text_: qsTr("Cache password and keyfiles in memory")
            x: combo.x
            y: password_txt.y + 40
            height: combo.height
            checked: {
                var isChecked = UserSettings.getSetting("MountV-CachePwd")
                return (isChecked == 1) ? true : false;
            }
            onCheckedChanged: {
                if(cache.checked == true)
                    UserSettings.setSetting("MountV-SaveHistory", 1)
                else
                    UserSettings.setSetting("MountV-SaveHistory", 0)
                //TODO : action
            }
        }

        GSCheckBox {
            id: display
            text_: qsTr("Display password")
            x: combo.x
            y: cache.y + 40
            height: combo.height
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

        GSCheckBox {
            id: use_Keyfiles
            text_: qsTr("Use keyfiles")
            x: combo.x
            y: display.y + 40
            height: combo.height
            checked: {
                var isChecked = UserSettings.getSetting("MountV-UseKeyFiles")
                return (isChecked == 1) ? true : false;
            }
            onCheckedChanged: {
                //TODO : action
                if(display.checked == true) {
                    UserSettings.setSetting("MountV-UseKeyFiles", 1)
                } else {
                    UserSettings.setSetting("MountV-UseKeyFiles", 0)
                }
            }
        }

        UI.GSButton {
            id: buttonKeyfiles
            x: buttonDevide.x - 30
            y: cache.y
            height: combo.height
            text: qsTr("Keyfiles...")
            width: 150
            color_: palette.green
            colorHover_: palette.greenHover
            colorPress_: palette.greenDark
        }

        UI.GSButton {
            id: buttonMountOption
            x: buttonDevide.x - 30
            y: display.y + 20
            height: combo.height
            text: qsTr("Mount Options...")
            width: 150
            color_: palette.green
            colorHover_: palette.greenHover
            colorPress_: palette.greenDark
        }
        Behavior on opacity { NumberAnimation { duration: 500; easing.type: Easing.OutQuad; } }
    }

    UI.GSButtonBordered {
        id: validation
        anchors.horizontalCenter: openVolume_Form.horizontalCenter
        anchors.bottom: openVolume_Form.bottom
        text: qsTr("Mount Volume")
        color_: palette.blue
        onClicked: {
            if(sudo_.isVisible === false)
            {
                sendInfoVolume()
            }
        }
    }

    function moving(url) {
        item.anchors.topMargin = -40
        volumePath = url
    }

    function appendPassword() {
        password.visible = true
        password.opacity = 1.0
    }

    function initDrag(parameter) {
        moving(parameter.value)
        UserSettings.addVolumePath(parameter.value)
        //console.log(parameter.value);
        combo.model = UserSettings.getVolumePaths(0)
    }

}
