import QtQuick 2.7
import QtQuick.Dialogs 1.2
import "../" as UI

Item {
    id: top
    property string path: ""
    property string message: ""
    property int type: 0

    Text {
        id:titre
        y: 10
        font.pointSize: 13
        font.family: "Helvetica"
        text: qsTr("Create a new file that will contain your volume:") + Translation.tr
        anchors.horizontalCenter: parent.horizontalCenter
        color: palette.text
        wrapMode: Text.WordWrap
    }

    UI.GSButtonBordered {
        id: buttonOpen
        anchors.top: titre.bottom
        anchors.topMargin: 20
        height: combo.height
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Select File...")
        width: 200
        onClicked: fileDialog.open()
        color_: palette.green
    }

    UI.GSHelpButton {
        size: combo.height
        anchors.left: buttonOpen.right
        anchors.leftMargin: 10
        y: buttonOpen.y
        onClicked: {
            openErrorMessage("Information", message)
        }
    }


    UI.GSCustomComboBox {
        id: combo
        width: parent.width - 250
        anchors.top: buttonOpen.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        model: {
            var paths = UserSettings.getVolumePaths(1)
            return paths;
        }
        onActivated: {
            path = currentText
        }
    }

    UI.GSCheckBox {
        id: historique
        text_: qsTr("Never save history")
        anchors.horizontalCenter: parent.horizontalCenter
        checked: {
            var isChecked = UserSettings.getSetting("MountV-SaveHistory")
            return (isChecked === "1") ? true : false;
        }
        x: combo.x
        y: combo.y + 50
        height: combo.height
        onCheckedChanged: {
            if(historique.checked == true)
                UserSettings.setSetting("MountV-SaveHistory", 1)
            else
                UserSettings.setSetting("MountV-SaveHistory", 0)
            if(historique.checked === true) {
                UserSettings.erasePaths()
                UserSettings.getVolumePaths(1)
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Please choose a file") + Translation.tr
        folder: shortcuts.home
        selectExisting: {
            if(type !== 2)
                return false
            else
                return true
        }
        onAccepted: {
            if(historique.pressed === false)
                UserSettings.addVolumePath(fileDialog.fileUrl)
            combo.model = UserSettings.getVolumePaths(0)
            path = fileDialog.fileUrl
        }
        onRejected: {
        }
    }

    function setFileDialog(bool) {
        fileDialog.selectExisting = bool
    }

}
