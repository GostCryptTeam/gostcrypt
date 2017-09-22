import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import "../" as UI

Item {
    id: top
    anchors.centerIn: parent

    Text {
        id:infos
        anchors.centerIn: parent
        width: top.width-120
        font.pixelSize: 12
        text: qsTr("In the next steps, you will set the options for the outer volume"
                   +" (within which the hidden volume will be created later on).") + Translation.tr
        x: 60
        color: palette.text
        horizontalAlignment: Text.AlignJustify
        wrapMode: Text.WordWrap
    }
}
