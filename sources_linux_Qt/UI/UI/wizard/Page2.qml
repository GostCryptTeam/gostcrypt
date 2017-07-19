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
        rowSpacing: 40
        ExclusiveGroup { id: groupRadio }
        UI.GSCheckBox {
            id: stdGVol
            text_: qsTr("Standard GostCrypt Volume")
            checked: true
            height: 30
            exclusiveGroup :groupRadio
            onCheckedChanged: {
                if(stdGVol.checked === true) {
                    top.type = 0
                }
            }
            Text {
                id:description
                width: top.width-60
                font.pixelSize: 12
                text: qsTr("Select this option if you want to create a normal GostCrypt volume.")
                anchors.top:stdGVol.bottom
                color: palette.text
                leftPadding: 40
                wrapMode: Text.WordWrap
            }
        }

        UI.GSCheckBox {
            id: hiddenVol
            text_: qsTr("Hidden GostCrypt volume")
            checked: false
            height: 40
            exclusiveGroup :groupRadio
            onCheckedChanged: {
                if(hiddenVol.checked === true) {
                    top.type = 1
                }
            }
            Text {
                id:description3
                width: top.width-60
                font.pixelSize: 12
                text: qsTr( "It may happen that you are forced by somebody to reveal the"+
                            " password of an encrypted volume. There are many situations where"+
                            " you cannot refuse to reveal the password (for example, due to extorsion) "+
                            ". Using a so-called hidden volume allows you to solve such situations without revealing the password of your volume."+
                            "<br><font color='#719c24'><a href=\"#\">More information about hidden volumes</a></font>")
                anchors.top:hiddenVol.bottom
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
            hiddenVol.checked = true
        else
            stdGVol.checked = true
    }
}
