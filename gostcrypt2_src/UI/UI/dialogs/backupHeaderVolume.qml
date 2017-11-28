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
            UserSettings.setSetting("Pref-AutoDismountAfter", numberOfMinutes.text)
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
                height: 150
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
                            UI.GSCheckBox {
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

                            UI.GSCheckBox {
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
                        UI.GSButtonBordered {
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
                id: blockHidden
                color: "transparent"
                border.width: 1
                border.color: palette.border
                radius: 5
                width: top.width-100
                height: 140
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: text2
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 25
                    y: 5
                    color: palette.green
                    text: qsTr("Hidden volume") + Translation.tr
                    font.pointSize: 11
                }

                Column {
                    spacing: 10
                    anchors.top: text2.bottom
                    anchors.topMargin: 10
                    anchors.horizontalCenter: parent.horizontalCenter

                    UI.GSButtonBordered {
                        id: containsHidden
                        anchors.horizontalCenter: parent.horizontalCenter
                        height: 40
                        text: qsTr("The volume contains a hidden volume") + Translation.tr
                        width: 320
                        color_: palette.green
                    }

                    Text {
                        id: text3
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: top.width - 130
                        y: 5
                        color: palette.text
                        wrapMode: Text.WordWrap
                        text: qsTr("<i>Leave blank if the volume does not contain a hidden volume or you want to save the header of the outer volume.</i>") + Translation.tr
                        font.pointSize: 10
                        horizontalAlignment: Text.AlignHCenter
                    }


                }



               /* Column {
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
                            UI.GSCheckBox {
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

                            UI.GSCheckBox {
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
                        UI.GSButtonBordered {
                            id: buttonKeyfiles
                            height: 40
                            text: qsTr("Keyfiles...") + Translation.tr
                            width: 150
                            color_: palette.green
                        }
                    }

                }*/
            }


        }

    }


    Row {
        spacing: 15
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        UI.GSButtonBordered {
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
