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
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("GostCrypt 2.0") + Translation.tr
            color: palette.text
            x:20
            font.pixelSize: 13
        }

        Image {
            id: link
            anchors.verticalCenter: parent.verticalCenter
            x: text.x + text.width + 10
            fillMode: Image.PreserveAspectFit
            source: "ressource/link.png"
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
            anchors.centerIn: parent
            font.pointSize: 15
        }
        MouseArea{
            width: parent.width
            height: parent.height
            hoverEnabled: true
            onEntered: appclose.color=palette.darkThird
            onExited: appclose.color=palette.border
            onClicked: app.appQuit();
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
