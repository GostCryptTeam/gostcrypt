import QtQuick 2.7
import "../" as UI

Item {
    id: top
    property variant algoHash: [Wizard.getAlgos()[0], Wizard.getHashs()[0]]
    Rectangle {
        id: algo_
        color: "transparent"
        border.width: 1
        border.color: palette.border
        radius: 5
        width: top.width-100
        height: top.height -150
        anchors.horizontalCenter: top.horizontalCenter

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            y:5
            color: palette.text
            text: qsTr("Encryption Algorithm")
        }

        UI.GSCustomComboBox {
            id: algo
            model: Wizard.getAlgos();
            x: 40
            y: 25
            width: parent.width - 170
            onActivated: {
                description.text = Wizard.getAlgosDescription(algo.currentIndex);
                algoHash[0] = algo.currentIndex
            }
        }

        UI.GSButtonBordered {
            color_: palette.green
            text: qsTr("Test")
            x: algo.x + algo.width + 20
            y: algo.y
            width: 70
            height: 40
            onClicked: {
                //TODO
            }
        }

        Text {
            id: description
            x: algo.x
            y: 80
            horizontalAlignment: Text.AlignJustify
            wrapMode: Text.WordWrap
            width: parent.width - 170 - 30
            text: Wizard.getAlgosDescription(algo.currentIndex);
            color: palette.text
            font.pixelSize: 12
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
        }

        UI.GSButtonBordered {
            color_: palette.green
            text: qsTr("Benchmark")
            x: algo.x + algo.width
            y:100
            width: 90
            onClicked: {
                //TODO
            }
        }

    }

    Rectangle {
        color: "transparent"
        border.width: 1
        border.color: palette.border
        radius: 5
        width: algo_.width
        height: 90
        y: algo_.y + algo_.height + 10
        anchors.horizontalCenter: top.horizontalCenter

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            y:5
            color: palette.text
            text: qsTr("Hash Algorithm")
        }

        UI.GSCustomComboBox {
            id: hash
            model: Wizard.getHashs();
            x: 40
            y: 35
            width: parent.width *0.5
            onActivated: {
                algoHash[1] = hash.currentIndex
            }
        }
        Text {
            id: link
            x: hash.x + hash.width + 10
            y: 45
            horizontalAlignment: Text.AlignJustify
            wrapMode: Text.WordWrap
            width: parent.width *0.5
            text: qsTr("<font color='#719c24'><a href='#'>Information on hash algorithms</a></font>")
            color: palette.text
            font.pixelSize: 12
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
        }
    }


}
