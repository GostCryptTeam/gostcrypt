import QtQuick 2.0
import QtQuick.Window 2.2

Rectangle{

    property int height_
    id:titlebar
    width: parent.width
    color: palette.border
    anchors.bottomMargin: 1

    Rectangle {
            anchors.top: titlebar.bottom
            height: 1
            width: parent.width
            color: palette.border
    }

    Rectangle {
        id: logoTop
        x:10
        y:10
        //width:120
        height: 20
        Image {
            fillMode: Image.PreserveAspectFit
            source: "ressource/logo_gostcrypt.png"
            height:20
        }

        Text {
            id: text
            text: qsTr("GostCrypt 2.0")
            color: palette.text
            x:20
            font.pixelSize: 13
        }

        Image {
            id: link
            y: -2
            x: 110
            fillMode: Image.PreserveAspectFit
            source: "ressource/link.png"
            height:22
            width:22
            MouseArea {
                anchors.fill: link
                hoverEnabled: true;
                onClicked: Qt.openUrlExternally("http://www.gostcrypt.org/")
                cursorShape: Qt.PointingHandCursor
            }
        }
    }


    Rectangle{
        id:appclose
        height: height_
        y:0
        width: height_
        anchors.right: parent.right
        color: palette.border
        Text{
            color: palette.text
            text: "×"
            anchors.horizontalCenter: parent.horizontalCenter
            font.pointSize: 20
        }
        MouseArea{
            width: parent.width
            height: parent.height
            hoverEnabled: true
            onEntered: appclose.color=palette.darkThird
            onExited: appclose.color=palette.border
            onClicked: Qt.quit()
        }
    }

    Rectangle{
        id:appminimize
        height: height_
        width: height_
        y:0
        anchors.right: appclose.left
        color: palette.border
        Text{
            color: palette.text
            text: '_'
            anchors.horizontalCenter: parent.horizontalCenter
            font.pointSize: 15
        }
        MouseArea{
            width: parent.width
            height: parent.height
            hoverEnabled: true
            onEntered: appminimize.color=palette.darkThird
            onExited: appminimize.color=palette.border
            onClicked: app.visibility = Window.Minimized
        }
    }
}
