import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import "../" as UI

Item {
    id: top
    anchors.centerIn: parent
    property int type: 0

    Text {
        id:infos
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 40
        font.pointSize: 13
        font.family: "Helvetica"
        text: qsTr("The volume cluster bitmap has been scanned and the maximum "
                   + "possible size of the hidden volume has been determined. In the "
                   + "next steps you will set the options, the size, and the password "
                   + "for the hidden volume.") + Translation.tr
        y: 25
        color: palette.text
        horizontalAlignment: Text.AlignJustify
        wrapMode: Text.WordWrap
    }
}
