import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import "../" as UI

Item {
    id: top
    property variant sizeType: [0, "MB"]
    property real relativeSize: slider.value
    property int type: 0

    Row {
        id: containerSize
        spacing: 20
        anchors.horizontalCenter: parent.horizontalCenter
        Column {
            visible: (type === 2 || type === 3) ? false : true
            spacing: 10
            width: parent.width/2 - 5
            Text {
                id:titre
                font.pointSize: 13
                anchors.horizontalCenter: parent.horizontalCenter
                font.family: "Helvetica"
                text: qsTr("Set a size for your volume:") + Translation.tr
                color: palette.text
                wrapMode: Text.WordWrap
            }

            TextField {
                id: sizeValue
                width: 250
                horizontalAlignment: TextInput.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
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
                Keys.onReturnPressed: manageWizard(1)
                Keys.onEnterPressed: manageWizard(1)
            }

            Row {
                id: row
                anchors.horizontalCenter: parent.horizontalCenter
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
        }

        Column {
            visible: (type === 2 || type === 3) ? true : false
            spacing: 10
            width: parent.width
            Text {
                id:titre2
                font.pointSize: 13
                font.family: "Helvetica"
                text: qsTr("Inner volume's relative size: <br>(") + Translation.tr + Math.floor(slider.value*100) + "%)"
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                color: palette.text
                wrapMode: Text.WordWrap
            }

            Slider {
                id: slider
                minimumValue: 0.01
                maximumValue: 1.00
                stepSize: 0.01
                anchors.horizontalCenter: parent.horizontalCenter
                value: 0.70

                style: SliderStyle {
                    groove: Rectangle {
                                implicitWidth: 250
                                implicitHeight: 8
                                color: palette.darkInput
                                radius: 8
                                border.width: 1
                                border.color: palette.shadow
                            }
                            handle: Rectangle {
                                anchors.centerIn: parent
                                color: control.pressed ? palette.darkSecond : palette.green
                                border.color: palette.shadow
                                border.width: 2
                                implicitWidth: 20
                                implicitHeight: 20
                                radius: 34
                            }
                }
            }
        }
    }


    Text {
        id:description
        width: parent.width - 40
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: containerSize.bottom
        anchors.topMargin: 10
        font.pixelSize: 12
        text: ""
        y: 150
        x: 60
        color: palette.text
        horizontalAlignment: Text.AlignJustify
        wrapMode: Text.WordWrap
    }

    Text {
        id: freeDiskSpace
        text: ""
        wrapMode: Text.WordWrap
        x: bottomBar.x
        y: bottomBar.y-2
        width: bottomBar.width- 40
        height: bottomBar.height
        horizontalAlignment: Text.AlignHCenter
        color: palette.textLowOpacity
        font.pointSize: 9
    }

    function setText(bold, text) {
        freeDiskSpace.text = bold
        description.text = text
    }
}
