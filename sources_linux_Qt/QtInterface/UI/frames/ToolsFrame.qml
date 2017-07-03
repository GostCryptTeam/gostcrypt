import QtQuick 2.0

Item {
    property QtObject mainWindow_
    signal menuChanged(string name, int index)
    width: app.width

    BackgroundFrame {
        name: "Tools"
    }

    Text {
        id:title
        font.pixelSize: 25
        color: palette.text
        anchors.horizontalCenter: parent.horizontalCenter
        y: 20
        text: "Tools"
    }
}
