import QtQuick 2.7

Item {
    property QtObject mainWindow_
    signal menuChanged(string name, int index)
    width: app.width

    BackgroundFrame {
        id: bk
        name: "Favorites"
    }

    Text {
        id:title
        font.pixelSize: 25
        color: palette.text
        anchors.horizontalCenter: parent.horizontalCenter
        y: 20
        text: "Favorites"
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
            text_: qsTr("Add mounted volume to Favorites...")
            description_: qsTr("???")
        }
        ItemMenu {
            text_: qsTr("Add mounted volume to system Favorites...")
            description_: qsTr("???")
        }
        ItemMenu {
            text_: qsTr("Organize favorite Volumes...")
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
            text_: qsTr("Organize System Favorite Volumes...")
            description_: qsTr("???")
        }
        ItemMenu {
            text_: qsTr("Mount Favorite Volumes...")
            description_: qsTr("???")
        }
    }
}
