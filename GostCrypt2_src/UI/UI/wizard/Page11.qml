import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import "../" as UI

Item {
    id: top
    anchors.centerIn: parent
    property string pathMounted: ""
    property string path: "/"
    property int type: 2

    Text {
        id:infos
        anchors.horizontalCenter: parent.horizontalCenter
        width: top.width-120
        font.pixelSize: 12
        text: qsTr("Outer volume has been successfully created and mounted as drive " + pathMounted
                   + " To this volume you should now copy some sensitive-looking files that you atually do NOT "
                   + "want to hide. The files will be there for anyone forcing you to disclose your password. "
                   + "You will reveal only the password for this outer volume, not for the hidden one. "
                   + "The files that you really care about will be stored in the hidden volume, chich will be created "
                   + "later on. When you finish copying, click Next. Do not dismount the volume.<br><br>Note : "
                   + "After you click Next, cluster bitmap of the outer volume will be scanned to determine the size "
                   + "of uninterrupted area of free space whose end is aligned with the end of the volume. This "
                   + "area will accommodate the hidden volume, so it will limit its maximum possible size. Cluster "
                   + "bitmap scanning ensures that no data on the outer volume are overwritter by the hidden volume.") + Translation.tr
        x: 30
        color: palette.text
        horizontalAlignment: Text.AlignJustify
        wrapMode: Text.WordWrap
    }

    UI.GSButtonBordered {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 50
        text: qsTr("Open Outer Volume")
        color_: palette.blue
        onClicked: ConnectSignals.openPath(path);
    }
}
