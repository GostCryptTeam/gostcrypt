import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4

ComboBox {
    id: control
    property var model_: [""]
    model: model_

    delegate: ItemDelegate {
        id: item
        width: control.width
        contentItem: Text {
            text: modelData
            color: palette.green
            font: control.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            anchors.fill: parent
            //color: palette.dark
            color: item.highlighted ? palette.darkSecond : palette.dark
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

    popup: Popup {
            y: control.height - 1
            width: control.width
            implicitHeight: contentItem.implicitHeight
            padding: 1

            contentItem: ListView {
                clip: true
                implicitHeight: contentHeight
                model: control.popup.visible ? control.delegateModel : null
                currentIndex: control.highlightedIndex

                ScrollIndicator.vertical: ScrollIndicator { }
            }

            background: Rectangle {
                color: palette.darkInput
                border.color: palette.darkInput
                radius: 2
            }
        }

    background: Rectangle {
        color: palette.darkInput
        implicitWidth: 120
        implicitHeight: 40
        border.color: control.pressed ? palette.darkInput : palette.darkInput
        border.width: control.visualFocus ? 2 : 1
        radius: 2
    }



}
