import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.4
import "../" as UI

import gostcrypt.ui.secureInput 1.0

Item {
    id: openVolume_Form

    property url volumePath
    property variant devices
    signal incorrectPassword()
    signal sendInfoVolume()

    Connections {
        target: openVolume_Form
        onSendInfoVolume: {
            if(password_value.text.length != 0)
            {
                qmlRequest("mount", {"path": volumePath, "password": password_value.text, "name": qsTr("Mount volume"), "desc": volumePath});
                var password_blank = new Array(password_value.length+1).join('#');
                password_value.text = password_blank
                password_value.text = "" //TODO: stock password in C++
            }else{
                app.openErrorMessage("Empty password", "Please enter a password.");
            }
        }
    }

    Connections {
        target: ConnectSignals
        onVolumePasswordIncorrect: {
            password_value.style = Qt.createComponent("textFieldRed.qml");
        }
    }

    Item {
        id: item
        height: 170
        anchors.centerIn: parent

        Text {
            id: textTop
            text: qsTr("Please open a GostCrypt volume :") + Translation.tr
            font.pointSize: 11
            color: palette.textLight
            anchors.horizontalCenter: parent.horizontalCenter

            Behavior on opacity { NumberAnimation { id: animTextTop; duration: app.duration; easing.type: Easing.OutQuad; } }
        }

        UI.GSCustomComboBox {
            id: combo
            anchors.horizontalCenter: parent.horizontalCenter
            width: openVolume_Form.width - 250
            y: textTop.y + textTop.height + 10
            height: 40
            model: {
                var paths = UserSettings.getVolumePaths(1)
                return paths;
            }
            onActivated: {
                if(currentText != "")
                    openVolume_Form.moving(currentText)
            }
        }

        UI.GSCheckBox {
            id: historique
            text_: qsTr("Never save history")
            y: combo.y + combo.height + 5
            height: combo.height
            anchors.horizontalCenter: parent.horizontalCenter
            size_: 20
            checked: {
                var isChecked = UserSettings.getSetting("MountV-SaveHistory")
                return (isChecked === "1") ? true : false;
            }
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

        Row {
            id: buttonsOpenVolume
            spacing: 25
            anchors.horizontalCenter: parent.horizontalCenter
            y: historique.y + historique.height + 5

            UI.GSButtonBordered {
                id: buttonOpen
                height: combo.height
                text: qsTr("Open...") + Translation.tr
                width: 120
                onClicked: fileDialog.open()
                color_: palette.green
            }

            UI.GSButtonBordered {
                id: buttonDevide
                height: combo.height
                text: qsTr("Select Device")
                width: 120
                color_: palette.green
                onClicked: {
                    qmlRequest("devices", "")
                    changeSubWindowTitle(qsTr("Please select a device"))
                    devicesSelection.opacity = 1.0
                }
            }
        }

        FileDialog {
            id: fileDialog
            title: qsTr("Please choose a file") + Translation.tr
            folder: shortcuts.home
            onAccepted: {
                if(historique.pressed === false)
                    UserSettings.addVolumePath(fileDialog.fileUrl)
                openVolume_Form.moving(fileDialog.fileUrl)
                combo.model = UserSettings.getVolumePaths(0)
            }
            onRejected: {
            }
        }

        Behavior on y {
            NumberAnimation {
                id: anim;
                duration: app.duration;
                easing.type: Easing.OutQuad;
                onRunningChanged: {
                    if (!anim.running) {
                        appendPassword();
                    }
                }
            }
        }
    }

    Item {
        id: password
        //visible: false
        y: item.y + item.height
        anchors.horizontalCenter: parent.horizontalCenter
        opacity: 0.0

        Text {
            id: password_txt
            y: 0
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Password: ")  + Translation.tr
            font.pointSize: 11
            lineHeightMode: Text.FixedHeight
            horizontalAlignment: Text.AlignRight
            lineHeight: 15
            color: '#719c24'
        }

        UI.SecureTextField {
            id: password_value
            y: password_txt.y + password_txt.height + 10
            anchors.horizontalCenter: parent.horizontalCenter
            width: combo.width
            horizontalAlignment: TextInput.AlignHCenter
            height: combo.height
            onValidated: {
                sendInfoVolume()
                var password_blank = new Array(password_value.length+1).join('#');
                password_value.text = password_blank
            }
        }

        UI.GSCheckBox {
            id: cache
            text_: qsTr("Cache password and keyfiles in memory")  + Translation.tr
            x: combo.x
            y: password_value.y + password_value.height + 10
            height: 20
            width: 300
            size_: 20
            sizeText: 10
            checked: {
                var isChecked = UserSettings.getSetting("MountV-CachePwd")
                return (isChecked === "1") ? true : false;
            }
            onCheckedChanged: {
                if(cache.checked == true)
                    UserSettings.setSetting("MountV-SaveHistory", 1)
                else
                    UserSettings.setSetting("MountV-SaveHistory", 0)
            }
        }

        UI.GSCheckBox {
            id: display
            text_: qsTr("Display password")  + Translation.tr
            x: combo.x
            anchors.top: cache.bottom
            anchors.topMargin: 5
            height: 20
            size_: 20
            sizeText: 10
            checked: {
                var isChecked = UserSettings.getSetting("MountV-ShowPassword")
                return (isChecked === "1") ? true : false;
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
            text_: qsTr("Use keyfiles") + Translation.tr
            x: combo.x
            anchors.top: display.bottom
            anchors.topMargin: 5
            height: 20
            size_: 20
            sizeText: 10
            checked: {
                var isChecked = UserSettings.getSetting("MountV-UseKeyFiles")
                return (isChecked === "1") ? true : false;
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
            anchors.right: password_value.right
            y: cache.y
            height: 30
            text: qsTr("Keyfiles...") + Translation.tr
            width: 150
            color_: palette.green
        }

        UI.GSButtonBordered {
            id: buttonMountOption
            anchors.right: password_value.right
            y: buttonKeyfiles.y + buttonKeyfiles.height + 5
            height: 30
            text: qsTr("Mount Options...") + Translation.tr
            width: 150
            color_: palette.green
        }
        Behavior on opacity {
            NumberAnimation {
                id: anim2
                duration: 500;
                easing.type: Easing.OutQuad;
                onRunningChanged: {
                    if(!anim2.running) {
                        password_value.focus = true;
                    }
                }
            }
        }
    }

    UI.GSButtonBordered {
        id: validation
        anchors.horizontalCenter: openVolume_Form.horizontalCenter
        anchors.bottom: openVolume_Form.bottom
        text: qsTr("Mount Volume") + Translation.tr
        opacity: 0.0
        color_: palette.blue
        onClicked: {
            if(sudo_.isVisible === false)
            {
                sendInfoVolume()
                var password_blank = new Array(password_value.length+1).join('#');
                password_value.text = password_blank
            }
        }
        Behavior on opacity { NumberAnimation { duration: 500; easing.type: Easing.OutQuad; } }
    }

    // Device part
    Rectangle {
        id: devicesSelection
        visible: false
        opacity: 0.0
        anchors.fill:parent
        color: palette.darkSecond
        Behavior on opacity { NumberAnimation { id: anim_; duration: app.duration/2; easing.type: Easing.OutQuad; onRunningChanged: {
                    if(!anim_.running) {
                        devicesSelection.visible = !devicesSelection.visible
                    }
                } } }

        Component {
                id: deviceDelegate
                Item {
                    id: elementDevice
                    width: 320;
                    height: 70;
                    anchors.horizontalCenter: parent.horizontalCenter
                    Behavior on height { NumberAnimation { duration: app.duration/3; easing.type: Easing.OutQuad; } }
                    Behavior on width { NumberAnimation { duration: app.duration/3; easing.type: Easing.OutQuad; } }
                    Rectangle {
                        id: contentDevice
                        anchors.fill: parent
                        color: '#3b3b3b'
                        border.color: palette.darkThird
                        border.width: 1
                        radius: 2

                        //Circle (number)
                        Rectangle {
                            anchors.verticalCenter: parent.verticalCenter
                            width: 50
                            height: 50
                            radius: 50
                            color: palette.darkInput
                            x:10
                            Text {
                                color: palette.text
                                text: number
                                anchors.centerIn: parent
                                font.pixelSize: 15
                            }
                        }
                        Column {
                            id: infos
                            x: 70
                            y: 8
                            Text {
                                id: infos_Path
                                font.pixelSize: 18
                                color:palette.text;
                                text: '<b>Path:</b> ' + path
                            }
                            Text {
                                font.pixelSize: 11
                                color:palette.text;
                                text: '<b>Size:</b> ' + size
                            }
                            Text {
                                font.pixelSize: 11
                                color:palette.text;
                                text: '<b>Mount point:</b> ' + mountPoint
                            }
                        }

                        MouseArea {
                            id:area
                            anchors.fill:parent
                            hoverEnabled: true
                            cursorShape: area.containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
                            onEntered: {
                                contentDevice.color = "#454545"
                                elementDevice.width = 400
                               // elementDevice.height = 87
                            }
                            onExited: {
                                contentDevice.color = "#3B3B3B"
                                elementDevice.width = 320
                               // elementDevice.height = 70
                            }
                            onClicked: {
                                openVolume_Form.moving(path)
                                if(historique.pressed === false)
                                    UserSettings.addVolumePath(path)
                                combo.model = UserSettings.getVolumePaths(0)
                                devicesSelection.opacity = 0.0
                                changeSubWindowTitle(qsTr("Open a GostCrypt Volume"))
                            }
                        }
                    }
                }
            }

        ListModel {
             id: listDeviceModel
        }

        ListView {
            id: listOfDevices
            anchors.fill: parent
            delegate: deviceDelegate
            model: listDeviceModel
            focus: true
            ScrollBar.vertical: ScrollBar { snapMode: ScrollBar.SnapOnRelease }
            snapMode: GridView.SnapToRow
            clip: true
            spacing:10
            anchors.topMargin: 10
            anchors.bottomMargin: 10
        }
    }

    function moving(url) {
        password_value.focus = true;
        textTop.opacity = 0.0
        combo.y = 15
        item.anchors.centerIn = undefined;
        item.y = 0
        item.height = 155
        volumePath = url
    }

    function appendPassword() {
        //password.visible = true
        password.opacity = 1.0
        validation.opacity = 1.0
    }

    function initDrag(parameter) {
        moving(parameter.value)
        UserSettings.addVolumePath(parameter.value)
        combo.model = UserSettings.getVolumePaths(0)
    }

    function addHostDevice(device)
    {
        listDeviceModel.append(device)
    }

}
