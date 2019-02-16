import QtQuick 2.7
import QtQuick.Controls 2.0
Item {
    id: language
    property var childOf
    anchors.fill: childOf
    signal changeLanguage()

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        y: 30
        text: qsTr("Please choose a language :")  + Translation.tr
        font.pixelSize: 14
        color: palette.text
    }

    Component {
        id: languageDelegate
        Item {
            width:parent.width
            height: 40
            Rectangle {
                id: element
                anchors.fill:parent
                radius:2
                color: palette.darkInput
                border{
                    width:1
                    color:palette.border
                }
                Text {
                    anchors.centerIn: parent
                    text: name
                    color: palette.text
                    font.pixelSize: 14
                }
            }
            MouseArea {
                id: languageArea
                anchors.fill: element
                hoverEnabled: true
                onClicked: {
                    //change language here
                    Translation.setLanguage(code)
                    catchClose() //closing the subwindow
                }
            }
            states: [
                State {
                    name: "hover"
                    when: languageArea.containsMouse && !languageArea.pressed
                    PropertyChanges {
                        target: element
                        color : palette.darkThird
                    }
                },
                State {
                    name: "pressed"
                    when: languageArea.pressed
                },
                State {
                    name: "exit"
                    when: !languageArea.containsMouse
                    PropertyChanges {
                        target: element
                        color: palette.darkInput
                    }
                }
            ]

            transitions: Transition {
                ColorAnimation { duration:100 }
            }
        }
    }

    ListModel {
        id: modelLanguage
    }

    ListView {
        id: listOfLanguage
        width: parent.width/2
        height: parent.height-80
        y: 60
        spacing: 10
        delegate: languageDelegate
        model: modelLanguage
        focus: true
        snapMode: ScrollBar.SnapAlways
        clip: true
        anchors.horizontalCenter: parent.horizontalCenter
        boundsBehavior: Flickable.DragOverBounds
        ScrollBar.vertical:
        ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
            //policy: ScrollBar.AsNeeded
            parent: listOfLanguage.parent
            anchors.top: listOfLanguage.top
            anchors.left: listOfLanguage.right
            anchors.bottom: listOfLanguage.bottom
         }
    }
    Component.onCompleted: {
        getLanguageList()
    }

    function getLanguageList() {
        var list = Translation.getAvaibleLanguages()
        for(var i = 0; i<list.length; i+=2) {
            modelLanguage.append({"name": list[i], "code": list[i+1]})
        }
    }
}
