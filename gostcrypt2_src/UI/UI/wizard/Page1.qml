import QtQuick 2.7
import QtQuick.Controls 1.4
import "../" as UI

Item {
    id: top
    property int type: 0

    Text {
        id:titre
        font.pointSize: 13
        font.family: "Helvetica"
        text: qsTr("Please choose the type of container you want:") + Translation.tr
        anchors.horizontalCenter: parent.horizontalCenter
        color: palette.text
        wrapMode: Text.WordWrap
    }

    Row {
        id: choice
        spacing: 30
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: titre.bottom
        anchors.topMargin: 20
        Rectangle {
            id: file
            width: 220
            height: 150
            color: "transparent"

            Image {
                id: img1
                y: 10
                anchors.rightMargin: 10
                source: "../ressource/container.png"
                anchors.horizontalCenter: parent.horizontalCenter
                scale:  fileArea.containsMouse ? 0.8 : 1.0
                smooth: fileArea.containsMouse
                Behavior on width {
                    NumberAnimation {
                        duration: app.duration/2;
                        easing.type: Easing.OutQuad;
                    }
                }
            }
            Text {
                text: qsTr("Encrypted file container") + Translation.tr
                width: 150
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                color: palette.text
                font.pointSize: 13
                anchors.horizontalCenter: img1.horizontalCenter
                y: 100
                anchors.topMargin: 20
            }
            MouseArea {
                id: fileArea
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    scale.start();
                    desc.text = qsTr("Creates a virtual encrypted disk within a file. Recomended for "+
                                     "inexperienced users.") + Translation.tr
                }
                onExited: {
                    unScale.start();
                    desc.text = ""
                }
                onClicked: { type = 0; manageWizard(1) }
                cursorShape: Qt.PointingHandCursor
            }

            SequentialAnimation {
                id: scale
                loops: 1
                PropertyAnimation {
                    target: img1
                    properties: "scale"
                    from: 1.0
                    to: 0.9
                    duration: app.duration/2
                }
            }

            SequentialAnimation {
                id: unScale
                loops: 1
                PropertyAnimation {
                    target: img1
                    properties: "scale"
                    from: 0.9
                    to: 1.0
                    duration: app.duration/2
                }
            }
        }
        Rectangle {
            id: device
            width: 220
            height: 150
            color: "transparent"

            Image {
                id: img2
                y: 10
                anchors.rightMargin: 10
                source: "../ressource/device.png"
                anchors.horizontalCenter: parent.horizontalCenter
                scale:  deviceArea.containsMouse ? 0.8 : 1.0
                smooth: deviceArea.containsMouse
                Behavior on width {
                    NumberAnimation {
                        duration: app.duration/2;
                        easing.type: Easing.OutQuad;
                    }
                }
            }
            Text {
                text: qsTr("Non system partition/device") + Translation.tr
                width: 150
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                color: palette.text
                font.pointSize: 13
                anchors.horizontalCenter: img2.horizontalCenter
                y: 100
                anchors.topMargin: 20
            }
            MouseArea {
                id: deviceArea
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    scaleDevice.start();
                    desc.text = qsTr("Encrypts a non-system partition on any internal or"+
                                     " external drive (e.g. a flash drive). Optionnaly, creates "+
                                     "a hidden volume.") + Translation.tr
                }
                onExited: {
                    unScaleDevice.start();
                    desc.text = ""
                }
                onClicked: { type = 1; manageWizard(1) }
                cursorShape: Qt.PointingHandCursor
            }

            SequentialAnimation {
                id: scaleDevice
                loops: 1
                PropertyAnimation {
                    target: img2
                    properties: "scale"
                    from: 1.0
                    to: 0.9
                    duration: app.duration/2
                }
            }

            SequentialAnimation {
                id: unScaleDevice
                loops: 1
                PropertyAnimation {
                    target: img2
                    properties: "scale"
                    from: 0.9
                    to: 1.0
                    duration: app.duration/2
                }
            }
        }
    }

    Text {
        id: desc
        text: ""
        wrapMode: Text.WordWrap
        x: bottomBar.x
        y: bottomBar.y-2
        width: bottomBar.width- 40
        height: bottomBar.height
        horizontalAlignment: Text.AlignHCenter
        color: palette.textLowOpacity
        font.pointSize: 9
    }
}
