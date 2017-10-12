import QtQuick 2.7
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2

Item {
    id: top
    width: 250
    height:292
    property int nb_notification: 0
    property var notifications: []
    property int a: 5

    function printNotification() {
        if(nb_notification > 1)
        {
            notif_title.text = nb_notification + " " + qsTr("NOTIFICATIONS") + Translation.tr;
        }else
        {
           notif_title.text = qsTr("NO NOTIFICATION") + Translation.tr;
        }
        for(var i=0;i<2;i++) addNotification(i, "test"+i, Math.floor(Math.random()*100));
    }

    function addNotification(id, name, percent) {
        notifications.push([id,name,percent]);
    }


    function drawNotification() {
        //Removing all the delegate items to clear the listOfNotifications
        //and run the animation when the user opens the Notification box
        listOfNotifications.clear();

        for(var notif in notifications)
        {
            listOfNotifications.append(
                        {
                            Notif_id: Number(notifications[notif][0]),
                            Notif_name: String(notifications[notif][1]),
                            Notif_percent: String(notifications[notif][2])
                        });
        }
    }

    function updateNotification(id, percent)
    {
        for(var i = 0; i < listOfNotifications.count; ++i)
            if(listOfNotifications.get(i).Notif_id === id)
                listOfNotifications.setProperty(i, "Notif_percent", percent)
    }

    Rectangle {
        id: form
        y: 11
        color: palette.border
        width:parent.width
        height: parent.height
        Text {
            id: notif_title
            text: ""
            anchors.horizontalCenter: parent.horizontalCenter
            y: 0
            height: 30
            color: palette.text
            lineHeight: 30
            verticalAlignment: Text.AlignVCenter
        }
        border.width: 1
        border.color: "#1a1a1a"
    }

    Image {
        fillMode: Image.PreserveAspectFit
        source: "ressource/triangle_notif.png"
        height:12
        x: 177
        y: 0
    }

    Rectangle {
        id: form2
        y: 42
        color: palette.darkSecond
        width: 248
        x: 1
        height: parent.height -31

        Component {
            id: notification_delegate
            Item {
                property int id: Notif_id
                width: 248; height: 70
                Rectangle { //body
                    id: body
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.fill: parent
                    color: "transparent"
                    width: 248

                    CircleLoadingBar {
                        id: circle
                        anchors.verticalCenter: parent.verticalCenter
                        x: 10
                        size: 50
                        colorLine: palette.green
                        percent: Notif_percent
                        title: Notif_name
                    }
                }
                Rectangle { //border
                    height:1
                    width: 248
                    anchors.bottom: parent.bottom
                    color: palette.border
                }
            }
        }

        ListView {
            id: listview
            anchors.fill: parent
            model: ListModel {
                id: listOfNotifications
                dynamicRoles: true
            }
            delegate: notification_delegate
            focus: true
            ScrollBar.vertical: ScrollBar {
                snapMode: ScrollBar.SnapOnRelease
            }
            clip: true
        }

    }

    Rectangle {
        id: form3
        y: parent.height+4
        width: 248
        x:1
        color: palette.green
        height: 7
    }

    Behavior on opacity {
        NumberAnimation {
            id: anim_;
            duration: app.duration/2;
            easing.type: Easing.OutQuad;
            onRunningChanged: {
                top.visible = true
                if(!anim_.running && top.opacity === 0.0) {
                    top.visible = false
                }
            }
        }
    }

}
