import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4 as OLDControl
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
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
        id: addKeyfile
        title: qsTr("Please choose a keyfile") + Translation.tr
        folder: shortcuts.home
        onAccepted: {
            UserSettings.addKeyfile(addKeyfile.fileUrl)
            updateTableview()
        }
        onRejected: {
        }
    }

    FileDialog {
        id: addTokenFile
        title: qsTr("Please choose a Token File") + Translation.tr
        folder: shortcuts.home
        onAccepted: {
            UserSettings.addKeyfile(addTokenFile.fileUrl)
            updateTableview()
        }
        onRejected: {
        }
    }

    FileDialog {
        id: addFolder
        title: qsTr("Please choose a Folder") + Translation.tr
        folder: shortcuts.home
        selectFolder: true
        onAccepted: {
            UserSettings.addKeyfilePath(addFolder.fileUrl)
            updateTableview()
        }
        onRejected: {
        }
    }



    Row {
        id: tableRow
        y: 15
        width: parent.width - 150
        height: 150
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10


        OLDControl.TableView {
            id: table
            width: 350
            OLDControl.TableViewColumn {
                role: "path"
                title: "Keyfiles"
            }
            style: TableViewStyle {
                frame: Rectangle {
                    border{
                        color: palette.darkInput
                    }
                }
                headerDelegate: Rectangle {
                    height: textItem.implicitHeight * 1.2
                    width: textItem.implicitWidth
                    color: palette.darkInput
                    Text {
                        id: textItem
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: styleData.textAlignment
                        anchors.leftMargin: 12
                        text: styleData.value
                        elide: Text.ElideRight
                        color: palette.text
                        renderType: Text.NativeRendering
                    }
                    Rectangle {
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 1
                        anchors.topMargin: 1
                        width: 1
                        color: palette.darkInput
                    }
                }
                rowDelegate: Rectangle {
                    color: {
                        var baseColor = styleData.alternate ? palette.darkSecond : palette.darkThird
                        return styleData.selected ? palette.dark : baseColor
                    }
                }

                activateItemOnSingleClick : true
                alternateBackgroundColor : palette.darkSecond
                backgroundColor : palette.darkThird
                highlightedTextColor : palette.blue
                textColor : palette.text


            }

            model: ListModel {
                id: libraryModel

                Component.onCompleted: {
                    return updateTableview();
                }
            }
        }
        Text {
            color: palette.text
            width: 200
            wrapMode: Text.WordWrap
            text: qsTr("WARNING: If you lose a keyfile or if any bit of its first 1024 kilobytes changes, "
                       +"it will be impossible to mount volumes that use the keyfile !") + Translation.tr
            font.pointSize: 10
        }
    }
    Column {
        spacing: 13
        anchors.top: tableRow.bottom
        anchors.topMargin: 15
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 150
        Row {
            spacing: 10
            width: parent.width - 150
            UI.ButtonBordered {
                id: addFiles
                height: 40
                text: qsTr("Add Files...") + Translation.tr
                width: 100
                onClicked: addKeyfile.open()
                color_: palette.green
                anchors.bottomMargin: 10
            }
            UI.ButtonBordered {
                id: aAddPath
                height: 40
                text: qsTr("Add Path...") + Translation.tr
                width: 100
                onClicked: addFolder.open()
                color_: palette.green
                anchors.bottomMargin: 10
            }
            UI.ButtonBordered {
                id: addTokenFiles
                height: 40
                text: qsTr("Add Token Files...") + Translation.tr
                width: 120
                onClicked: addTokenFile.open()
                color_: palette.green
                anchors.bottomMargin: 10
            }
            UI.ButtonBordered {
                id: remove
                height: 40
                text: qsTr("Remove") + Translation.tr
                width: 80
                onClicked: { if(table.currentRow != -1) { UserSettings.removeKeyfile(table.model.get(table.currentRow).path); updateTableview() } }
                color_: palette.blue
                anchors.bottomMargin: 10
            }
            UI.ButtonBordered {
                id: removeAll
                height: 40
                text: qsTr("Remove All") + Translation.tr
                width: 100
                onClicked: { UserSettings.removeAllKeyfile(); updateTableview(); }
                color_: palette.blue
                anchors.bottomMargin: 10
            }
        }

        Text {
            color: palette.text
            width: parent.width
            wrapMode: Text.WordWrap
            text: qsTr("Any kind of file (for example, .mp3, .jpg, .zip, .avi) may be used as "
                       +"a GostCrypt keyfile. Note that GostCrypt never modifies the keyfile contents. "
                       +"You can select more than one keyfile (the order does not matter). If you add a "
                       +"folder, all non-hidden files found in it will be used as keyfiles. Click 'Add Token Files' "
                       +"to select keyfiles stored on security or smart cards (or to import keyfiles to "
                       +"security tokens or smart cards).") + Translation.tr
            font.pointSize: 8
        }
    }



    Row {
        spacing: 15
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        UI.CheckBox {
            id: closeTokenSessionAfterMount
            text_: qsTr("Use keyfile") + Translation.tr
            y: 5
            size_: 30
            sizeText: 12
            textColor: palette.green
            checked: {
                var isChecked = UserSettings.getSetting("MountV-UseKeyFile")
                return (isChecked === "1") ? true : false;
            }
            onCheckedChanged: {
                if(closeTokenSessionAfterMount.checked == true)
                    UserSettings.setSetting("MountV-UseKeyFile", 1)
                else
                    UserSettings.setSetting("MountV-UseKeyFile", 0)
            }
        }
        UI.ButtonBordered {
            id: generate
            height: 40
            text: qsTr("Generate Random Keyfiles...") + Translation.tr
            width: 200
            onClicked: top.update()
            color_: palette.green
            anchors.bottomMargin: 10
        }

        UI.ButtonBordered {
            id: save
            height: 40
            text: qsTr("Save changes") + Translation.tr
            width: 120
            onClicked: top.update()
            color_: palette.blue
            anchors.bottomMargin: 10
        }
    }

    function updateTableview() {
        table.model.clear();
        var keyfiles = UserSettings.getFavoriteKeyFiles();
        for(var i = 0; i < keyfiles.length; ++i) table.model.append({ path: keyfiles[i] })
    }
}
