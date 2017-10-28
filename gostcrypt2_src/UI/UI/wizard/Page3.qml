import QtQuick 2.7
import QtQuick.Controls 1.4
import "../" as UI

Item {
    id: top
    x: 20
    property int type: 0
    Grid {
        y: 20
        columns: 1
        rows: 2
        height:parent.height
        width: parent.width
        rowSpacing: 60
        ExclusiveGroup { id: groupRadio }
        UI.GSCheckBox {
            id: normal
            text_: qsTr("Normal Mode")
            checked: true
            height: 30
            exclusiveGroup :groupRadio
            onCheckedChanged: {
                if(normal.checked === true) {
                    top.type = 0
                }
            }
            Text {
                id:description
                width: top.width
                font.pixelSize: 12
                text: qsTr("If you select this option, the wizard will first help you create a normal GostCrypt"+
                      "volume and then a hidden GostCrypt volume within it. Inexperienced users should always select this option.") + Translation.tr
                anchors.top:normal.bottom
                color: palette.text
                wrapMode: Text.WordWrap
            }
        }

        UI.GSCheckBox {
            id: direct
            text_: qsTr("Direct Mode")
            checked: false
            height: 40
            exclusiveGroup :groupRadio
            onCheckedChanged: {
                if(direct.checked === true) {
                    top.type = 1
                }
            }
            Text {
                id:description3
                width: top.width-60
                font.pixelSize: 12
                text: qsTr( "If you select this option, you will create a hidden volume within"+
                            " an existing GostCrypt volume. It will be assumed that you have already created a GostCrypt volume"+
                            " that is suitable to host the hidden volume." ) + Translation.tr
                anchors.top:direct.bottom
                color: palette.text
                leftPadding: 40
                wrapMode: Text.WordWrap
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
        }
    }


    function setType(value)
    {
        type = value
        if(value === 1)
            direct.checked = true
        else
            normal.checked = true
    }
}
