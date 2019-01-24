import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

CheckBox {
    id: box
    property string text_
    property int sizeText: 11
    property int leftPadding_: 5
    property int size_: 30
    property color textColor: palette.text

    style: CheckBoxStyle {
        label: Text {
            leftPadding: leftPadding_
            font.pointSize: sizeText
            color: textColor
            text: text_ + Translation.tr
            wrapMode: Text.WordWrap
        }
        indicator: Rectangle {
            implicitWidth: size_
            implicitHeight: size_
            radius: size_/2
            border.color: control.activeFocus ? palette.green : palette.darkSecond
            border.width: 1
            color: palette.bkCheckBox
            Rectangle {
                visible: control.checked
                color: palette.green
                border.color: palette.border
                radius: 15
                anchors.margins: 7
                anchors.fill: parent
            }
        }
    }
}
