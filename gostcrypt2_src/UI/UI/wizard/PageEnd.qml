import QtQuick 2.7
import"../" as UI

Item {
    id: top
    anchors.fill: parent
    Text {
        id:infos
        anchors.centerIn: parent
        font.pixelSize: 12
        text: qsTr("<font size='7' color='2f88a7'>Done !</font><br>Click the exit button to finish.") + Translation.tr
        color: palette.text
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
    }
    UI.GSButtonBordered {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 20
        text: qsTr("Exit")
        color_: palette.blue
        onClicked: catchClose()
    }
}
