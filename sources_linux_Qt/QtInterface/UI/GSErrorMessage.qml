import QtQuick 2.7

Item {
    anchors.fill: parent
    id:errorLayer
    x: 0
    y: 0
    width: parent.width
    height: parent.height
    property string title: ""
    property string contentError: ""
    property bool isVisible: false

    Behavior on opacity {
        NumberAnimation {
            id: animation
            duration: app.duration;
            easing.type: Easing.OutQuad;
            onRunningChanged: {
                if(!animation.running && errorLayer.isVisible == false) {
                    errorLayer.visible = false
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            console.log("Clic catch");
        }
    }

    Rectangle {
        anchors.fill: errorLayer
        color: "#000000"
        opacity: 0.9

    }

    Text {
        id: titleText
        text: title
        y: 50
        width: errorLayer.width
        //width: errorLayer.width
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pointSize: 24
        color: "#719c24"
    }

    Text {
        id: contentText
        text: contentError
        y: 100
        width: errorLayer.width
        //width: errorLayer.width
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pointSize: 14
        color: "#ffffff"
        leftPadding: errorLayer.width/4
        rightPadding: errorLayer.width/4
        bottomPadding: 20
    }

    GSButtonBordered {
        id: exitButton
        anchors.horizontalCenter: errorLayer.horizontalCenter
        anchors.top: contentText.bottom
        text: qsTr("OK")
        color_: palette.blue
        onClicked: {
            closeErrorMessage();
        }
    }
}
