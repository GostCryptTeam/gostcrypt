import QtQuick 2.7
import QtQuick.Controls 2.0
import "../" as UI

Item {
    id: top;
    property string volumePath: ""
    property int mode: 0 //0 : change password
    signal update()

    Connections {
        target: top
        onUpdate: {

            catchClose();
        }
    }


    Flickable {
        id: flickable
        y: 15
        anchors.horizontalCenter: parent.horizontalCenter
        height: 280
        width: top.width-98
        clip: true
        contentWidth: content.width
        contentHeight: content.height
        flickableDirection: Flickable.VerticalFlick
        interactive: true
        boundsBehavior: Flickable.StopAtBounds

        ScrollBar.vertical: ScrollBar {
                parent: flickable.parent
                anchors.top: flickable.top
                anchors.left: flickable.right
                anchors.bottom: flickable.bottom
                active: true
            }
        Column {
            id: content
            spacing: 15
            Rectangle {
                id: block
                color: "transparent"
                border.width: 1
                border.color: palette.border
                radius: 5
                width: top.width-100
                height: 160
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: text
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 25
                    y: 5
                    color: palette.green
                    text: qsTr("Current Volume password") + Translation.tr
                    font.pointSize: 11
                }
                Column {
                    anchors.top: text.bottom
                    anchors.topMargin: 5
                    spacing: 5
                    anchors.horizontalCenter: parent.horizontalCenter
                    Row {
                        spacing: 15
                        anchors.horizontalCenter: parent.horizontalCenter
                        Text {
                            id: password_txt
                            text: qsTr("Password: ")  + Translation.tr
                            font.pointSize: 11
                            lineHeightMode: Text.FixedHeight
                            horizontalAlignment: Text.AlignRight
                            verticalAlignment: Text.AlignVCenter
                            lineHeight: 40
                            color: palette.text
                        }

                        UI.SecureTextField {
                            id: password_value
                            width: 250
                            horizontalAlignment: TextInput.AlignHCenter
                            height: 40
                            onValidated: {
                                sendInfoVolume()
                                var password_blank = new Array(password_value.length+1).join('#');
                                password_value.text = password_blank
                            }
                        }
                    }
                    Row {
                        spacing: 20
                        Column {
                            spacing: 5
                            UI.CheckBox {
                                id: display
                                text_: qsTr("Display password")  + Translation.tr
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

                            UI.CheckBox {
                                id: use_Keyfiles
                                text_: qsTr("Use keyfiles") + Translation.tr
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
                        }
                        UI.ButtonBordered {
                            id: buttonKeyfiles
                            height: 40
                            text: qsTr("Keyfiles...") + Translation.tr
                            width: 150
                            color_: palette.green
                        }
                    }

                }
            }
            Rectangle {
                id: block2
                color: "transparent"
                border.width: 1
                border.color: palette.border
                radius: 5
                width: top.width-100
                height: 230
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: text2
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 25
                    y: 5
                    color: palette.green
                    text: qsTr("New Volume password") + Translation.tr
                    font.pointSize: 11
                }
                Column {
                    id: col
                    anchors.top: text2.bottom
                    anchors.topMargin: 5
                    spacing: 5
                    anchors.horizontalCenter: parent.horizontalCenter
                    Row {
                        spacing: 15
                        anchors.horizontalCenter: parent.horizontalCenter
                        Text {
                            id: password_txt2
                            text: qsTr("New Password: ")  + Translation.tr
                            font.pointSize: 11
                            lineHeightMode: Text.FixedHeight
                            horizontalAlignment: Text.AlignRight
                            verticalAlignment: Text.AlignVCenter
                            lineHeight: 40
                            color: palette.text
                            width: 200
                        }

                        UI.SecureTextField {
                            id: password_value1
                            width: 250
                            horizontalAlignment: TextInput.AlignHCenter
                            height: 40
                            onValidated: {
                                sendInfoVolume()
                                var password_blank = new Array(password_value.length+1).join('#');
                                password_value.text = password_blank
                            }
                        }
                    }
                    Row {
                        spacing: 15
                        anchors.horizontalCenter: parent.horizontalCenter
                        Text {
                            id: password_txt1
                            text: qsTr("Confirm password: ")  + Translation.tr
                            font.pointSize: 11
                            lineHeightMode: Text.FixedHeight
                            horizontalAlignment: Text.AlignRight
                            verticalAlignment: Text.AlignVCenter
                            lineHeight: 40
                            color: palette.text
                            width: 200
                        }

                        UI.SecureTextField {
                            id: password_value2
                            width: 250
                            horizontalAlignment: TextInput.AlignHCenter
                            height: 40
                            onValidated: {
                                sendInfoVolume()
                                var password_blank = new Array(password_value.length+1).join('#');
                                password_value.text = password_blank
                            }
                        }
                    }
                    Row {
                        spacing: 20
                        Column {
                            spacing: 5
                            UI.CheckBox {
                                id: display2
                                text_: qsTr("Display password")  + Translation.tr
                                height: 20
                                size_: 20
                                sizeText: 10
                                width: 200
                                checked: {
                                    var isChecked = UserSettings.getSetting("MountV-ShowPassword")
                                    return (isChecked === "1") ? true : false;
                                }
                                onCheckedChanged: {
                                    if(display2.checked == true) {
                                        UserSettings.setSetting("MountV-ShowPassword", 1)
                                        password_value1.echoMode = TextInput.Normal;
                                        password_value2.echoMode = TextInput.Normal;
                                    } else {
                                        UserSettings.setSetting("MountV-ShowPassword", 0)
                                        password_value1.echoMode = TextInput.Password;
                                        password_value2.echoMode = TextInput.Password;
                                    }
                                }

                            }

                            UI.CheckBox {
                                id: use_Keyfiles2
                                text_: qsTr("Use keyfiles") + Translation.tr
                                height: 20
                                size_: 20
                                sizeText: 10
                                width: 200
                                checked: {
                                    var isChecked = UserSettings.getSetting("MountV-UseKeyFiles")
                                    return (isChecked === "1") ? true : false;
                                }
                                onCheckedChanged: {
                                    //TODO : action
                                    if(use_Keyfiles2.checked == true) {
                                        UserSettings.setSetting("MountV-UseKeyFiles", 1)
                                    } else {
                                        UserSettings.setSetting("MountV-UseKeyFiles", 0)
                                    }
                                }
                            }
                        }
                        UI.ButtonBordered {
                            id: buttonKeyfiles2
                            height: 40
                            text: qsTr("Keyfiles...") + Translation.tr
                            width: 150
                            color_: palette.green
                        }
                    }
                    Row {
                        spacing: 20
                        Text {
                            id: pkcs
                            text: qsTr("PKCS-5 PRF: ")  + Translation.tr
                            font.pointSize: 11
                            lineHeightMode: Text.FixedHeight
                            horizontalAlignment: Text.AlignRight
                            verticalAlignment: Text.AlignVCenter
                            lineHeight: 40
                            color: palette.text
                            width: 200
                        }
                        UI.CustomComboBox {
                            id: combo
                            width: 250
                            height: 40
                            model: ["Unchanged", "HMAC-GOST R 34.11-2012","HMAC-GOST R 31.11-94", "HMAC-Whirlpool"]
                            onActivated: {
                                if(currentText != "")
                                    openVolume_Form.moving(currentText)
                            }
                        }
                    }


                }
            }
        }

    }


    Row {
        spacing: 15
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        UI.ButtonBordered {
            id: benchmark
            height: 40
            text: qsTr("Save changes") + Translation.tr
            width: 120
            onClicked: top.update()
            color_: palette.blue
            anchors.bottomMargin: 10
        }
    }

}
