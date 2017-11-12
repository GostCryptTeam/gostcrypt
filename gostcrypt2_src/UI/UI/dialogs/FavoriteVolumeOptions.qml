import QtQuick 2.7
import "../" as UI

Item {
    id: top
    property string volumePath: "file:///home/william/testVolume"
    property variant volumeInfos: { return UserSettings.getFavoriteVolumeSetting(volumePath); }
    signal updateVolume()


    Connections {
        target: top
        onUpdateVolume: {
            console.log(readOnly.checked);
            UserSettings.setFavoriteVolumeSetting({
                                                      'sName': name.text,
                                                      'sPath': volumePath,
                                                      'sReadOnly': readOnly.checked,
                                                      'sRemovableMedium' : removableMedium.checked,
                                                      'sUponLogon': uponLogon.checked,
                                                      'sMountWhenDeviceConnected': mountWhenDeviceConnected.checked,
                                                      'sDoNotMountVolumeOnMountAllFavorite': openExplorerWhenMounted.checked
                                                  });
            app.refreshFavorite();
            catchClose();
        }
    }
    Text {
        id: text
        anchors.horizontalCenter: parent.horizontalCenter
        y: 10
        anchors.bottomMargin: 30
        color: palette.text
        font.pointSize: 11
        text: {
            volumePath = volumePath.replace(/^(file:\/{2})/,"");
            var cleanPath = decodeURIComponent(volumePath);
            return qsTr("Information about ") + Translation.tr + "\"" + cleanPath + "\""
        }
    }
    Column {
        anchors.topMargin: 15
        anchors.top: text.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 50
        Row {
            spacing: 10
            Text {
                id: text_
                text: qsTr("Volume name : ") + Translation.tr
                color: palette.text
                font.pointSize: 11
                verticalAlignment: Text.AlignVCenter
                height: 40
                anchors.bottom: name.bottom
            }
            UI.SecureTextField {
                id: name
                height: 40
                width: 250
                type: true
                text: {
                    return volumeInfos.sName
                }
                onValidated: {
                    top.updateVolume()
                }
            }
        }

        UI.GSCheckBox {
            id: readOnly
            text_: qsTr("Mount selected volume as read-only")
            height: 40
            checked: {
                return volumeInfos.sReadOnly
            }
        }
        UI.GSCheckBox {
            id: uponLogon
            text_: qsTr("Mount selected volume upon logon")
            height: 40
            checked: {
                return volumeInfos.sUponLogon
            }
        }
        UI.GSCheckBox {
            id: removableMedium
            text_: qsTr("Mount selected volume as removable medium")
            height: 40
            checked: {
                return volumeInfos.sRemovableMedium
            }
        }
        UI.GSCheckBox {
            id: mountWhenDeviceConnected
            text_: qsTr("Mount selected volume when its host device gets connected")
            height: 40
            checked: {
                return volumeInfos.sMountWhenDeviceConnected
            }
        }
        UI.GSCheckBox {
            id: openExplorerWhenMounted
            text_: qsTr("Do not mount selected volume when 'Mount Favorite Volumes' hot key is pressed")
            height: 40
            checked: {
                return volumeInfos.sOpenExplorerWhenMounted
            }
        }
    }

    UI.GSButtonBordered {
        id: saveButton
        height: 40
        text: qsTr("Save changes") + Translation.tr
        width: 120
        onClicked: top.updateVolume()
        color_: palette.blue
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 10
    }


}
