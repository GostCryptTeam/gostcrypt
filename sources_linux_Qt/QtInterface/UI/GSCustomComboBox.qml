import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4

ComboBox {
    id: control
    property var model_: [""]
    model: model_

    delegate: ItemDelegate {
        width: control.width
        contentItem: Text {
            text: modelData
            color: "#719c24"
            font: control.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
        highlighted: control.highlightedIndex == index
    }

    indicator: Canvas {
        id: canvas
        x: control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        width: 12
        height: 8
        contextType: "2d"

        Connections {
            target: control
            onPressedChanged: canvas.requestPaint()
        }

        onPaint: {
            context.reset();
            context.moveTo(0, 0);
            context.lineTo(width, 0);
            context.lineTo(width / 2, height);
            context.closePath();
            context.fillStyle = control.pressed ? "#5e7d25" : "#719c24";
            context.fill();
        }
    }
    contentItem: Text {
        leftPadding: 20
        rightPadding: control.indicator.width + control.spacing

        text: control.displayText
        font: control.font
        color: control.pressed ? "#5e7d25" : "#719c24"
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        color:"transparent"
        implicitWidth: 120
        implicitHeight: 40
        border.color: control.pressed ? "#5e7d25" : "#719c24"
        border.width: control.visualFocus ? 2 : 1
        radius: 2
    }
}
