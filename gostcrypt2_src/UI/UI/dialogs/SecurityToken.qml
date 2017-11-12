import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import "../" as UI

Item {
    id: top;
    signal update()

    Connections {
        target: top
        onUpdate: {
            catchClose();
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Please choose a file") + Translation.tr
        folder: shortcuts.home
        onAccepted: {
            UserSettings.addPKCSPath(fileDialog.fileUrl)
            var paths = [];
            paths.push(UserSettings.getSetting("Pref-PKCSLibPath"))
            combo.model = paths
        }
        onRejected: {
        }
    }

    Column {
        id: content
        y: 20
        spacing: 10
        anchors.horizontalCenter: parent.horizontalCenter
        Rectangle {
            id: block
            color: "transparent"
            border.width: 1
            border.color: palette.border
            radius: 5
            width: top.width-100
            height: 270
            anchors.horizontalCenter: parent.horizontalCenter
            Text {
                id: text
                anchors.horizontalCenter: parent.horizontalCenter
                height: 25
                y: 5
                color: palette.green
                text: qsTr("PKCS #11 Library path") + Translation.tr
                font.pointSize: 11
            }

            Row {
                id: row
                spacing: 15
                anchors.top: text.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                UI.GSCustomComboBox {
                    id: combo
                    width: 250
                    height: 40
                    model: {
                        var paths = [];
                        paths.push(UserSettings.getSetting("Pref-PKCSLibPath"))
                        return paths;
                    }
                }
                UI.GSButtonBordered {
                    id: selectPKCS
                    height: combo.height
                    text: qsTr("Open...") + Translation.tr
                    width: 120
                    onClicked: fileDialog.open()
                    color_: palette.green
                }
            }
            Column {
                spacing: 10
                anchors.top: row.bottom
                width: top.width - 200
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 10

                UI.GSButtonBordered {
                    id: autoDetect
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Auto-Detect Library") + Translation.tr
                    width: 200
                    height: 30
                    onClicked: qmlRequest("autodetectPKCSlib", "") //TODO
                    color_: palette.blue
                    y: 10
                }
                Text {
                    color: palette.text
                    width: parent.width
                    wrapMode: Text.WordWrap
                    text: qsTr("In order to allow GostCrypt to access a security token or smart card, "
                               +"you need to install a PKCS #11 software library for the token or smart card "
                               +"first. Such a library may be supplied with the device or it may be avaible "
                               +"for download from the website of the vendor or other third parties.<br><br>"
                               +"After you install the library, you can either select it manually by clicking "
                               +"'Select Library' or you can let GostCrypt find and select it automatically, "
                               +"by clicking 'Auto-Detect Library' (only the Linux system directory will be "
                               +"searched).") + Translation.tr
                    font.pointSize: 8
                }
                UI.GSCheckBox {
                    id: closeTokenSessionAfterMount
                    text_: qsTr("Close token session (log out) after a volume is successfully mounted") + Translation.tr
                    size_: 20
                    sizeText: 10
                    anchors.topMargin: 25
                    textColor: palette.green
                    checked: {
                        var isChecked = UserSettings.getSetting("Pref-CloseTokenSessionAfterMount")
                        return (isChecked === "1") ? true : false;
                    }
                    onCheckedChanged: {
                        if(closeTokenSessionAfterMount.checked == true)
                            UserSettings.setSetting("Pref-CloseTokenSessionAfterMount", 1)
                        else
                            UserSettings.setSetting("Pref-CloseTokenSessionAfterMount", 0)
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
            id: save
            height: 40
            text: qsTr("Save changes") + Translation.tr
            width: 120
            onClicked: top.update()
            color_: palette.blue
            anchors.bottomMargin: 10
        }
    }

}
