import QtQuick 2.0
import QtQml 2.2

Item {
    id: root

    width: size
    height: size

    property int size: 200
    property real origin: 0
    property real percent: 0 //percent
    property real lineWidth: 5
    property color colorLine: palette.blue
    property color colorBackground: palette.darkInput
    property string title: ""

    //Animation
    property alias animate: percentAnimate.enabled

    onPercentChanged: {
        canvas.requestPaint()
    }

    Behavior on percent {
        id: percentAnimate
        enabled: true
        NumberAnimation {
            duration: app.duration
            easing.type: Easing.InOutCubic
            onRunningChanged: {
                console.log(percent)
            }
        }
    }

    Canvas {
        id: canvas
        anchors.fill: parent
        rotation: -90

        onPaint: {
            var ctx = getContext("2d");
            var x = width/2, y = height/2;
            var start = 0, end = Math.PI * (parent.percent/50);
            ctx.reset();
            //background
            ctx.beginPath();
            ctx.arc(x, y, x - parent.lineWidth / 2, 0, Math.PI * 2, false)
            ctx.lineWidth = root.lineWidth
            ctx.strokeStyle = root.colorBackground
            ctx.stroke()
            //bar
            ctx.beginPath();
            ctx.arc(x,y, x - parent.lineWidth/2, start, end, false);
            ctx.lineWidth = root.lineWidth;
            ctx.strokeStyle = root.colorLine;
            ctx.stroke();
        }
    }

    Text {
        text: Math.floor(parent.percent) + "%"
        anchors.centerIn: parent
        color: palette.text
    }

}
