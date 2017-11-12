import QtQuick 2.7
import QtQuick.Controls 2.0
import "../" as UI

Item {
    id: top;
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
            spacing: 10
            height: 500
            anchors.horizontalCenter: parent.horizontalCenter
            Rectangle {
                id: block
                color: "transparent"
                border.width: 1
                border.color: palette.border
                radius: 5
                width: top.width-100
                height: 55
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: text
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 25
                    y: 5
                    color: palette.green
                    text: qsTr("Default mount options") + Translation.tr
                    font.pointSize: 11
                }

                Row {
                    spacing: 15
                    anchors.top: text.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    UI.GSCheckBox {
                        id: readOnly
                        text_: qsTr("Mount volumes as read-only")
                        size_: 20
                        sizeText: 10
                        checked: {
                            var isChecked = UserSettings.getSetting("Pref-ReadOnly")
                            return (isChecked === "1") ? true : false;
                        }
                        onCheckedChanged: {
                            if(readOnly.checked == true)
                                UserSettings.setSetting("Pref-ReadOnly", 1)
                            else
                                UserSettings.setSetting("Pref-ReadOnly", 0)
                        }
                    }
                    UI.GSCheckBox {
                        id: removable
                        text_: qsTr("Mount volumes as removable media")
                        size_: 20
                        sizeText: 10
                        checked: {
                            var isChecked = UserSettings.getSetting("Pref-RemovableMedia")
                            return (isChecked === "1") ? true : false;
                        }
                        onCheckedChanged: {
                            if(removable.checked == true)
                                UserSettings.setSetting("Pref-RemovableMedia", 1)
                            else
                                UserSettings.setSetting("Pref-RemovableMedia", 0)
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
                height: 55
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: text2
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 25
                    y: 5
                    color: palette.green
                    text: qsTr("GostCrypt Background Task") + Translation.tr
                    font.pointSize: 11
                }

                Row {
                    spacing: 15
                    anchors.top: text2.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    UI.GSCheckBox {
                        id: enabled
                        text_: qsTr("Enabled")
                        size_: 20
                        sizeText: 10
                        checked: {
                            var isChecked = UserSettings.getSetting("Pref-TaskEnabled")
                            return (isChecked === "1") ? true : false;
                        }
                        onCheckedChanged: {
                            if(enabled.checked == true)
                                UserSettings.setSetting("Pref-TaskEnabled", 1)
                            else
                                UserSettings.setSetting("Pref-TaskEnabled", 0)
                        }
                    }
                    UI.GSCheckBox {
                        id: exit
                        text_: qsTr("Exit when there are no mounted volumes")
                        size_: 20
                        sizeText: 10
                        checked: {
                            var isChecked = UserSettings.getSetting("Pref-TaskExitNoVolume")
                            return (isChecked === "1") ? true : false;
                        }
                        onCheckedChanged: {
                            if(exit.checked == true)
                                UserSettings.setSetting("Pref-TaskExitNoVolume", 1)
                            else
                                UserSettings.setSetting("Pref-TaskExitNoVolume", 0)
                        }
                    }
                }
            }

            Rectangle {
                id: block3
                color: "transparent"
                border.width: 1
                border.color: palette.border
                radius: 5
                width: top.width-100
                height: 85
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: text3
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 25
                    y: 5
                    color: palette.green
                    text: qsTr("Explorer") + Translation.tr
                    font.pointSize: 11
                }

                Column {
                    spacing: 5
                    anchors.top: text3.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    UI.GSCheckBox {
                        id: autoOpen
                        text_: qsTr("Open Explorer window for successfully mounted volume")
                        size_: 20
                        sizeText: 10
                        checked: {
                            var isChecked = UserSettings.getSetting("Pref-OpenOnMounted")
                            return (isChecked === "1") ? true : false;
                        }
                        onCheckedChanged: {
                            if(autoOpen.checked == true)
                                UserSettings.setSetting("Pref-OpenOnMounted", 1)
                            else
                                UserSettings.setSetting("Pref-OpenOnMounted", 0)
                        }
                    }
                    UI.GSCheckBox {
                        id: timestamp
                        text_: qsTr("Preserve modification timestamp of file containers")
                        size_: 20
                        sizeText: 10
                        checked: {
                            var isChecked = UserSettings.getSetting("Pref-TimeStamp")
                            return (isChecked === "1") ? true : false;
                        }
                        onCheckedChanged: {
                            if(timestamp.checked == true)
                                UserSettings.setSetting("Pref-TimeStamp", 1)
                            else
                                UserSettings.setSetting("Pref-TimeStamp", 0)
                        }
                    }
                }
            }

            Rectangle {
                id: block4
                color: "transparent"
                border.width: 1
                border.color: palette.border
                radius: 5
                width: top.width-100
                height: 110
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: text4
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 25
                    y: 5
                    color: palette.green
                    text: qsTr("Password Cache") + Translation.tr
                    font.pointSize: 11
                }

                Column {
                    spacing: 5
                    anchors.top: text4.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    UI.GSCheckBox {
                        id: cacheInDriver
                        text_: qsTr("Cache passwords in driver memory")
                        size_: 20
                        sizeText: 10
                        checked: {
                            var isChecked = UserSettings.getSetting("Pref-CacheInDriver")
                            return (isChecked === "1") ? true : false;
                        }
                        onCheckedChanged: {
                            if(cacheInDriver.checked == true)
                                UserSettings.setSetting("Pref-CacheInDriver", 1)
                            else
                                UserSettings.setSetting("Pref-CacheInDriver", 0)
                        }
                    }
                    UI.GSCheckBox {
                        id: wipe
                        text_: qsTr("Wipe cached passwords on exit")
                        size_: 20
                        sizeText: 10
                        checked: {
                            var isChecked = UserSettings.getSetting("Pref-WipeOnExit")
                            return (isChecked === "1") ? true : false;
                        }
                        onCheckedChanged: {
                            if(wipe.checked == true)
                                UserSettings.setSetting("Pref-WipeOnExit", 1)
                            else
                                UserSettings.setSetting("Pref-WipeOnExit", 0)
                        }
                    }
                    UI.GSCheckBox {
                        id: wipeOnAD
                        text_: qsTr("Wipe cached passwords on auto-dismount")
                        size_: 20
                        sizeText: 10
                        checked: {
                            var isChecked = UserSettings.getSetting("Pref-WipeOnAutoDismount")
                            return (isChecked === "1") ? true : false;
                        }
                        onCheckedChanged: {
                            if(wipeOnAD.checked == true)
                                UserSettings.setSetting("Pref-WipeOnAutoDismount", 1)
                            else
                                UserSettings.setSetting("Pref-WipeOnAutoDismount", 0)
                        }
                    }
                }
            }

            Rectangle {
                id: block5
                color: "transparent"
                border.width: 1
                border.color: palette.border
                radius: 5
                width: top.width-100
                height: 140
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: text5
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 25
                    y: 5
                    color: palette.green
                    text: qsTr("Password Cache") + Translation.tr
                    font.pointSize: 11
                }

                Column {
                    spacing: 5
                    anchors.top: text5.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    Row {
                        spacing: 10
                        Text {
                            id: textDismountWhen
                            height: 25
                            y: 13
                            color: palette.text
                            text: qsTr("Dismount all when :") + Translation.tr
                            font.pointSize: 10
                        }
                        Column {
                            spacing: 5
                            UI.GSCheckBox {
                                id: userLogOff
                                text_: qsTr("User logs off")
                                size_: 20
                                sizeText: 10
                                checked: {
                                    var isChecked = UserSettings.getSetting("Pref-AutoDismountWhenUserLogOff")
                                    return (isChecked === "1") ? true : false;
                                }
                                onCheckedChanged: {
                                    if(userLogOff.checked == true)
                                        UserSettings.setSetting("Pref-AutoDismountWhenUserLogOff", 1)
                                    else
                                        UserSettings.setSetting("Pref-AutoDismountWhenUserLogOff", 0)
                                }
                            }
                            UI.GSCheckBox {
                                id: screenSaver
                                text_: qsTr("Screen saver is launched")
                                size_: 20
                                sizeText: 10
                                checked: {
                                    var isChecked = UserSettings.getSetting("Pref-AutoDismountWhenScreenSaver")
                                    return (isChecked === "1") ? true : false;
                                }
                                onCheckedChanged: {
                                    if(screenSaver.checked == true)
                                        UserSettings.setSetting("Pref-AutoDismountWhenScreenSaver", 1)
                                    else
                                        UserSettings.setSetting("Pref-AutoDismountWhenScreenSaver", 0)
                                }
                            }
                        }
                        UI.GSCheckBox {
                            id: enteringPower
                            text_: qsTr("Entering power saving mode")
                            size_: 20
                            sizeText: 10
                            checked: {
                                var isChecked = UserSettings.getSetting("Pref-AutoDismountWhenPowerSaving")
                                return (isChecked === "1") ? true : false;
                            }
                            onCheckedChanged: {
                                if(enteringPower.checked == true)
                                    UserSettings.setSetting("Pref-AutoDismountWhenPowerSaving", 1)
                                else
                                    UserSettings.setSetting("Pref-AutoDismountWhenPowerSaving", 0)
                            }
                        }
                    }

                    Row {
                        y: 10
                        spacing: 10
                        UI.GSCheckBox {
                            id: autoDismountAfter
                            text_: qsTr("Auto-dismount volume after no data has been read/written to it for")
                            size_: 20
                            sizeText: 10
                            checked: {
                                var isChecked = UserSettings.getSetting("Pref-AutoDismountAfterXMinutesEnabled")
                                return (isChecked === "1") ? true : false;
                            }
                            onCheckedChanged: {
                                if(autoDismountAfter.checked == true) {
                                    UserSettings.setSetting("Pref-AutoDismountAfterXMinutesEnabled", 1)
                                    numberOfMinutes.readOnly = false
                                }
                                else {
                                    UserSettings.setSetting("Pref-AutoDismountAfterXMinutesEnabled", 0)
                                    numberOfMinutes.readOnly = true
                                }
                            }
                        }
                        UI.SecureTextField {
                            id: numberOfMinutes
                            type: true
                            width: 50
                            text: UserSettings.getSetting("Pref-AutoDismountAfter")
                            readOnly: autoDismountAfter.enabled
                        }
                        Text {
                            height: 25
                            color: palette.text
                            text: qsTr("minutes") + Translation.tr
                            font.pointSize: 10
                        }
                    }
                    UI.GSCheckBox {
                        id: forceAutoDismount
                        text_: qsTr("Force auto-dismount even if colume containes open files or directories")
                        size_: 20
                        sizeText: 10
                        checked: {
                            var isChecked = UserSettings.getSetting("Pref-ForceAutoDismountEventIfOpened")
                            return (isChecked === "1") ? true : false;
                        }
                        onCheckedChanged: {
                            if(forceAutoDismount.checked == true)
                                UserSettings.setSetting("Pref-ForceAutoDismountEventIfOpened", 1)
                            else
                                UserSettings.setSetting("Pref-ForceAutoDismountEventIfOpened", 0)
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
