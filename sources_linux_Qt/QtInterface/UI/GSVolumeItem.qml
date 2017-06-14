import QtQuick 2.7
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2

Component {
    id:volumeDelegate
    Item {
        width: 240
        height:80
        TooltipArea {
                text: Path_
            }
        Rectangle {
            width: 240
            height:80
            color: '#3b3b3b'
            radius: 70
        }
        Text {
            text: AuxMountPoint_
            color: "#bdbdbd"
            font.pixelSize: 10
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
            font.pixelSize: 10
            x: 100
            y: 25
            width: 100
            clip: true
            elide: Text.ElideLeft
        }
        Text {
            id: pathText
            text: Path_
            color: "#bdbdbd"
            font.pixelSize: 10
            x: 100
            y: 40
            width: 100
            clip: true
            elide: Text.ElideRight

        }
        Text {
            text: "Size : " + Size_
            horizontalAlignment: Text.AlignHCenter
            color: "#97c966"
            font.pixelSize: 11
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
        Rectangle {
            id: dismountVolume
            width: 20
            height:20
            radius:25
            color: "#ffffff"
            opacity: 0.0
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                console.log("Entrée");
                dismountVolume.opacity = 1.0
            }
            onExited: {
                console.log("Sortie");
                dismountVolume.opacity = 0.0
            }

            onDoubleClicked: {
                ConnectSignals.openPath(AuxMountPoint_);
            }
        }


    }

}

