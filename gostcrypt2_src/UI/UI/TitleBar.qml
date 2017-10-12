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

    Rectangle {
        property bool checked: false
        id: notifications
        y:0
        height: height_
        width: height_
        anchors.right: appminimize.left
        x: 20
        color: palette.border

        Image {
            id: notifications_image
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            source: "ressource/notification.png"
            height: 20
            width: 20
        }

        Rectangle {
            id: unread_notifs
            //visible: false
            width: 10
            height: 10
            color: palette.green
            radius: 10
            x:20
            y:25
            border.width: 1
            border.color: "#1a1a1a"
        }

        MouseArea {
            id: area
            width: parent.width
            height: parent.height
            hoverEnabled: true
        }

        states: [
            State {
                name: "hover"
                when: area.containsMouse && !area.pressed
                PropertyChanges {
                    target: notifications
                    color : {
                        notifications.color=palette.green
                    }
                }
            },
            State {
                name: "pressed"
                when: area.pressed
                PropertyChanges {
                    target: notifications
                    color : {
                        title.changeNotif(!notifications.checked)
                    }
                }
            },
            State {
                name: "exit"
                when: !area.containsMouse
                PropertyChanges {
                    target: notifications
                    color : {
                        if(notifications.checked)
                            notifications.color=palette.green
                        else
                            notifications.color=palette.border
                    }
                }
            }
        ]

        transitions: Transition {
            ColorAnimation { duration:app.duration/2 }
        }

    }

    function changeNotif(bool) {
        if(!notifications.checked) {
            notifications.color=palette.green
            notifs.opacity = 1.0
            notifs.printNotification();
            notifs.drawNotification();
        }
        else {
           notifications.color=palette.border
           notifs.opacity = 0.0
        }
        notifications.checked = bool
        bk_notifs.enabled = false
    }
}
