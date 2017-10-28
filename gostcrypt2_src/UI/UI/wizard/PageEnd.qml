import QtQuick 2.7
import"../" as UI

Item {
    id: top
    anchors.fill: parent
    Text {
        id:infos
        anchors.horizontalCenter: parent.horizontalCenter
        y: 25
        font.pixelSize: 12
        text: qsTr("<font size='7' color='2f88a7'>The volume is being created</font><br><br>You will receive a notification (top right of the screen) when the operation is completed. Do not close GostCrypt until loading is complete.") + Translation.tr
        color: palette.text
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        width: 350
    }
    UI.GSButtonBordered {
        anchors.top: infos.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 20
        text: qsTr("Exit")
        color_: palette.blue
        onClicked: catchClose()
    }
}
