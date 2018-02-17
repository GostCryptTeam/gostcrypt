import QtQuick 2.7
import "../" as UI

Item {
    id: top
    property variant used: [app.hashs[0], app.algorithms[0]]
    property int type: 0

    Row {
        id: content
        spacing: 20
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 20
        Item {
            width: 250
            height: 150
            Text {
                id: text
                anchors.horizontalCenter: parent.horizontalCenter
                y:5
                font.pointSize: 11
                font.family: "Helvetica"
                color: palette.text
                text: qsTr("Encryption Algorithm :") + Translation.tr
            }

            UI.CustomComboBox {
                id: algo
                model: app.algorithms;
                anchors.top: text.bottom
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width -20
                onActivated: {
                    used[0] = algo.textAt(algo.currentIndex)
                }

            }
            Row {
                id: buttons
                spacing: 20
                anchors.top: algo.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 10

                UI.ButtonBordered {
                    id: testbutton
                    color_: palette.green
                    text: qsTr("Test") + Translation.tr
                    width: 70
                    height: 40
                    onClicked: {
                        //TODO
                    }
                }

                UI.ButtonBordered {
                    id: benchmarkButton
                    color_: palette.green
                    text: qsTr("Benchmark") + Translation.tr
                    width: 90
                    height: 40
                    onClicked: {
                        //TODO
                    }
                }
            }

            Text {
                id: description
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WordWrap
                width: parent.width - 20
                text: "Algo description here rgergergerg qrgq rgqr qerg qrg qrfgq rgerg qrg qerg r";
                color: palette.text
                font.pointSize: 10
                font.family: "Helvetica"
                anchors.top: buttons.bottom
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }

        }
        Item {
            width: 250
            height: 150

            Text {
                id: text2
                anchors.horizontalCenter: parent.horizontalCenter
                y:5
                font.pointSize: 11
                font.family: "Helvetica"
                color: palette.text
                text: qsTr("Hash Algorithm") + Translation.tr
            }

            UI.CustomComboBox {
                id: hash
                model: app.hashs;
                anchors.top: text2.bottom
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width -20
                onActivated: {
                    used[1] = hash.textAt(hash.currentIndex)
                }
            }

            Text {
                id: link
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WordWrap
                width: parent.width - 20
                text: qsTr("<font color='#719c24'><a href='#'>Information on hash algorithms</a></font>") + Translation.tr
                color: palette.text
                font.family: "Helvetica"
                anchors.top: hash.bottom
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 12
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
        }
    }
}
