import QtQuick 2.7
import QtGraphicalEffects 1.0

Component {
    /*Item {
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
        Image {
            id: disk
            source: "../ressource/disk.png"
            asynchronous : true
            x:15
            y:15
        }
        ColorOverlay {
              anchors.fill: disk
              source: disk
              color: "#80800000"
          }
    }*/
    VolumeItemGraphic {
        id: volumeItem
        anchors.horizontalCenter: mainWindow.anchors.horizontalCenter
        width:585
        height:80
    }
}
