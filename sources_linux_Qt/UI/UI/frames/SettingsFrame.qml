import QtQuick 2.7

Item {
    property QtObject mainWindow_
    signal menuChanged(string name, int index)
    width: app.width

    BackgroundFrame {
        name: "Settings"
    }

    Text {
        id:title
        font.pixelSize: 25
        color: palette.text
        anchors.horizontalCenter: parent.horizontalCenter
        y: 20
        text: "Settings"
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
            text_: qsTr("Language...")
            description_: qsTr("???")
        }
        ItemMenu {
            text_: qsTr("Hot Keys...")
            description_: qsTr("???")
        }
        ItemMenu {
            text_: qsTr("System Encryption...")
            description_: qsTr("???")
        }
        ItemMenu {
            text_: qsTr("System Favorite Volumes...")
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
            text_: qsTr("Performance")
            description_: qsTr("???")
        }
        ItemMenu {
            text_: qsTr("Default Keyfiles...")
            description_: qsTr("???")
        }
        ItemMenu {
            text_: qsTr("Security Tokens...")
            description_: qsTr("???")
        }
        ItemMenu {
            text_: qsTr("Preferences...")
            description_: qsTr("???")
        }
    }
}
