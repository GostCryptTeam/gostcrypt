import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import "../" as UI

Item {
    id: top
    property variant sizeType: [0, "MB"]
    property int type: 0

    Text {
        id:titre
        font.pointSize: 13
        font.family: "Helvetica"
        text: qsTr("Set a size for your volume:") + Translation.tr
        anchors.horizontalCenter: parent.horizontalCenter
        color: palette.text
        wrapMode: Text.WordWrap
    }

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
        text: ""
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
        text: ""
        y: 150
        x: 60
        color: palette.text
        horizontalAlignment: Text.AlignJustify
        wrapMode: Text.WordWrap
    }

    function setText(bold, text) {
        freeDiskSpace.text = bold
        description.text = text
    }
}
