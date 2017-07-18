import QtQuick 2.7
import QtQuick.Controls 1.4
import "../" as UI

Item {
    id: top
    property int type: 0
    x: 20
    Grid {
        columns: 1
        rows: 3
        height:parent.height
        width: parent.width
        rowSpacing: 40
        ExclusiveGroup { id: groupRadio }
        UI.GSCheckBox {
            id: encryptedFile
            text_: qsTr("Create an encrypted file container")
            checked: true
            height: 30
            exclusiveGroup :groupRadio
            onCheckedChanged: {
                if(encryptedFile.checked === true) {
                    top.type = 0
                }
            }
            Text {
                id:description
                width: top.width-60
                text: qsTr("Creates a virtual encrypted disk within a file. Recomended for "+
                "inexperienced users.<br> <font color='#719c24'><a href=\"#\">More Information</a></font>")
                anchors.top:encryptedFile.bottom
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

        UI.GSCheckBox {
            id: nonSystem
            text_: qsTr("Encrypt a non-system partition/drive")
            checked: false
            height: 40
            exclusiveGroup :groupRadio
            onCheckedChanged: {
                if(encryptedFile.checked === true) {
                    top.type = 1
                }
            }
            Text {
                id:description2
                width: top.width-60
                text: qsTr("Encrypts a non-system partition on any internal or"+
                      " external driv (e.g. a flash drive). Optionnaly, creates "+
                      "a hidden volume.")
                anchors.top:nonSystem.bottom
                color: palette.text
                leftPadding: 40
                wrapMode: Text.WordWrap
            }
        }

        UI.GSCheckBox {
            id: system
            text_: qsTr("Encrypt the system partition or entire system drive")
            checked: false
            height: 40
            enabled: false
            exclusiveGroup :groupRadio
            onCheckedChanged: {
                if(system.checked === true) {
                    top.type = 2
                }
            }
            Text {
                id:description3
                width: top.width-60
                text: qsTr( "Encrypts the partition/drive where Windows is installed. "+
                            "Anyone who wants to gain access and use the system, read and "+
                            "write files, etc., will need to enter the correct "+
                            "password each time before Windows boots. Optionnaly, creates a "+
                            "hidden system.<br><font color='#719c24'><a href=\"#\">More about system encryption</a></font>")
                anchors.top:system.bottom
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
        if(value === 0)
            encryptedFile.checked = true
        else if (value === 1)
            nonSystem.checked = true
        else
            system.checked = true
    }
}
