import QtQuick 2.7
import QtQuick.Controls 2.0 as CONTROLS
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "../" as UI

Item {
    id: top;
    property string volumePath: ""
    property int mode: 0

    signal update()

    FileDialog {
        id: fileDialog
        title: qsTr("Please choose a file") + Translation.tr
        folder: shortcuts.home
        onAccepted: {
            qmlRequest("restoreHeaderVolume", {volume: fileDialog.fileUrl})
            catchClose();
        }
        onRejected: {
        }
    }

    Connections {
        target: top
        onUpdate: {
            fileDialog.open()
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

        CONTROLS.ScrollBar.vertical: CONTROLS.ScrollBar {
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
                height: 100
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: text
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 25
                    y: 5
                    color: palette.green
                    text: qsTr("Please select the type of volume header backup you want to use:") + Translation.tr
                    font.pointSize: 11
                }
                Column {
                    anchors.top: text.bottom
                    anchors.topMargin: 5
                    spacing: 5
                    anchors.horizontalCenter: parent.horizontalCenter
                    ExclusiveGroup { id: groupChoice }
                    UI.GSCheckBox {
                        id: choice1
                        text_: qsTr("Restore the volume header from thebackup embedded in the volume")
                        size_: 20
                        checked: true
                        exclusiveGroup: groupChoice
                        onCheckedChanged: {

                        }
                    }

                    UI.GSCheckBox {
                        id: choice2
                        text_: qsTr("Restore the volume header from an external backup file")
                        size_: 20
                        exclusiveGroup: groupChoice
                        onCheckedChanged: {

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
                    text: qsTr("Volume password") + Translation.tr
                    font.pointSize: 11
                }

                Column {
                    spacing: 10
                    anchors.top: text2.bottom
                    anchors.topMargin: 10
                    anchors.horizontalCenter: parent.horizontalCenter

                    Column {
                        id: password
                        spacing: 5
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
                                        if(use_Keyfiles.checked === true) {
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
            text: qsTr("Restore header...") + Translation.tr
            width: 140
            onClicked: {
                if(password_value.text !== "")
                {
                    top.update();
                }
            }
            color_: palette.blue
            anchors.bottomMargin: 10
        }
    }

}
