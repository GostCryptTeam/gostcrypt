import QtQuick 2.0

Rectangle {
    property int _width: 100
    property int _height: 100
    width: _width; height: _height

    color: "red"

    MouseArea {
        anchors.fill: parent
        onClicked: console.log("Button clicked!")
    }
}
