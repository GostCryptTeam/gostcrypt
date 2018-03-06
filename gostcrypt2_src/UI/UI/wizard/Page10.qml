import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import "../" as UI

Item {
    id: top
    property variant format: [app.filesystems[0]] //file system, cluster & dynamic(bool) (standard volumes)
    property int type: 0

    MouseArea {
        anchors.fill: parent
        hoverEnabled : true
        onPositionChanged: {
            //Wizard.sendRandomPosition(mouse.x, mouse.y)
            //TODO? : send mouse position to randomly change the pool & header/master keys
        }
    }

    Rectangle {
        id: options_
        color: "transparent"
        border.width: 1
        border.color: palette.border
        radius: 5
        width: top.width-100
        height: 80
        y: 10
        anchors.horizontalCenter: top.horizontalCenter

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            y:5
            color: palette.text
            text: qsTr("Options") + Translation.tr
        }

        Text {
            id: fileSystem
            x:20
            y:35
            color: palette.text
            text: qsTr("Filesystem") + Translation.tr
        }

        UI.CustomComboBox {
            id: typeFilesystem
            model: app.filesystems
            x: fileSystem.x + fileSystem.width + 10
            y: fileSystem.y-5
            width: parent.width * 0.2
            height: 25
            onActivated: {
                format[0] = typeFilesystem.textAt(typeFilesystem.currentIndex)
            }
        }

        Text {
            id: cluster
            x:typeFilesystem.x + typeFilesystem.width +10
            y: fileSystem.y
            color: palette.text
            text: qsTr("Cluster") + Translation.tr
        }

        UI.CustomComboBox {
            id: clusterType
            model: [qsTr("Default"),
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
            onActivated: {
                clusterType[0] = clusterType.textAt(clusterType.currentIndex)
            }
        }
    }

    Rectangle {
        id: pool_
        y: options_.y + options_.height + 20
        color: "transparent"
        border.width: 1
        border.color: palette.border
        radius: 5
        width: top.width-100
        height: 80
        anchors.horizontalCenter: top.horizontalCenter

        UI.HelpButton {
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
            text: qsTr("Random Pool:") + Translation.tr
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
            text: qsTr("Header key:") + Translation.tr
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
            text: qsTr("Master key:") + Translation.tr
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
}
