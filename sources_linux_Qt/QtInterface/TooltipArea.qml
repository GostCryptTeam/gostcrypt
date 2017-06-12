import QtQuick 2.4
import QtQuick.Controls 2.0
import QtQuick.Controls.Private 1.0

MouseArea {
    id: _root
    property string text: ""

    anchors.fill: parent
    hoverEnabled: _root.enabled

    onExited: Tooltip.hideText()
    onCanceled: Tooltip.hideText()

    Timer {
        interval: 1000
        running: _root.enabled && _root.containsMouse && _root.text.length
        onTriggered: Tooltip.showText(_root, Qt.point(_root.mouseX, _root.mouseY), _root.text)
    }
}
