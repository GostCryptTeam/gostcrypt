import QtQuick 2.7
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2

Component {
    id:volumeDelegate

    Item {
        id: item
        width: 240
        height:80
        property string mountPoint: ""

        function updateFavorite()
        {
            if(UserSettings.isFavorite(Path_) === true)
            {
                overlay.opacity = 1.0
                image.source = "ressource/volumeFavoriteHover.png"
                Favorite = true
            }else{
                overlay.opacity = 0.0
                image.source = "ressource/volumeFavorite.png"
                Favorite = false
            }
        }

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
            color: palette.dark
            Item {
                id:rightPartTexts
                anchors.fill: parent
                Text {
                    text: { return mountPoint_ + Translation.tr }
                    color: palette.textLight
                    font.pixelSize: 14
                    y: 7
                    width: 140
                    clip: true
                    elide: Text.ElideLeft
                    horizontalAlignment: Text.AlignHCenter
                    anchors.horizontalCenter: rightPartTexts.horizontalCenter
                }
                Text {
                    text: EncryptionAlgorithmName_ + Translation.tr
                    horizontalAlignment: Text.AlignHCenter
                    color: palette.textLight
                    font.pixelSize: 10
                    anchors.horizontalCenter: rightPartTexts.horizontalCenter
                    y: 26
                    width: 120
                    clip: true
                    elide: Text.ElideLeft
                }
                Text {
                    id: pathText
                    text: Path_ + Translation.tr
                    color: palette.textLight
                    font.pixelSize: 10
                    anchors.horizontalCenter: rightPartTexts.horizontalCenter
                    y: 41
                    width: 120
                    clip: true
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter

                }
                Text {
                    text: "Size : " + Size_ + Translation.tr
                    horizontalAlignment: Text.AlignHCenter
                    color: palette.textLight
                    font.pixelSize: 11
                    anchors.horizontalCenter: rightPartTexts.horizontalCenter
                    y: 56
                    width: 120
                    clip: true
                    elide: Text.ElideLeft
                }

                Behavior on opacity {
                    NumberAnimation {
                        duration: app.duration/2;
                        easing.type: Easing.OutQuad;
                    }
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: rightPart
                hoverEnabled: true
                propagateComposedEvents: true
                onEntered: {
                    updateFavorite()
                    dismountVolume.opacity = 1.0
                    volumeFavorite.opacity = 1.0
                    volumeProperties.opacity = 1.0
                    rightPartTexts.opacity = 0.1
                    volumeTools_.opacity = 1.0
                }
                onExited: {
                    dismountVolume.opacity = 0.0
                    volumeFavorite.opacity = 0.0
                    volumeProperties.opacity = 0.0
                    rightPartTexts.opacity = 1.0
                    volumeTools_.opacity = 0.0
                }

                //options
                Row {
                    id: rowOptions
                    spacing: 20
                    anchors.centerIn: parent
                    Rectangle {
                        id: volumeFavorite
                        width: 25
                        height:25
                        radius:0
                        color: "transparent"
                        opacity: 0.0

                        ToolTip {
                            parent: volumeFavorite
                            text: {
                                if(Favorite == false)
                                    qsTr("Add volume to favorites...")
                                else
                                    qsTr("Remove from favorites...")
                            }
                            visible: volumeFavoriteArea.containsMouse
                            delay: 500
                            timeout: 5000
                        }

                        Image {
                            id: image
                            source: {
                                if(Favorite === true)
                                    return "ressource/volumeFavoriteHover.png"
                                else
                                    return "ressource/volumeFavorite.png"
                            }
                            //asynchronous : true
                            //antialiasing: true
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

                        ColorOverlay{
                            id: overlay
                            anchors.fill: image
                            source:image
                            color: palette.blue
                            opacity: {
                                if(Favorite === false)
                                    return 0.0
                                else
                                    return 1.0
                            }
                            antialiasing: true
                        }

                        MouseArea {
                            id: volumeFavoriteArea
                            anchors.fill: volumeFavorite
                            hoverEnabled: true
                            onClicked: {
                                if(Favorite === false)
                                {
                                    UserSettings.setFavoritesVolumes(Path_)
                                    Favorite = true;
                                    image.source = "ressource/volumeFavoriteHover.png"
                                    overlay.opacity = 1.0
                                }else{
                                    UserSettings.setFavoritesVolumes(Path_)
                                    Favorite = false;
                                    image.source = "ressource/volumeFavorite.png"
                                    overlay.opacity = 0.0
                                }
                            }
                            onEntered: {
                                cursorShape = Qt.PointingHandCursor
                                image.source = "ressource/volumeFavoriteHover.png"
                            }
                            onExited: {
                                cursorShape = Qt.ArrowCursor
                                if(Favorite === true)
                                    image.source = "ressource/volumeFavoriteHover.png"
                                else
                                    image.source = "ressource/volumeFavorite.png"
                            }
                        }
                        Behavior on opacity {
                            NumberAnimation {
                                id: animationFavorite
                                duration: app.duration/3;
                                easing.type: Easing.OutQuad;
                                onRunningChanged: {

                                }
                            }
                        }
                    }


                    Rectangle {
                        id: volumeProperties
                        width: 25
                        height:25
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
                            source: "ressource/volumeProperties.png"
                           // asynchronous : true
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
                            }
                            onEntered: {
                                cursorShape = Qt.PointingHandCursor
                                image2.source = "ressource/volumePropertiesHover.png"
                            }
                            onExited: {
                                cursorShape = Qt.ArrowCursor
                                image2.source = "ressource/volumeProperties.png"
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
                        width: 25
                        height:25
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
                            source: "ressource/volumeTools.png"
                            //asynchronous : true
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
                                //LoadVolume.loadVolume("/media/volume", "GOST Grasshopper", "/home/user/myVolumes/volume", "5 MB");
                                volumeToolsMenu.path = Path_
                                volumeToolsMenu.opacity = (volumeToolsMenu.opacity == 0.0) ? 1.0 : 0.0
                                if(volumeToolsMenu.visible == false) volumeToolsMenu.visible = true
                                volumeToolsMenuLayer.opacity = (volumeToolsMenuLayer.opacity == 0.0) ? 0.4 : 0.0
                                if(volumeToolsMenuLayer.visible == false) volumeToolsMenuLayer.visible = true
                            }
                            onEntered: {
                                cursorShape = Qt.PointingHandCursor
                                image3.source = "ressource/volumeToolsHover.png"
                            }
                            onExited: {
                                cursorShape = Qt.ArrowCursor
                                image3.source = "ressource/volumeTools.png"
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
                        if(!animation.running && dismountVolume.isVisible === false) {
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

                qmlRequest("openmountpoint", {"path": mountPoint_});
            }
            onDoubleClicked: {
                qmlRequest("openmountpoint", {"path": mountPoint_});
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
                    qmlRequest("dismount", {"volumepath": Path_});
                }
            }
        }
        function getPath()
        {
            return Path_;
        }

    }




}

