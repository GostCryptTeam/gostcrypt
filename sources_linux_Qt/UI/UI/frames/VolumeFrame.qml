import QtQuick 2.7

Item {
    property QtObject mainWindow_
    signal menuChanged(string name, int index)
    width: app.width

    BackgroundFrame {
        name: "Volume"
    }

    Text {
        id:title
        font.pixelSize: 25
        color: palette.text
        anchors.horizontalCenter: parent.horizontalCenter
        y: 20
        text: qsTr("Volume") + Translation.tr
    }

    Grid {
        id: rowWindow1
        x: 98
        y: 120
        height: 130
        spacing: 15
        horizontalItemAlignment: Grid.AlignHCenter
        //columns: 4
        anchors.horizontalCenter: parent.horizontalCenter

        ItemMenu {
            text_: qsTr("Select File...")
            description_: qsTr("Select a GostCrypt volume.")
        }
        ItemMenu {
            text_: qsTr("Select Device...")
            description_: qsTr("Select a device to be mounted")
        }
        ItemMenu {
            text_: qsTr("Create new volume...")
            description_: qsTr("Opens the GostCrypt creation wizard.")
        }
        ItemMenu {
            text_: qsTr("Resume interrupted process")
            description_: qsTr("???")
        }
    }
    Grid {
        id: rowWindow2
        x: 98
        y: 120 + 130
        height: 130
        spacing: 15
        horizontalItemAlignment: Grid.AlignHCenter
        //columns: 4
        anchors.horizontalCenter: parent.horizontalCenter
        ItemMenu {
            text_: qsTr("Mount volume with options")
            description_: qsTr("???")
        }
        ItemMenu {
            text_: qsTr("Auto-Mount all Device-Hosted volumes")
            description_: qsTr("???")
        }
        ItemMenu {
            text_: qsTr("Dismount all mounted volumes")
            description_: qsTr("???")
        }
        ItemMenu {
            text_: qsTr("Change volume password...")
            description_: qsTr("???")
        }
    }
    Grid {
        id: rowWindow3
        x: 98
        y: 120 + 130*2
        height: 130
        spacing: 15
        horizontalItemAlignment: Grid.AlignHCenter
        columns: 3
        anchors.horizontalCenter: parent.horizontalCenter
        ItemMenu {
            text_: qsTr("Add/Remove Keyfiles from/to Volume...")
            description_: qsTr("???")
        }
        ItemMenu {
            text_: qsTr("Remove all Keyfile from Volume...")
            description_: qsTr("???")
        }
        ItemMenu {
            text_: qsTr("Set header Key Derivation Algorithm....")
            description_: qsTr("???")
        }
    }
}
