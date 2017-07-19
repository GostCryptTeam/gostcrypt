import QtQuick 2.7
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2

Component {
    id:volumeDelegate

    Item {
        id: item
        width: 240
        height:80
        TooltipArea {
            text: Path_
        }
        Rectangle {
            id: form
            width: 240
            height:80
            color: '#3b3b3b'
            radius: 2
            border.width: 1
            border.color: palette.darkInput

        }
        Rectangle {
            id: rightPart
            x:85
            y:0
            height:80
            width: 240-85
            color: "transparent"
            ToolTip {
                parent: form
                text: qsTr("Double-click to open volume")
                visible: mouseArea.containsMouse
                delay: 500
                timeout: 5000
            }
        }

        Rectangle {
            id: leftPart
            width: 85
            height:80
            color: palette.darkThird
            radius: 2
            border.width: 1
            border.color: palette.darkInput
            ToolTip {
                parent: leftPart
                text: qsTr("TODO: shortcuts ")
                visible: mouseAreaLeft.containsMouse
                delay: 500
                timeout: 5000
            }
        }
        Text {
            text: MountPoint_
            color: "#bdbdbd"
            font.pixelSize: 10
            x: 100
            y: 10
            width: 120
            clip: true
            elide: Text.ElideLeft
            horizontalAlignment: Text.AlignHCenter
        }
        Text {
            text: EncryptionAlgorithmName_
            horizontalAlignment: Text.AlignHCenter
            color: "#e1e1e1"
            font.pixelSize: 10
            x: 100
            y: 25
            width: 120
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
            width: 120
            clip: true
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter

        }
        Text {
            text: "Size : " + Size_
            horizontalAlignment: Text.AlignHCenter
            color: "#97c966"
            font.pixelSize: 11
            x: 100
            y: 55
            width: 120
            clip: true
            elide: Text.ElideLeft
        }

        Image {
            id: disk
            source: "ressource/disk.png"
            asynchronous : true
            x:15
            y:20
            Behavior on y {
                NumberAnimation {
                    duration: app.duration;
                    easing.type: Easing.OutQuad;
                }
            }
        }
        Rectangle {
            id: dismountVolume
            width: 25
            height:25
            radius:2
            color: "transparent"
            opacity: 0.0
            Text {
                anchors.centerIn: parent
                text: "×"
                color: palette.text
                font.pixelSize: 20
            }

            MouseArea {
                anchors.fill: dismountVolume
                onClicked: {
                    console.log("Dismount volume");
                    ConnectSignals.connectReceiveDismount(Path_);
                    listOfVolumes.remove(index);
                    if(listOfVolumes.count == 0) helpStart.visible = true;
                }
            }
            Behavior on opacity {
                NumberAnimation {
                    id: animation
                    duration: app.duration/2;
                    easing.type: Easing.OutQuad;
                    onRunningChanged: {
                        if(!animation.running && dismountVolume.isVisible == false) {
                            dismountVolume.visible = false
                        }
                    }
                }
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: rightPart
            hoverEnabled: true
            propagateComposedEvents: true
            onEntered: {
                dismountVolume.opacity = 1.0
            }
            onExited: {
                dismountVolume.opacity = 0.0
            }
            onDoubleClicked: {
                ConnectSignals.openPath(MountPoint_);
            }
        }

        MouseArea {
            id: mouseAreaLeft
            anchors.fill: leftPart
            hoverEnabled: true
            propagateComposedEvents: true
            onEntered: {
                dismountVolume.opacity = 1.0
                disk.y = 10
            }
            onExited: {
                dismountVolume.opacity = 0.0
                disk.y = 20
            }
        }
    }
}

