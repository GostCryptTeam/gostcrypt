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
        text: qsTr("Please choose the type of volume you want:") + Translation.tr
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
                y: 0
                anchors.rightMargin: 10
                source: "../ressource/normal.png"
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
                text: qsTr("Standard GostCrypt volume") + Translation.tr
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
                    desc.text = qsTr("Select this option if you want to create a normal GostCrypt volume.") + Translation.tr
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
                y: 0
                anchors.rightMargin: 10
                source: "../ressource/hidden.png"
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
                text: qsTr("Hidden GostCrypt Volume") + Translation.tr
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
                    desc.text = qsTr( "A hidden GostCrypt volume allows you to create a second volume inside your main volume. Simply enter the desired volume password to access it."+
                                     " <font color='#719c24'><a href=\"#\">More information.</a></font>") + Translation.tr
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


/*
Item {
    id: top
    x: 20
    property int type: 0
    Grid {
        y: 20
        columns: 1
        rows: 2
        height:parent.height
        width: parent.width
        rowSpacing: 40
        ExclusiveGroup { id: groupRadio }
        UI.GSCheckBox {
            id: stdGVol
            text_: qsTr("Standard GostCrypt Volume")
            checked: true
            height: 30
            exclusiveGroup :groupRadio
            onCheckedChanged: {
                if(stdGVol.checked === true) {
                    top.type = 0
                }
            }
            Text {function setType(value)
    {
        type = value
        if(value === 0)
            encryptedFile.checked = true
        else if (value === 1)
            nonSystem.checked = true
        else
            system.checked = true
    }
                id:description
                width: top.width-60
                font.pixelSize: 12
                text:
                anchors.top:stdGVol.bottom
                color: palette.text
                leftPadding: 40
                wrapMode: Text.WordWrap
            }
        }

        UI.GSCheckBox {
            id: hiddenVol
            text_: qsTr("Hidden GostCrypt volume")
            checked: false
            height: 40
            exclusiveGroup :groupRadio
            onCheckedChanged: {
                if(hiddenVol.checked === true) {
                    top.type = 1
                }
            }
            Text {
                id:description3
                width: top.width-60
                font.pixelSize: 12
                text: qsTr( "It may happen that you are forced by somebody to reveal the"+
                            " password of an encrypted volume. There are many situations where"+
                            " you cannot refuse to reveal the password (for example, due to extorsion) "+
                            ". Using a so-called hidden volume allows you to solve such situations without revealing the password of your volume."+
                            "<br><font color='#719c24'><a href=\"#\">More information about hidden volumes</a></font>") + Translation.tr
                anchors.top:hiddenVol.bottom
                color: palette.text
                leftPadding: 40
                wrapMode: Text.WordWrap
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
        }
    }

}
*/
