import QtQuick 2.7
import QtQuick.Window 2.2
import gostcrypt.modelclass 1.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import "LoadVolume.js" as LoadVol

Window {
    id: app
    title: qsTr("GostCrypt 2.0")
    visible: true
    minimumWidth: 790
    minimumHeight: 530
    maximumHeight: minimumHeight
    maximumWidth: minimumWidth

    width: 790
    height: 530
    color: "#303030"

    //properties
    property bool isPC: true
    property bool isLandscape: false
    property int duration: 500
    property int menuWidth: 150
    property int widthOfSeizure: 15
    property bool shown: false
    property real rotate

    //colors used in the UI
    QtObject {
        id: palette
        property color dark: "#303030"
        property color darkSecond: "#2a2a2a"
        property color darkThirdb: "#272727"
        property color text: "#e1e1e1"
        property color textLight: "#e1e1e1"
        property color green: "#FFEB3B"
        property color blue: "#212121"
    }

    //Auto-detect screen type
    onWidthChanged: {
        if( width > height ) {
            app.isLandscape = true
        } else {
            app.isLandscape = false
        }
    }

    GSMainForm {
        id: mainWindow
        Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }

    Row {
        id: rowWindow
        spacing: 70
        anchors.horizontalCenter: parent.horizontalCenter
        topPadding: 18
        GSButtonGreen_icon {
            text: qsTr("Mount Volume")
            onClicked: {
                testt.setText(text)
                subWindow.opacity = (subWindow.opacity == 1.0) ? 0.0 : 1.0
                subWindow.visible = (subWindow.visible == true) ? false : true
                if(subWindow.visible == true) {
                    subWindow.w = "../dialogs/GSOpenVolume.qml"
                    //OpenVolume.createForm('Open a GostCrypt volume');
                    subWindow.title = 'Open a GostCrypt volume'
                    subWindow.loadForm()
                }
            }
        }
        GSButtonBlue_icon {
            text: qsTr("Create Volume")
            onClicked: {
                testt.setText(text)
            }
        }
        Behavior on anchors.horizontalCenterOffset { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }
    Row {
        id: rowBottom
        spacing: 22
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 28
        GSButtonGreen {
            text: qsTr("Auto Mount Device")
        }
        GSButtonGreen {
            text: qsTr("Dismount All")
        }
        GSButtonGreen {
            text: qsTr("Volume Tools")
        }
        Behavior on anchors.horizontalCenterOffset { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }

    GSMenu{
        id: gs_Menu
        height: app.height
        Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }

    }

    function toggleMenu() {
        gs_Menu.x = app.shown ? -gs_Menu.width : 0
        rowWindow.anchors.horizontalCenterOffset = app.shown ? 0 : 75
        rowBottom.anchors.horizontalCenterOffset = app.shown ? 0 : 75
        mainWindow.x = app.shown ? 0 : 75
        menuButton.x = app.shown ? 20 : 165
        volumeListElement.x = app.shown ? 98 : 173
        app.shown = !app.shown
        menuButton.value = !menuButton.value;
    }

    GSMenuButton {
        id: menuButton
        x: 20
        y: 15
        value: rotate
        Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
        onXChanged: {
            rotate = (1-Math.abs(gs_Menu.x/gs_Menu.width))
            menuButton.value = rotate
        }
    }

    ModelClass{
        id: testt
    }


    Item {
        id: volumeListElement
        x: 98
        y: 111
        width: 594
        height: 303
        GSVolumeItem {
            id:volumeDelegate
        }

        ListModel {
            id: listOfVolumes
        }

        ListView {
            id: listViewVolumes
            anchors.fill: parent;
            anchors.margins: 5
            model: listOfVolumes
            delegate: volumeDelegate
            focus: true
        }
        Behavior on x { NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad } }
    }


    GSSubWindow {
        id: subWindow
        width:app.width
        height:429
        visible: false
        opacity: 0.0
    }


    /* Button {
        text: "Test dialog"
        onClicked: {
            var component = Qt.createComponent("../dialogs/GSOpenVolume.qml");
            var window    = component.createObject(app)
            window.show()
        }
    }*/
}
