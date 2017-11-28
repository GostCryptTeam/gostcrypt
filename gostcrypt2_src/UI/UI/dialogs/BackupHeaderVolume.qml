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
                        onClicked: {
                            if(password_value.text !== "")
                            {
                                password_value.readOnly = true
                                hiddenVolumePassword.visible = true
                                containsHidden.visible = false
                                text3.visible = false
                                removeHidden.visible = true
                            }
                            else
                            {
                                openErrorMessage(qsTr("Missing password"), qsTr("Please enter a password for the outer volume first."))
                            }
                        }
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


                    Column {
                        id: hiddenVolumePassword
                        visible: false
                        spacing: 5
                        Row {
                            spacing: 15
                            anchors.horizontalCenter: parent.horizontalCenter
                            Text {
                                id: password_txt_hidden
                                text: qsTr("Password: ")  + Translation.tr
                                font.pointSize: 11
                                lineHeightMode: Text.FixedHeight
                                horizontalAlignment: Text.AlignRight
                                verticalAlignment: Text.AlignVCenter
                                lineHeight: 40
                                color: palette.text
                            }

                            UI.SecureTextField {
                                id: password_value_hidden
                                width: 250
                                horizontalAlignment: TextInput.AlignHCenter
                                height: 40
                            }
                        }
                        Row {
                            spacing: 20
                            Column {
                                spacing: 5
                                UI.GSCheckBox {
                                    id: displayHidden
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
                                    id: use_Keyfiles_hidden
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
                                id: buttonKeyfiles_hidden
                                height: 40
                                text: qsTr("Keyfiles...") + Translation.tr
                                width: 150
                                color_: palette.green
                            }
                        }
                    }


                }

                UI.GSButtonBordered {
                    id: removeHidden
                    visible: false
                    anchors.top: blockHidden.top
                    anchors.right: blockHidden.right
                    anchors.topMargin: 5
                    anchors.rightMargin: 5
                    height: 40
                    text: "×"
                    width: 40
                    color_: palette.blue
                    onClicked: {
                        password_value.readOnly = false
                        hiddenVolumePassword.visible = false
                        containsHidden.visible = true
                        text3.visible = true
                        removeHidden.visible = false
                    }
                }
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
            text: qsTr("Backup header") + Translation.tr
            width: 120
            onClicked: {
                if(password_value.text != "" && ((hiddenVolumePassword.visible && password_value_hidden.text != "") || !hiddenVolumePassword.visible))
                {
                    openErrorMessage(qsTr("Are you sure you want to create volume header backup ?"),
                                     qsTr("Are you sure you want to create volume header backup for ")+volumePath+
                                     qsTr("?<br><br>After you click Yes, you will prompted for a filename for the header backup."+
                                          "<br><br>Note: Both the standard and the hidden volume headers will be re-encrypted using a new"+
                                          " salt and stored in the backup file. If there is no hidden volume within this volume, the area reserved"+
                                          " for the hidden volume header in the backup file will be filled with random data (to preserve plausible deniability"+
                                          "). When restoring a volume header from the backup file, you will need to enter the correct password (and/or to"+
                                          " supply the correct keyfiles) that was/were valid when the volume header backup was created. The password (and/or keyfiles) will also"+
                                          " automatically determine the type of the volume header to restore i.e. standard or hidden (note that GostCrypt determines that the type"+
                                          " through the process of trial and error)."), undefined, true, "backupheadervolume", {volume: volumePath})
                }
                catchClose();

            }
            color_: palette.blue
            anchors.bottomMargin: 10
        }
    }

}
