import QtQuick 2.7
import QtQuick.Controls 2.0
import "../" as UI

Item {
    id: top;
    signal update()

    Connections {
        target: top
        onUpdate: {
            catchClose();
        }
    }

    Flickable {
        id: flickable
        y: 15
        anchors.horizontalCenter: parent.horizontalCenter
        height: 280
        width: top.width-98
        clip: true
        contentWidth: content.width
        contentHeight: content.height
        flickableDirection: Flickable.VerticalFlick
        interactive: true
        boundsBehavior: Flickable.StopAtBounds

        ScrollBar.vertical: ScrollBar {
                parent: flickable.parent
                anchors.top: flickable.top
                anchors.left: flickable.right
                anchors.bottom: flickable.bottom
                active: true
            }

        Column {
            id: content
            spacing: 20
            Row {
                spacing: 5
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    y:5
                    height: 30
                    color: palette.text
                    font.pointSize: 11
                    text: qsTr("Cipher :") + Translation.tr
                }
                UI.GSCustomComboBox {
                    id: algo
                    model: ["GOST 28147-89", "Gost Grasshopper"]
                    width: 200
                    height: 30
                }
                UI.GSCheckBox {
                    id: xtsmode
                    text_: qsTr("XTS mode") + Translation.tr
                    size_: 20
                    sizeText: 10
                    height: 30
                    textColor: palette.green
                    checked: (UserSettings.getSetting("XTS-mode") === "1") ? true : false
                    onCheckedChanged: {
                        if(xtsmode.checked == true)
                            UserSettings.setSetting("XTS-mode", 1)
                        else
                            UserSettings.setSetting("XTS-mode", 0)
                    }
                }
            }

            Rectangle {
                id: block1
                color: "transparent"
                border.width: 1
                border.color: palette.border
                radius: 5
                width: top.width-100
                height: 110
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    id: text1
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 25
                    y: 5
                    color: palette.green
                    text: qsTr("Key (hexadecimal)") + Translation.tr
                    font.pointSize: 11
                }

                UI.SecureTextField {
                    id: keytext
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: text1.bottom
                    anchors.topMargin: 5
                    height: 30
                    width: parent.width - 20
                    text: "8899aabbccddeeff0011223344556677fedcba98765432100123456789abcdef"
                    horizontalAlignment: TextInput.AlignHCenter
                    type: true
                    maximumLength: 64
                }

                Row {
                    anchors.top: keytext.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.topMargin: 5
                    spacing: 5
                    Text {
                        y:5
                        height: 30
                        color: palette.text
                        text: qsTr("Key size:") + Translation.tr
                        font.pointSize: 11
                    }
                    UI.GSCustomComboBox {
                        id: bits
                        model: ["256"]
                        width: 100
                        height: 30
                    }
                    Text {
                        y:5
                        height: 30
                        color: palette.text
                        text: qsTr("bits.") + Translation.tr
                        font.pointSize: 11
                    }
                }

            }

            Rectangle {
                id: block2
                color: "transparent"
                border.width: 1
                border.color: palette.border
                radius: 5
                width: top.width-100
                height: 200
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    id: text2
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 25
                    y: 5
                    color: palette.green
                    text: qsTr("XTS mode") + Translation.tr
                    font.pointSize: 11
                }

                Text {
                    id: textseckey
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 30
                    anchors.top: text2.bottom
                    anchors.topMargin: 5
                    color: palette.text
                    text: qsTr("Secondary key (hexadecimal)") + Translation.tr
                    font.pointSize: 11
                }
                UI.SecureTextField {
                    id: secondarykey
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: textseckey.bottom
                    height: 30
                    width: parent.width - 20
                    text: "0000000000000000000000000000000000000000000000000000000000000000"
                    type: true
                    maximumLength: 64
                    horizontalAlignment: TextInput.AlignHCenter
                }
                Text {
                    id: dataunitnumber1
                    y: 5
                    anchors.top: secondarykey.bottom
                    anchors.topMargin: 5
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: palette.text
                    text: qsTr("Data unit number (64-bit hexadecimal, data unit size is 512 bytes) :") + Translation.tr
                    font.pointSize: 11
                }
                Column {
                    spacing: 5
                    anchors.top: dataunitnumber1.bottom
                    anchors.topMargin: 5
                    anchors.horizontalCenter: parent.horizontalCenter
                    Row {
                        spacing: 5
                        UI.SecureTextField {
                            id: dataunitnumber2
                            width: 200
                            text: "0"
                            type: true
                            maximumLength: 64
                        }
                    }
                    Row {
                        spacing: 5
                        Text {
                            y:5
                            height: 30
                            color: palette.text
                            text: qsTr("Block number: ") + Translation.tr
                            font.pointSize: 11
                        }
                        UI.GSCustomComboBox {
                            id: blocknumber
                            model: ["0"]
                            width: 100
                            height: 30
                        }
                    }
                }
            }

            Rectangle {
                id: block3
                color: "transparent"
                border.width: 1
                border.color: palette.border
                radius: 5
                width: top.width-100
                height: 110
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    id: text3
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 25
                    y: 5
                    color: palette.green
                    text: qsTr("Plaintext (hexadecimal)") + Translation.tr
                    font.pointSize: 11
                }

                Row {
                    spacing: 5
                    anchors.top: text3.bottom
                    anchors.topMargin: 5
                    width: parent.width - 100
                    anchors.horizontalCenter: parent.horizontalCenter
                    UI.SecureTextField {
                        id: plain
                        height: 30
                        width: parent.width - 20
                        text: "001122334455667700ffeeddccbbaa9988"
                        horizontalAlignment: TextInput.AlignHCenter
                        type: true
                        maximumLength: 32
                    }
                    Item {
                        anchors.top: plain.bottom
                        anchors.topMargin: 5
                        Row {
                            spacing: 5
                            Text {
                                y:5
                                height: 30
                                color: palette.text
                                text: qsTr("Plain text size:") + Translation.tr
                                font.pointSize: 11
                            }
                            UI.GSCustomComboBox {
                                id: plainsize
                                model: ["128"]
                                width: 100
                                height: 30
                            }
                            Text {
                                y:5
                                height: 30
                                color: palette.text
                                text: qsTr("bits.") + Translation.tr
                                font.pointSize: 11
                            }
                        }

                    }


                }
            }

            Rectangle {
                id: block4
                color: "transparent"
                border.width: 1
                border.color: palette.border
                radius: 5
                width: top.width-100
                height: 80
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    id: text4
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 25
                    y: 5
                    color: palette.green
                    text: qsTr("Ciphertext (hexadecimal)") + Translation.tr
                    font.pointSize: 11
                }

                UI.SecureTextField {
                    id: cipherText
                    anchors.top: text4.bottom
                    anchors.topMargin: 5
                    height: 30
                    width: parent.width - 100
                    text: "00000000000000000000000000000000"
                    horizontalAlignment: TextInput.AlignHCenter
                    type: true
                    maximumLength: 32
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

    }

    Row {
        spacing: 15
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        UI.GSButtonBordered {
            id: encrypt
            height: 40
            text: qsTr("Encrypt") + Translation.tr
            width: 120
            //onClicked: top.update() //TODO
            color_: palette.blue
            anchors.bottomMargin: 10
        }
        UI.GSButtonBordered {
            id: decrypt
            height: 40
            text: qsTr("Decrypt") + Translation.tr
            width: 120
            //onClicked: top.update() //TODO
            color_: palette.blue
            anchors.bottomMargin: 10
        }

        UI.GSButtonBordered {
            id: autotest
            height: 40
            text: qsTr("Auto-Test All") + Translation.tr
            width: 120
            //onClicked: top.update() //TODO
            color_: palette.green
            anchors.bottomMargin: 10
        }

        UI.GSButtonBordered {
            id: reset
            height: 40
            text: qsTr("Reset") + Translation.tr
            width: 120
            //onClicked: top.update() //TODO
            color_: palette.green
            anchors.bottomMargin: 10
        }
    }

}
