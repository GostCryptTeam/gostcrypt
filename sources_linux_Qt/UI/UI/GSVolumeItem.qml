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
            Item {
                id:rightPartTexts
                anchors.fill: parent
                Text {
                    text: MountPoint_ + Translation.tr
                    color: "#bdbdbd"
                    font.pixelSize: 10
                    y: 10
                    width: 120
                    clip: true
                    elide: Text.ElideLeft
                    horizontalAlignment: Text.AlignHCenter
                    anchors.horizontalCenter: rightPartTexts.horizontalCenter
                }
                Text {
                    text: EncryptionAlgorithmName_ + Translation.tr
                    horizontalAlignment: Text.AlignHCenter
                    color: "#e1e1e1"
                    font.pixelSize: 10
                    anchors.horizontalCenter: rightPartTexts.horizontalCenter
                    y: 25
                    width: 120
                    clip: true
                    elide: Text.ElideLeft
                }
                Text {
                    id: pathText
                    text: Path_ + Translation.tr
                    color: "#bdbdbd"
                    font.pixelSize: 10
                    anchors.horizontalCenter: rightPartTexts.horizontalCenter
                    y: 40
                    width: 120
                    clip: true
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter

                }
                Text {
                    text: "Size : " + Size_ + Translation.tr
                    horizontalAlignment: Text.AlignHCenter
                    color: "#97c966"
                    font.pixelSize: 11
                    anchors.horizontalCenter: rightPartTexts.horizontalCenter
                    y: 55
                    width: 120
                    clip: true
                    elide: Text.ElideLeft
                }
            }

            GaussianBlur {
                id: blur
                anchors.fill: rightPartTexts
                source: rightPartTexts
                radius: 8
                samples: 16
                deviation: 3
                opacity: 0.0
            }

            MouseArea {
                id: mouseArea
                anchors.fill: rightPart
                hoverEnabled: true
                propagateComposedEvents: true
                onEntered: {
                    dismountVolume.opacity = 1.0
                    volumeFavorite.opacity = 1.0
                    volumeProperties.opacity = 1.0
                    rightPartTexts.opacity = 0.1
                    blur.opacity = 0.4
                    volumeTools_.opacity = 1.0
                }
                onExited: {
                    dismountVolume.opacity = 0.0
                    volumeFavorite.opacity = 0.0
                    volumeProperties.opacity = 0.0
                    rightPartTexts.opacity = 1.0
                    blur.opacity = 0.0
                    volumeTools_.opacity = 0.0
                }

                //options
                Row {
                    id: rowOptions
                    spacing: 14
                    anchors.centerIn: parent
                    Rectangle {
                        id: volumeFavorite
                        width: 35
                        height:35
                        radius:0
                        color: "transparent"
                        opacity: 0.0

                        ToolTip {
                            parent: volumeFavorite
                            text: qsTr("Add/remove volume from favorites...")
                            visible: volumeFavoriteArea.containsMouse
                            delay: 500
                            timeout: 5000
                        }

                        Image {
                            id: image
                            source: "ressource/volumeFavorite.svg"
                            asynchronous : true
                            x:0
                            y:0
                            width:parent.width
                            height:parent.height
                            Behavior on y {
                                NumberAnimation {
                                    duration: app.duration;
                                    easing.type: Easing.OutQuad;
                                }
                            }
                        }

                        MouseArea {
                            id: volumeFavoriteArea
                            anchors.fill: volumeFavorite
                            hoverEnabled: true
                            onClicked: {
                                console.log("Volume favorite");
                            }
                            onEntered: {
                                cursorShape = Qt.PointingHandCursor
                                image.source = "ressource/volumeFavoriteHover.svg"
                            }
                            onExited: {
                                cursorShape = Qt.ArrowCursor
                                image.source = "ressource/volumeFavorite.svg"
                            }
                        }
                        Behavior on opacity {
                            NumberAnimation {
                                id: animationFavorite
                                duration: app.duration/2;
                                easing.type: Easing.OutQuad;
                                onRunningChanged: {

                                }
                            }
                        }
                    }


                    Rectangle {
                        id: volumeProperties
                        width: 35
                        height:35
                        radius:0
                        color: "transparent"
                        opacity: 0.0

                        ToolTip {
                            parent: volumeProperties
                            text: qsTr("Volume properties...")
                            visible: volumePropertiesArea.containsMouse
                            delay: 500
                            timeout: 5000
                        }

                        Image {
                            id: image2
                            source: "ressource/volumeProperties.svg"
                            asynchronous : true
                            x:0
                            y:0
                            width:parent.width
                            height:parent.height
                            Behavior on y {
                                NumberAnimation {
                                    duration: app.duration;
                                    easing.type: Easing.OutQuad;
                                }
                            }
                        }

                        MouseArea {
                            id: volumePropertiesArea
                            anchors.fill: volumeProperties
                            hoverEnabled: true
                            onClicked: {
                                console.log("Volume properties");
                            }
                            onEntered: {
                                cursorShape = Qt.PointingHandCursor
                                image2.source = "ressource/volumePropertiesHover.svg"
                            }
                            onExited: {
                                cursorShape = Qt.ArrowCursor
                                image2.source = "ressource/volumeProperties.svg"
                            }
                        }
                        Behavior on opacity {
                            NumberAnimation {
                                id: animationProperties
                                duration: app.duration/2;
                                easing.type: Easing.OutQuad;
                                onRunningChanged: {

                                }
                            }
                        }
                    }


                    Rectangle {
                        id: volumeTools_
                        width: 35
                        height:35
                        radius:0
                        color: "transparent"
                        opacity: 0.0
                        ToolTip {
                            parent: volumeTools_
                            text: qsTr("Volume Tools...")
                            visible: volumeTools_Area.containsMouse
                            delay: 500
                            timeout: 5000
                        }

                        Image {
                            id: image3
                            source: "ressource/volumeTools.svg"
                            asynchronous : true
                            smooth: true
                            x:0
                            y:0
                            width:parent.width
                            height:parent.height
                            Behavior on y {
                                NumberAnimation {
                                    duration: app.duration;
                                    easing.type: Easing.OutQuad;
                                }
                            }
                        }

                        MouseArea {
                            id:volumeTools_Area
                            anchors.fill: volumeTools_
                            hoverEnabled: true
                            onClicked: {
                                console.log("Volume tools");
                            }
                            onEntered: {
                                cursorShape = Qt.PointingHandCursor
                                image3.source = "ressource/volumeToolsHover.svg"
                            }
                            onExited: {
                                cursorShape = Qt.ArrowCursor
                                image3.source = "ressource/volumeTools.svg"
                            }
                        }
                        Behavior on opacity {
                            NumberAnimation {
                                id: animationTools
                                duration: app.duration/2;
                                easing.type: Easing.OutQuad;
                                onRunningChanged: {

                                }
                            }
                        }
                    }
                }
            }
        }

        //left part
        Rectangle {
            id: leftPart
            width: 85
            height:80
            color: palette.darkThird
            radius: 2
            border.width: 1
            border.color: palette.darkInput
        }

        Image {
            id: disk
            source: "ressource/disk.png"
            asynchronous : true
            x:15
            y:20
            Behavior on y {
                NumberAnimation {
                    duration: app.duration/2.2;
                    easing.type: Easing.OutQuad;
                }
            }
            Behavior on opacity {
                NumberAnimation {
                    duration: app.duration;
                    easing.type: Easing.OutQuad;
                }
            }
        }

        Text {
            id: openVolume
            text: qsTr("Open") + Translation.tr
            color: palette.text
            opacity: 0.0
            anchors.horizontalCenter: parent.horizontalAlignment
            y: 55
            horizontalAlignment: Text.AlignHCenter
            width: 85
            Behavior on opacity {
                NumberAnimation {
                    duration: app.duration/2;
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

            ToolTip {
                parent: dismountVolume
                text: qsTr("Dismount volume...")
                visible: dismountVolumeArea.containsMouse
                delay: 500
                timeout: 5000
            }
            Text {
                id: dismountButton
                anchors.centerIn: parent
                text: "×"
                color: palette.text
                font.pixelSize: 20
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
            id: mouseAreaLeft
            anchors.fill: leftPart
            hoverEnabled: true
            propagateComposedEvents: true
            onEntered: {
                dismountVolume.opacity = 1.0
                disk.y = 10
                openVolume.opacity = 1.0
                disk.opacity = 0.5
                cursorShape = Qt.PointingHandCursor
            }
            onExited: {
                dismountVolume.opacity = 0.0
                openVolume.opacity = 0.0
                disk.y = 20
                disk.opacity = 1.0
                cursorShape = Qt.ArrowCursor
            }
            onClicked: {
                ConnectSignals.openPath(MountPoint_);
            }
            onDoubleClicked: {
                ConnectSignals.openPath(MountPoint_);
            }

            ToolTip {
                parent: form
                text: qsTr("Click to open volume")
                visible: mouseAreaLeft.containsMouse && !dismountVolumeArea.containsMouse
                delay: 500
                timeout: 5000
            }

            MouseArea {
                id: dismountVolumeArea
                width: 20
                height: 20
                x:0
                y:0
                hoverEnabled: true
                onEntered: {
                    dismountButton.color = palette.blue
                }
                onExited: {
                    dismountButton.color = palette.text
                }

                onClicked: {
                    console.log("Dismount volume");
                    ConnectSignals.connectReceiveDismount(Path_);
                    listOfVolumes.remove(index);
                    if(listOfVolumes.count == 0) helpStart.visible = true;
                }
            }
        }

    }
}

