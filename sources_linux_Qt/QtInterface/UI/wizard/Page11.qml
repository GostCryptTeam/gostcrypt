import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import "../" as UI

Item {
    id: top
    Rectangle {
        id: options_
        color: "transparent"
        border.width: 1
        border.color: palette.border
        radius: 5
        width: top.width-100
        height: 80
        anchors.horizontalCenter: top.horizontalCenter

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            y:5
            color: palette.text
            text: qsTr("Options")
        }

        Text {
            id: fileSystem
            x:20
            y:35
            color: palette.text
            text: qsTr("Filesystem")
        }

        UI.GSCustomComboBox {
            id: typeFilesystem
            model: ["FAT", "None"]
            x: fileSystem.x + fileSystem.width + 10
            y: fileSystem.y-5
            width: parent.width * 0.2
            height: 25
        }

        Text {
            id: cluster
            x:typeFilesystem.x + typeFilesystem.width +10
            y: fileSystem.y
            color: palette.text
            text: qsTr("Cluster")
        }

        UI.GSCustomComboBox {
            id: clusterType
            model: ["Default",
                "0.5KB",
                "1KB",
                "2KB",
                "4KB",
                "8KB",
                "16KB",
                "32KB",
            ]
            x: cluster.x + cluster.width + 10
            y: fileSystem.y-5
            width: parent.width * 0.3
            height: 25
        }

        UI.GSCheckBox {
            text_: qsTr("Dynamic")
            checked: false
            x: clusterType.x + clusterType.width + 10
            y: clusterType.y
            height: 25
            leftPadding_: 5
            sizeText: 9
            onCheckedChanged: {
                //TODO
            }
        }
    }

    Rectangle {
        id: pool_
        y: options_.y + options_.height + 10
        color: "transparent"
        border.width: 1
        border.color: palette.border
        radius: 5
        width: top.width-100
        height: 80
        anchors.horizontalCenter: top.horizontalCenter

        UI.GSCheckBox {
            id: hidePool
            text_: ""
            checked: false
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.topMargin: 10
            height: 25
            leftPadding_: 5
            sizeText: 9
        }

        UI.GSHelpButton {
            size: 30
            x: -10
            y: -10
            onClicked: {
                openErrorMessage("IMPORTANT","Move your mouse as randomly as possible within this window. The longer"
                                 +" you move it, the better. This significantly increases the cryptographic strength of the"
                                 +" encryption keys. Then click Format to create the volume.")
            }
        }

        Text {
            id: randomPool_
            x: 20
            y: 10
            color: palette.text
            text: qsTr("Random Pool:")
        }
        Text {
            id: randomPoolValue_
            x: randomPool_.x + randomPool_.width + 10
            y: randomPool_.y
            width: pool_.width-10-randomPool_.width - 65
            elide: Text.ElideRight
            color: palette.text
            text: ""
        }
        Text {
            id: headerKey_
            x: 20
            y: randomPool_.y + 20
            color: palette.text
            text: qsTr("Header key:")
        }
        Text {
            id: headerKeyValue_
            x: headerKey_.x + headerKey_.width + 10
            y: headerKey_.y
            width: pool_.width-10-headerKey_.width - 60
            color: palette.text
            elide: Text.ElideRight
            text: ""
        }
        Text {
            id: masterKey_
            x: 20
            y: headerKey_.y + 20
            color: palette.text
            text: qsTr("Master key:")
        }
        Text {
            id: masterKeyValue_
            x: masterKey_.x + masterKey_.width + 10
            y: masterKey_.y
            width: pool_.width-10-masterKey_.width - 60
            color: palette.text
            elide: Text.ElideRight
            text: ""
        }
    }

    Rectangle {
        id: progress_
        y: pool_.y + pool_.height + 10
        color: "transparent"
        border.width: 1
        border.color: palette.border
        radius: 5
        width: top.width-100
        height: 80
        anchors.horizontalCenter: top.horizontalCenter

        ProgressBar {
            id: bar
            width: parent.width*0.7
            height: 20
            maximumValue: 100
            value: 0
            x: 20
            y: 20
            style: ProgressBarStyle {
                background: Rectangle {
                    implicitWidth: bar.width
                    implicitHeight: bar.height
                    color: palette.darkThird
                    border.color: palette.border
                    border.width: 1
                }
                progress: Rectangle {
                    color: palette.blue
                    border.color: palette.border
                    border.width: 1
                }
            }
        }

        UI.GSButtonBordered {
            color_: palette.blue
            text: qsTr("Abort")
            y: 20
            anchors.left: bar.right
            anchors.leftMargin: 20
            width: 110
            height: 40
        }

        Text {
            id:infos
            width: parent.width*0.7 +20 +140
            font.pixelSize: 12
            text: qsTr("Done : <b><font color='#'>0%</font></b> - Speed : <b><font color='#'>0 o/s</font></b> - Left : <b><font color='#'>0 s</font></b>")
            y: bar.y + 30
            x: 20
            color: palette.text
            horizontalAlignment: Text.AlignJustify
            wrapMode: Text.WordWrap
        }

    }

    function updatePool(pool)
    {
        if(!hidePool.checked)
            randomPoolValue_.text = pool
        else
            randomPoolValue_.text = pool.replace(/./g, '*')
    }
    function updateHeaderKey(HK)
    {
        if(!hidePool.checked)
            headerKeyValue_.text = HK
        else
            headerKeyValue_.text = HK.replace(/./g, '*')
    }
    function updateMasterKey(MK)
    {
        if(!hidePool.checked)
            masterKeyValue_.text = MK
        else
            masterKeyValue_.text = MK.replace(/./g, '*')
    }
    function setProgress(bar, percent, speed, left)
    {

    }
}
