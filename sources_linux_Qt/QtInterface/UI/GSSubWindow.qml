import QtQuick 2.7
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.4
//import "OpenVolume.js" as OpenVolume
import QtQml 2.0 as QML

Item {
    id:subWindow_
    x: 0
    y: 51
    width: 790
    height: 429
    property string title: "title"
    property string w
    property var object

    Rectangle {
        //anchors.fill: app
        width: app.width
        height:app.height
        x:0
        y:-parent.y
        color: '#000000'
        opacity: 0.5

        MouseArea {
            anchors.fill: parent
        }
    }

    Rectangle {
        id: closeButton
        x: parent.width - 100
        y: -17
        width: 34
        height: 34
        color: "#232323"
        radius: 4
        z: 1
        visible: true
        antialiasing: true
        border.color: "#2f88a7"
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
            source: "../ressource/closeSubWindow.png"
        }
        MouseArea {
            id: closeButton_Area
            //close the subwindow and delete its values
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                //supprimer l'objet
                //object.destroy()
                //fermer la subwindow
                subWindow_.opacity = 0.0
            }
        }
    }

    states: [
        State {
            name: "hover"
            when: closeButton_Area.containsMouse && !closeButton_Area.pressed
            PropertyChanges {
                target: closeButton
                border.color: "#42bfea"
            }
        },
        State {
            name: "pressed"
            when: closeButton_Area.pressed
            PropertyChanges {
                target: closeButton
                border.color: "#74d1f2"
            }
        },
        State {
            name: "exit"
            when: !closeButton_Area.containsMouse
            PropertyChanges {
                target: closeButton
                border.color: "#2f88a7"
            }
        }
    ]

    Rectangle {
        id: rectangle1
        y: parent.y
        anchors.fill: parent
        color: "#2a2a2a"
    }

    RectangularGlow {
        id: effect
        anchors.fill: rect2
        glowRadius: 80
        spread: 0.2
        color: "#000000"
        cornerRadius: rect2.radius + glowRadius
    }

    Rectangle {
        id: rect2
        color: "#2a2a2a"
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        y: parent.y
        radius: 0

    }

    Text {
        text: title
        font.family: "Helvetica"
        font.pointSize: 24
        color: "#719c24"
        leftPadding: 50
        topPadding: 10
    }

    //Content
    ScrollView {
        id: scrollArea
        x: 50
        y: 70
        width: parent.width - 100
        height: parent.height - 100
    }

    Behavior on opacity { NumberAnimation { id: anim_; duration: app.duration; easing.type: Easing.OutQuad; onRunningChanged: {
                if(!anim_.running && subWindow_.opacity == 0.0) {
                       subWindow_.visible = false
                }
            } } }

    Loader {
        id: loader
        anchors.fill: scrollArea
        //asynchronous: true // KNOWN ISSUE TO BE FIXED!!
    }

    Connections {
        target: loader.item
        onMountVolume: {
            ConnectSignals.connectReceiveMount(path,password);
        }
    }

    //Load the right QML Form
    function loadForm() {
        var component = Qt.createComponent(w);
        var parent = scrollArea;
        if (component.status == QML.Component.Ready) {
            loader.setSource(w);
        }else if (component.status == Component.Error) {
            // Error Handling
            console.log("Error loading component:", component.errorString());
        }
    }

    function catchClose() { //todo : code d'erreur ?
        //fermer la subwindow
        subWindow_.opacity = 0.0
    }
}
