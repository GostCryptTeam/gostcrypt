import QtQuick 2.7
import QtQuick.Dialogs 1.2
import "../" as UI

Item {
    id: top
    property string path: ""
    property string message: ""
    property int type: 0

    UI.GSCustomComboBox {
        id: combo
        x: 60
        y: 10
        width: parent.width - 250
        model: {
            var paths = UserSettings.getVolumePaths(1)
            return paths;
        }
        onActivated: {
            path = currentText
        }
    }
    UI.GSButtonBordered {
        id: buttonOpen
        x: combo.x + combo.width + 15
        y: combo.y
        height: combo.height
        text: qsTr("Select File...")
        width: 100
        onClicked: fileDialog.open()
        color_: palette.green
    }
    UI.GSCheckBox {
        id: historique
        text_: qsTr("Never save history")
        checked: {
            var isChecked = UserSettings.getSetting("MountV-SaveHistory")
            return (isChecked == 1) ? true : false;
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

    Text {
        id:description2
        width: top.width-120
        font.pixelSize: 12
        text: message
        y: 110
        x: 60
        color: palette.text
        horizontalAlignment: Text.AlignJustify
        wrapMode: Text.WordWrap
    }

    function setFileDialog(bool) {
        fileDialog.selectExisting = bool
    }

}
