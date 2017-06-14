import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.4
import "../UI" as UI

Item {
    id: openVolume_Form
    property var childOf
    property string volumePath
    //modality: Qt.WindowModal
    anchors.fill: childOf
    signal mountVolume(string path, string password)
    //anchors.centerIn: childOf

    Connections {
        target: openVolume_Form
        onMountVolume: {
            ConnectSignals.connectReceiveMount(path,password);
        }

    }

    Connections {
        target: ConnectSignals
        onSendSubWindowMountVolumePasswordIncorrect: {
            password_value_style.border.color = "#e84747"
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
        }

        UI.GSButtonGreen {
            id: buttonOpen
            x: combo.x + combo.width + 15
            y: combo.y
            height: combo.height
            text: "Open..."
            width: 100
            onClicked: fileDialog.open()
        }

        GSCheckBox {
            id: historique
            text_: qsTr("Never save history")
            checked: true
            x: combo.x
            y: img.y + 60
            height: combo.height
        }

        UI.GSButtonGreen {
            id: buttonTools
            x: combo.x + combo.width - 140
            y: historique.y
            height: combo.height
            text: qsTr("Volume Tools")
            width: 120
        }

        UI.GSButtonGreen {
            id: buttonDevide
            x: buttonTools.x + buttonTools.width + 15
            y: buttonTools.y
            height: combo.height
            text: qsTr("Select Device")
            width: 120
        }

        FileDialog {
            id: fileDialog
            title: "Please choose a file"
            folder: shortcuts.home
            onAccepted: {
                console.log("You chose: " + fileDialog.fileUrl)
                openVolume_Form.moving(fileDialog.fileUrls)
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
            style: TextFieldStyle {

                textColor: "#e1e1e1"
                background: Rectangle {
                    id: password_value_style
                    radius: 5
                    implicitWidth: 100
                    implicitHeight: 24
                    border.color: "#333"
                    border.width: 1
                    color: '#181818'
                }
            }
        }

        GSCheckBox {
            id: cache
            text_: qsTr("Cache password and keyfiles in memory")
            x: combo.x
            y: password_txt.y + 40
            height: combo.height
        }

        GSCheckBox {
            id: display
            text_: qsTr("Display password")
            x: combo.x
            y: cache.y + 40
            height: combo.height
            onClicked: {
                if(display.checked == false)
                {
                    password_value.echoMode = TextInput.Password;
                }else{
                    password_value.echoMode = TextInput.Normal;
                }
            }
        }

        GSCheckBox {
            id: use_Keyfiles
            text_: qsTr("Use keyfiles")
            x: combo.x
            y: display.y + 40
            height: combo.height
        }

        UI.GSButtonGreen {
            id: buttonKeyfiles
            x: buttonDevide.x - 30
            y: cache.y
            height: combo.height
            text: qsTr("Keyfiles...")
            width: 150
        }

        UI.GSButtonGreen {
            id: buttonMountOption
            x: buttonDevide.x - 30
            y: display.y + 20
            height: combo.height
            text: qsTr("Mount Options...")
            width: 150
        }

        Behavior on opacity { NumberAnimation { duration: 500; easing.type: Easing.OutQuad; } }

    }

    UI.GSButtonBlueBordered {
        id: validation
        anchors.horizontalCenter: openVolume_Form.horizontalCenter
        anchors.bottom: openVolume_Form.bottom
        text: qsTr("Mount Volume")
        onClicked: {
            //catchClose();
            openVolume_Form.mountVolume(fileDialog.fileUrl, password_value.text);
        }
    }

    function moving(url) {
        item.anchors.topMargin = -40
        combo.model = [fileDialog.fileUrl, "C:\\volumes\\myvolume", "C:\\volumes\\old", "C:\\Users\\Administrateur\\volume"]
        volumePath = fileDialog.fileUrl
    }

    function appendPassword() {
        password.visible = true
        password.opacity = 1.0
    }

}
