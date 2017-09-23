import QtQuick 2.7
import QtQuick.Controls 1.2 as ControlsOld
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.4
import "../" as UI

Item {
    id: openVolume_Form
    property var childOf
    property string volumePath
    property variant devices
    anchors.fill: childOf
    signal incorrectPassword()
    signal sendInfoVolume()

    Connections {
        target: openVolume_Form
        onSendInfoVolume: {
            if(password_value.text.length != 0)
            {
                //mountVolume(volumePath, password_value.text);
                qmlRequest("mount", {"path": volumePath, "password": password_value.text});
                var password_blank = new Array(password_value.length+1).join('#');
                password_value.text = password_blank
                password_value.text = ""
            }else{
                app.openErrorMessage("Empty password", "Please enter a password.");
            }
        }

    }

    Connections {
        target: ConnectSignals
        onVolumePasswordIncorrect: {
            app.openErrorMessage(qsTr("Bad password"),qsTr("Incorrect password or not a GostCrypt volume."))
            password_value.style = Qt.createComponent("textFieldRed.qml");
        }
    }

    Item {
        id: item
        anchors.fill: parent
        anchors.topMargin: 40
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

        UI.GSButtonBordered {
            id: buttonOpen
            x: combo.x + combo.width + 15
            y: combo.y
            height: combo.height
            text: qsTr("Open...") + Translation.tr
            width: 100
            onClicked: fileDialog.open()
            color_: palette.green
        }

        UI.GSCheckBox {
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

        UI.GSButtonBordered {
            id: buttonTools
            x: combo.x + combo.width - 140
            y: historique.y
            height: combo.height
            text: qsTr("Volume Tools")
            width: 120
            color_: palette.green
        }

        UI.GSButtonBordered {
            id: buttonDevide
            x: buttonTools.x + buttonTools.width + 15
            y: buttonTools.y
            height: combo.height
            text: qsTr("Select Device")
            width: 120
            color_: palette.green
            onClicked: {
                devices = ConnectSignals.getListOfDevices()
                changeSubWindowTitle(qsTr("Please select a device"))
                devicesSelection.opacity = 1.0
                var i = 0
                while(devices[i] !== undefined) {
                    listDeviceModel.append(
                                {
                                    number: i,
                                    mountPoint: devices[i][0],
                                    name: devices[i][1],
                                    path: devices[i][2],
                                    removable: devices[i][3],
                                    size: devices[i][4],
                                    systemNumber: devices[i][5]
                                })
                    i++;
                }
            }
        }

        FileDialog {
            id: fileDialog
            title: qsTr("Please choose a file") + Translation.tr
            folder: shortcuts.home
            onAccepted: {
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
        y: buttonTools.y + buttonTools.height + 30
        anchors.topMargin: 00
        opacity: 0.0

        Text {
            id: password_txt
            y: 10 //TODO lineheight
            width: 95
            leftPadding: 20
            text: qsTr("Password: ")  + Translation.tr
            font.pointSize: 11
            lineHeightMode: Text.FixedHeight
            horizontalAlignment: Text.AlignRight
            lineHeight: combo.height
            color: '#719c24'
        }

        ControlsOld.TextField {
            id: password_value
            x: combo.x
            width: combo.width + 118
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

        UI.GSCheckBox {
            id: cache
            text_: qsTr("Cache password and keyfiles in memory")  + Translation.tr
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

        UI.GSCheckBox {
            id: display
            text_: qsTr("Display password")  + Translation.tr
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

        UI.GSCheckBox {
            id: use_Keyfiles
            text_: qsTr("Use keyfiles") + Translation.tr
            x: combo.x
            y: display.y + 40
            height: combo.height
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
            x: buttonDevide.x - 30
            y: cache.y
            height: combo.height
            text: qsTr("Keyfiles...") + Translation.tr
            width: 150
            color_: palette.green
        }

        UI.GSButtonBordered {
            id: buttonMountOption
            x: buttonDevide.x - 30
            y: display.y + 20
            height: combo.height
            text: qsTr("Mount Options...") + Translation.tr
            width: 150
            color_: palette.green
        }
        Behavior on opacity { NumberAnimation { duration: 500; easing.type: Easing.OutQuad; } }
    }

    UI.GSButtonBordered {
        id: validation
        anchors.horizontalCenter: openVolume_Form.horizontalCenter
        anchors.bottom: openVolume_Form.bottom
        text: qsTr("Mount Volume") + Translation.tr
        color_: palette.blue
        onClicked: {
            if(sudo_.isVisible === false)
            {
                sendInfoVolume()
                var password_blank = new Array(password_value.length+1).join('#');
                password_value.text = password_blank
            }
        }
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
                                text: '<b>Removable:</b> ' + removable
                            }
                        }
                        /*Item {
                            x: 70
                            y: 5
                            width: parent.width -10
                            height: parent.height -10
                            Column {
                                Text { color:palette.text; text: '<b>MountPoint:</b> ' + mountPoint }
                                Text { color:palette.text; text: '<b>Name:</b> ' + name }

                                Text { color:palette.text; text: '<b>Removable:</b> ' + removable }

                                Text { color:palette.text; text: '<b>SystemNumber:</b> ' + systemNumber }
                            }
                        }*/
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
        item.anchors.topMargin = 20
        volumePath = url
    }

    function appendPassword() {
        password.visible = true
        password.opacity = 1.0
    }

    function initDrag(parameter) {
        moving(parameter.value)
        UserSettings.addVolumePath(parameter.value)
        combo.model = UserSettings.getVolumePaths(0)
    }

}
