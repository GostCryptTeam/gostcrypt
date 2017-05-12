import QtQuick 2.7

Component {
    Item {

        anchors.horizontalCenter: mainWindow.anchors.horizontalCenter
        width:585
        height:80
        Rectangle {
            anchors.margins: 5
            anchors.fill: parent
            color: '#3b3b3b'
            radius: 25
        }
        Text {
            text: text_
            color: "#e1e1e1"
            font.pointSize: 11
            x: 20
            y: 30
        }
    }
}
