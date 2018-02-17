import QtQuick 2.7

Item {
    anchors.fill: parent
    id:errorLayer
    x: 0
    y: 0
    width: parent.width
    height: parent.height
    property string title: ""
    property bool type: false
    property string contentError: ""
    property bool isVisible: false
    property int size: 15
    property string button1Name: qsTr("Yes")
    property string button2Name: qsTr("No")
    property string callback: ""
    property variant callback_params

    Behavior on opacity {
        NumberAnimation {
            id: animation
            duration: app.duration/3;
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
        }
    }

    Rectangle {
        anchors.fill: errorLayer
        color: "#000000"
        opacity: 0.9

    }

    Text {
        id: titleText
        text: title + Translation.tr
        anchors.horizontalCenter: parent.horizontalCenter
        y: 50
        width: errorLayer.width - 50
        wrapMode: Text.WordWrap
        //width: errorLayer.width
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pointSize: 24
        color: "#719c24"
    }

    Text {
        id: contentText
        text: contentError + Translation.tr
        anchors.top: titleText.bottom
        anchors.topMargin: 10
        width: errorLayer.width
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Helvetica"
        font.pixelSize: size
        color: "#ffffff"
        leftPadding: errorLayer.width/6
        rightPadding: errorLayer.width/6
        bottomPadding: 20
        wrapMode: Text.WordWrap
    }

    Row {
        spacing: 10
        anchors.top: contentText.bottom
        anchors.horizontalCenter: errorLayer.horizontalCenter

        ButtonBordered {
            id: exitButton
            visible: (type) ? false : true
            text: qsTr("OK") + Translation.tr
            color_: palette.blue
            onClicked: {
                closeErrorMessage();
            }
        }

        ButtonBordered {
            id: button1
            visible: (type) ? true : false
            text: button1Name
            color_: palette.blue
            onClicked: {
                if(callback !== "")
                    qmlRequest(callback, callback_params)
                closeErrorMessage();
            }
        }

        ButtonBordered {
            id: button2
            visible: (type) ? true : false
            text: button2Name + Translation.tr
            color_: palette.blue
            onClicked: {
                closeErrorMessage();
            }
        }
    }


}
