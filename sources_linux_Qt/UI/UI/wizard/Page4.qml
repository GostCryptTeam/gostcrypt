import QtQuick 2.7
import QtQuick.Dialogs 1.2
import "../" as UI

Item {
    id: top
    property string path: ""
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
        text: "Select File..."
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
        title: "Please choose a file"
        folder: shortcuts.home
        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrl)
            if(historique.pressed === false)
                UserSettings.addVolumePath(fileDialog.fileUrl)
            combo.model = UserSettings.getVolumePaths(0)
            path = fileDialog.fileUrl
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    Text {
        id:description2
        width: top.width-120
        font.pixelSize: 12
        text: qsTr("A GostCrypt volume can reside in a file (called GostCrypt container),"
                   +" which can reside on a hard disk, on a USB flash drive, etc. A GostCrypt"
                   +" container is just like any normal file (it can be, for example, moved or deleted as"
                   +" any normal file). Click 'Select File' to choose a filename for the container and"
                   +" to select the location where you wish the container to be created.<br><br><b>WARNING</b>: If you select"
                   +" an existing file, GostCrypt will NOT encrypt it; the file will be deleted and replaced with"
                   +" the newly created GostCrypt container. You will be able to encrypt existing giles (later"
                   +" on) by moving them to the GostCrypt container that you are about to create now.")
        y: 110
        x: 60
        color: palette.text
        horizontalAlignment: Text.AlignJustify
        wrapMode: Text.WordWrap
    }

}
