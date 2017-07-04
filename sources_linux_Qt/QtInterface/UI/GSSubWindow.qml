import QtQuick 2.7
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.4
import QtQml 2.0 as QML

Item {
    id:subWindow_
    x: 0
    y: 51
    property string title: ""
    property string w
    property string name: ""
    property var object
    property var heightSubWindow
    property bool isOpen
    property variant parameter: {"name" : "", "value" : ""}

    //Black background
    Rectangle {
        width: app.width
        height:app.height
        x:0
        y:-parent.y
        color: palette.shadow
        opacity: 0.7
        MouseArea {
            anchors.fill: parent
            onClicked: {
            }
        }
    }

    /*Rectangle {
        id: closeButton
        x: parent.width - 100
        y: containerSub.y + 15
        width: 34
        height: 34
        color: palette.darkSecond
        radius: 4
        z: 1
        visible: true
        antialiasing: true
        border.color: palette.blue
        border.width: 2
        Image {
            id: closeButton_Image
            anchors.centerIn: parent
            x: 0
            y: 0
            width: 30
            height: 30
            enabled: false
            smooth: false
            source: "ressource/closeSubWindow.png"
        }
        MouseArea {
            id: closeButton_Area
            //close the subwindow and delete its values
            anchors.fill: parent
            hoverEnabled: true
            z:2
            onClicked: {
                catchClose()
                // ConnectSignals.connectEndSudo();
            }
        }
    }*/

    states: [
        State {
            name: "hover"
            when: closeButton_Area.containsMouse && !closeButton_Area.pressed
            PropertyChanges {
                target: closeButton
                border.color: palette.blueHover
            }
        },
        State {
            name: "pressed"
            when: closeButton_Area.pressed
            PropertyChanges {
                target: closeButton
                border.color: palette.bluePress
            }
        },
        State {
            name: "exit"
            when: !closeButton_Area.containsMouse
            PropertyChanges {
                target: closeButton
                border.color: palette.blue
            }
        }
    ]

    RectangularGlow {
        id: effect
        anchors.fill: containerSub
        glowRadius: 10
        spread: 0.2
        color: palette.shadow
        cornerRadius: containerSub.radius + glowRadius
    }

    Rectangle {
        id: containerSub
        color: "transparent"//palette.darkSecond
        anchors.centerIn: parent
        width: parent.width - 80
        height: heightSubWindow
        y: parent.y
        radius: 0
        Behavior on height { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad; } }
        Rectangle {
            width:parent.width
            x:0
            y:0
            height: 70
            color: palette.darkThird
        }
        Rectangle {
            width:parent.width
            x:0
            y:70
            height: parent.height-70
            color: palette.dark
        }
        Text {
            topPadding: 5
            anchors.horizontalCenter: parent.horizontalCenter
            y: 5
            text: "<font color=\"#719c24\"><a href=\"home\">"+qsTr("Home")+"</font></a> > "+ name
            color: palette.text
            font.pixelSize: 12

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
            onLinkActivated: {
                catchClose()
            }
        }
        Text {
            topPadding: 20
            text: title
            font.family: "Helvetica"
            font.pointSize: 17
            color: palette.green
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            y: containerSub.y + 10
        }
        Rectangle {
            id: close
            width:40
            height:40
            color: palette.border
            radius: 5
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.top: parent.top
            anchors.topMargin: 10
            Text{
                color: palette.text
                text: "×"
                anchors.centerIn: parent
                font.pointSize: 20
            }
            MouseArea{
                width: parent.width
                height: parent.height
                hoverEnabled: true
                onEntered: close.color = palette.darkInput
                onExited: close.color = palette.border
                onClicked: catchClose()
            }
        }
    }

    //Content
    ScrollView {
        id: scrollArea
        x: containerSub.x
        y: 30
        width: containerSub.width
        height: containerSub.height-80
    }

    Behavior on opacity { NumberAnimation { id: anim_; duration: app.duration/2; easing.type: Easing.OutQuad; onRunningChanged: {
                if(!anim_.running && subWindow_.isOpen == false) {
                    subWindow_.visible = false
                }
            } } }

    Loader {
        id: loader
        anchors.fill: scrollArea
        //asynchronous: true // KNOWN ISSUE TO BE FIXED!!
        onLoaded: {
            applyParameter()
        }
    }

    //Load the right QML Form
    function loadForm() {
        loader.setSource("");
        var component = Qt.createComponent(w);
        var parent = scrollArea;
        if (component.status == QML.Component.Ready) {
            console.log("Chargement de la forme " + w);
            loader.setSource(w);
        }else if (component.status == Component.Error) {
            // Error Handling
            console.log("Error loading component:", component.errorString());
        }
    }

    function applyParameter() {
        switch(parameter.name)
        {
        case "dropVolume":
            console.log("[Debug] : Opening a volume from a drag action.")
            loader.item.initDrag(parameter)
            break;
        }
    }

    function catchClose() { //todo : code d'erreur ?
        //effacer le contenu du loader
        loader.setSource("");
        //fermer la subwindow
        subWindow_.parameter = ""
        subWindow_.opacity = 0.0
        subWindow_.isOpen = false
        heightSubWindow = 429
    }

    function changeSubWindowHeight(value) {
        heightSubWindow = value;
        loader.item.y = subWindow.y
    }
}
