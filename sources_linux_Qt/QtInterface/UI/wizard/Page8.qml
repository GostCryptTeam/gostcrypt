import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import "../" as UI

Item {
    id: top
    property variant sizeType: [0, "MB"]
    property int type: 0
    TextField {
        id: sizeValue
        x: 10
        y: 10
        width: parent.width * 0.5
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: TextInput.AlignHCenter
        height: 40
        inputMethodHints: Qt.ImhDigitsOnly
        validator: IntValidator{}
        focus: true
        style: TextFieldStyle {
            textColor: "#e1e1e1"
            background: Rectangle {
                radius: 2
                implicitWidth: 100
                implicitHeight: 24
                border.color: "#333"
                border.width: 1
                color: palette.darkInput
            }
        }
        Keys.onPressed: {
            sizeType[0] = parseInt(sizeValue.text+event.text)
        }
    }
    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        y: 70
        spacing: 10
        ExclusiveGroup { id: groupRadio }
        UI.GSCheckBox {
            id: kB_
            text_: qsTr("KB")
            checked: false
            height: 40
            exclusiveGroup: groupRadio
            onCheckedChanged: {
                if(kB_.checked) sizeType[1] = "KB";
            }
        }
        UI.GSCheckBox {
            id: mB_
            text_: qsTr("MB")
            checked: true
            height: 40
            exclusiveGroup: groupRadio
            onCheckedChanged: {
                if(mB_.checked) sizeType[1] = "MB";
            }
        }
        UI.GSCheckBox {
            id: gB_
            text_: qsTr("GB")
            checked: false
            height: 40
            exclusiveGroup: groupRadio
            onCheckedChanged: {
                if(gB_.checked) sizeType[1] = "GB";
            }
        }
    }

    Text {
        id:freeDiskSpace
        width: top.width-120
        font.pixelSize: 15
        text: qsTr("<b>Free space on drive " + Wizard.getfreeSpace()+"</b>")
        y: 120
        x: 60
        color: palette.text
        horizontalAlignment: Text.AlignJustify
        wrapMode: Text.WordWrap
    }

    Text {
        id:description
        width: top.width-120
        font.pixelSize: 12
        text: qsTr("Please specify the size of the container you want to create.<br><br>If"
                   +" you create a dynamic (sparse-file) container, this parameter will specify its maximum possible size."
                   +"<br><br>Note that possible size of an NTFS volume is 3792 KB.")
        y: 150
        x: 60
        color: palette.text
        horizontalAlignment: Text.AlignJustify
        wrapMode: Text.WordWrap
    }






}
