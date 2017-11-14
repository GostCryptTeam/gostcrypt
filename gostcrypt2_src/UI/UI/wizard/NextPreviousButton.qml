import QtQuick 2.7
import "../" as UI

Rectangle {
    property bool type: false
    signal pressed()

    id: top

    width: 40
    height: 40
    radius: 20
    color: palette.grayWizard

    Text {
        anchors.centerIn: parent
        text: (type === false) ? "<" : ">"
        font.pointSize: 15
        color: palette.text
    }

    MouseArea {
        id: area
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            scale.start();
        }
        onExited: {
            unScale.start();
        }
        onClicked: { top.pressed() }
        cursorShape: Qt.PointingHandCursor
    }

    SequentialAnimation {
        id: scale
        loops: 1
        PropertyAnimation {
            target: top
            properties: "scale"
            from: 1.0
            to: 1.2
            duration: app.duration/3
        }
    }

    SequentialAnimation {
        id: unScale
        loops: 1
        PropertyAnimation {
            target: top
            properties: "scale"
            from: 1.2
            to: 1.0
            duration: app.duration/3
        }
    }
}
