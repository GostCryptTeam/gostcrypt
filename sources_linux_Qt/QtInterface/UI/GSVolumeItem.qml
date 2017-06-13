import QtQuick 2.7
import QtGraphicalEffects 1.0

Component {
    Item {
        Rectangle {
            width: 240
            height:80
            color: '#3b3b3b'
            radius: 70
        }
        Text {
            text: AuxMountPoint_
            color: "#bdbdbd"
            font.pointSize: 8
            x: 100
            y: 10
            width: 100
            clip: true
            elide: Text.ElideLeft
        }
        Text {
            text: EncryptionAlgorithmName_
            horizontalAlignment: Text.AlignHCenter
            color: "#e1e1e1"
            font.pointSize: 8
            x: 100
            y: 25
            width: 100
            clip: true
            elide: Text.ElideLeft
        }
        Text {
            text: Path_
            color: "#bdbdbd"
            font.pointSize: 8
            x: 100
            y: 40
            width: 100
            clip: true
            elide: Text.ElideRight
        }
        Text {
            text: "Size : "+ Size_
            horizontalAlignment: Text.AlignHCenter
            color: "#97c966"
            font.pointSize: 8
            x: 100
            y: 55
            width: 100
            clip: true
            elide: Text.ElideLeft
        }

        Image {
            id: disk
            source: "../ressource/disk.png"
            asynchronous : true
            x:25
            y:20
        }
    }
}

