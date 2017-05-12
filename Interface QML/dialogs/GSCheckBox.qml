import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4

CheckBox {
    id: box
    property string text_

    style: CheckBoxStyle {
        label: Text {
            leftPadding: 15
            font.pointSize: 11
            color: "white"
            text: text_
        }
        indicator: Rectangle {
            implicitWidth: 30
            implicitHeight: 30
            radius: 15
            border.color: control.activeFocus ? "#719c24" : "#191919"
            border.width: 1
            color: '#191919'
            Rectangle {
                visible: control.checked
                color: "#719c24"
                border.color: "#57791b"
                radius: 15
                anchors.margins: 7
                anchors.fill: parent
            }
        }
    }
}
