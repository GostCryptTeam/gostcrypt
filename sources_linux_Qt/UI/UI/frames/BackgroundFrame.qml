import QtQuick 2.0

Item {
    property var name
    Rectangle {
        id: topBar
        x: 0
        y: 0
        width: 790
        height: 74
        color: palette.darkSecond
    }

    Rectangle {
        id: bk
        x: 98
        y: 100
        width: 594
        height: 400
        color: palette.darkSecond
    }

    Text {
        anchors.verticalCenter: topBar.verticalCenter
        anchors.right: bk.right
        y: 5
        text: "<font color=\"#e1e1e1\"><a href=\"home\">"+qsTr("Home")+"</font></a> > "+ name
        color: palette.text
        font.pixelSize: 12

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
        }
        onLinkActivated: {
            menuChanged("frames/HomeFrame.qml",0)
            ConnectSignals.getAllMountedVolumes()
        }
    }
}
