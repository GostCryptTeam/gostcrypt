import QtQuick 2.7
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.4
import QtQml 2.0 as QML

Item {
    id:subWindow_
    property alias loadedItem : loader.item
    property string title: ""
    property string w
    property string name: ""
    property var object
    property bool isOpen
    property variant parameter: {"name" : "", "value" : "", "type": false}

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
    }


    //Black background
    Rectangle {
        width: app.width
        height: app.height
        x: -parent.x
        y: -parent.y
        color: palette.shadow
        opacity: 0.7
        MouseArea {
            anchors.fill: parent
            onClicked: {
            }
        }
    }

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


    //Stylized subwindow
    Rectangle {
        id: containerSubStylized
        visible: (parameter.type === true) ? true : false
        width: 608
        anchors.centerIn: parent
        height: parent.height
        radius: 10
        y: 40
        color: "transparent";
        Rectangle {
            id: imgWizard
            color: "transparent";
            width: 608
            height: 151
            Image {
                source: "ressource/wizard.png"
                Rectangle {
                    id: contentTitle
                    x: 195
                    y: 108
                    width: 403
                    height: 35
                    color: "transparent"
                    Text {
                        id: titleWizard_
                        anchors.centerIn: contentTitle
                        text: title + Translation.tr
                        font.family: "Helvetica"
                        font.pointSize: 14
                        color: palette.text
                    }

                }

            }
            Text {
                topPadding: 5
                anchors.horizontalCenter: parent.horizontalCenter
                y: 20
                text: "<font color=\""+palette.blue+"\"><a href=\"home\">"+qsTr("Home") + Translation.tr+"</font></a> > "+ name + Translation.tr
                color: palette.blue
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
        }
        Rectangle {
            id: closeWizard
            width:40
            height:40
            color: palette.grayWizard
            radius: 5
            anchors.right: parent.right
            anchors.rightMargin: 10
            y: 50
            anchors.topMargin: 10
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
                onEntered: closeWizard.color = palette.darkInput
                onExited: closeWizard.color = palette.grayWizard
                onClicked: catchClose()
            }
        }
        Rectangle {
            id: contentWizard
            color: palette.grayWizard
            width: parent.width
            height: 270
            anchors.top: imgWizard.bottom
            Rectangle {
                id: bottomBar
                width: parent.width
                height: 50
                anchors.bottom: parent.bottom
                color: palette.grayWizardDark
            }
        }

    }

    //Normal subwindow
    Rectangle {
        id: containerSub
        color: palette.shadow
        anchors.centerIn: parent
        width: parent.width - 80
        height: parent.height
        y: 40
        radius: 0
        visible: (parameter.type === 0 || parameter.type === undefined) ? true : false
        Behavior on height { NumberAnimation { duration: app.duration/3; easing.type: Easing.OutQuad; } }
        Rectangle {
            width:parent.width-2
            x:1
            y:1
            height: 70
            color: palette.darkThird
        }
        Rectangle {
            width:parent.width-2
            x:1
            y:70
            height: parent.height-71
            color: palette.dark
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
                font.pointSize: 15
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
        Text {
                topPadding: 5
                anchors.horizontalCenter: parent.horizontalCenter
                y: 5
                text: "<font color=\"#719c24\"><a href=\"home\">"+qsTr("Home") + Translation.tr+"</font></a> > "+ name + Translation.tr
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
                id: title_
                width: app.width
                height: 30
                text: title + Translation.tr
                font.family: "Helvetica"
                font.pointSize: 17
                color: palette.green
                y: containerSub.y + 10
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                topPadding: 20
            }
    }


    //Content
    ScrollView {
        id: scrollArea
        x: (parameter.type === true) ?  containerSubStylized.x : containerSub.x
        y: (parameter.type === true) ?  contentWizard.y : 30

        width: (parameter.type === true) ?  containerSubStylized.width : containerSub.width
        height: (parameter.type === true) ?  containerSubStylized.height : containerSub.height-80

    }

    Behavior on opacity {
        NumberAnimation {
            id: anim_;
            duration: app.duration/2;
            easing.type: Easing.OutQuad;
            onRunningChanged: {
                if(!anim_.running && subWindow_.isOpen == false) {
                    subWindow_.visible = false
                }
            }
        }
    }

    Loader {
        id: loader
        anchors.fill: scrollArea
        //asynchronous: true // KNOWN ISSUE TO BE FIXED!!
        onLoaded: {
            applyParameter()
        }
    }

    //Load the right QML Form
    function loadForm(height, title) {
        loader.setSource("");
        var parent = scrollArea;
        subWindow_.title = title;
        loader.setSource(w);
    }

    function applyParameter() {

        changeSubWindowTitle();
        changeSubWindowHeight();

        switch(parameter.name)
        {
        case "dropVolume":
            loader.item.initDrag(parameter)
            break;
        case "favorite-volumes":
            loader.item.volumePath = parameter.value
            break;
        case "volume-settings":
        case "volume-backup":
            loader.item.volumePath = parameter.value
            break;
        }
    }

    function catchClose() { //todo : code d'erreur ?
        //effacer le contenu du loader
        loader.setSource("");
        //fermer la subwindow
        subWindow_.opacity = 0.0
        subWindow_.isOpen = false
    }

    function changeSubWindowHeight(value) {
        if(parameter.type === true)
            loader.item.y = 10
        else
            loader.item.y = 40
    }

    function changeSubWindowTitle() {
        title_.text = subWindow_.title
    }

    function getLoader()
    {
        return loader;
    }

}
