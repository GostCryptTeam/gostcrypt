import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.4
import "../" as UI

import gostcrypt.ui.secureInput 1.0

Item {
    id: openVolume_Form

    property url volumePath
    property variant devices
    property variant listKeyfiles: []
    signal incorrectPassword()
    signal sendInfoVolume()
    signal sendKeyFileInfoVolume()

    Connections {
        target: openVolume_Form
        onSendInfoVolume: {
            if(password_value.text.length !== 0)
            {
                var params = {
                    "path": volumePath,
                    "password": password_value.text,
                    "nb-keyfiles": 0,
                    "name": qsTr("Mount volume"),
                    "desc": volumePath
                }

                //Use Mount Options
                if(UserSettings.getSetting("Pref-useCurrentMO") === "1")
                {
                    params["use-mount-option"] = 1
                    params["timestamp"] = UserSettings.getSetting("Pref-TimeStamp")
                    params["protection"] = UserSettings.getSetting("Pref-ReadOnly")
                    params["backup-headers"] = UserSettings.getSetting("Pref-backup-headers")
                    params["shared"] = UserSettings.getSetting("Pref-shared")
                    params["user"] = UserSettings.getSetting("Pref-user")
                    params["group"] = UserSettings.getSetting("Pref-group")
                }

                qmlRequest("mount", params);
                var password_blank = new Array(password_value.length+1).join('#');
                password_value.text = password_blank
                password_value.text = "" //TODO: stock password in C++
            }else{
                app.openErrorMessage("Empty password", "Please enter a password.");
            }
        }
        onSendKeyFileInfoVolume: {
            if(listKeyfiles.length > 0 || password_value.text.length !== 0)
            {
                var request = {
                    "path": volumePath,
                    "password": password_value.text,
                    "nb-keyfiles": listKeyfiles.length,
                    "name": qsTr("Mount volume"),
                    "desc": qsTr("Volume mounted using "+ listKeyfiles.length +" keyfiles")
                };

                //Use Mount Options
                if(UserSettings.getSetting("Pref-useCurrentMO") === "1")
                {
                    request["timestamp"] = UserSettings.getSetting("Pref-TimeStamp")
                    request["protection"] = UserSettings.getSetting("Pref-ReadOnly")
                    request["backup-headers"] = UserSettings.getSetting("Pref-backup-headers")
                    request["shared"] = UserSettings.getSetting("Pref-shared")
                    request["user"] = UserSettings.getSetting("Pref-user")
                    request["group"] = UserSettings.getSetting("Pref-group")
                }

                for(var i in listKeyfiles)
                    request["keyfile"+i] = listKeyfiles[i].path;
                console.log(request["keyfile0"])

                qmlRequest("mount", request);

                //Empties values
                var password_blank = new Array(password_value.length+1).join('#');
                password_value.text = password_blank
                password_value.text = "" //TODO: stock password in C++
                keyfiles_paths.model = []
                listKeyfiles = []
            }else{
                app.openErrorMessage("Empty password", "Please enter a password or select keyfiles.");
            }
        }
    }

    Connections {
        target: ConnectSignals
        onVolumePasswordIncorrect: {
            password_value.style = Qt.createComponent("textFieldRed.qml");
        }
    }

    Item {
        id: item
        height: 170
        anchors.centerIn: parent

        Text {
            id: textTop
            text: qsTr("Please open a GostCrypt volume :") + Translation.tr
            font.pointSize: 11
            color: palette.textLight
            anchors.horizontalCenter: parent.horizontalCenter

            Behavior on opacity { NumberAnimation { id: animTextTop; duration: app.duration; easing.type: Easing.OutQuad; } }
        }

        UI.CustomComboBox {
            id: combo
            anchors.horizontalCenter: parent.horizontalCenter
            width: openVolume_Form.width - 250
            y: textTop.y + textTop.height + 10
            height: 40
            model: {
                var paths = UserSettings.getVolumePaths(1)
                return paths;
            }
            onActivated: {
                if(currentText != "")
                    openVolume_Form.moving(currentText)
            }
        }

        UI.CheckBox {
            id: historique
            text_: qsTr("Never save history")
            y: combo.y + combo.height + 5
            height: combo.height
            anchors.horizontalCenter: parent.horizontalCenter
            size_: 20
            checked: {
                var isChecked = UserSettings.getSetting("MountV-SaveHistory")
                return (isChecked === "1") ? true : false;
            }
            onCheckedChanged: {
                if(historique.checked == true)
                    UserSettings.setSetting("MountV-SaveHistory", 1)
                else
                    UserSettings.setSetting("MountV-SaveHistlistKeyfilesory", 0)
                if(historique.checked === true) {
                    UserSettings.erasePaths()
                    UserSettings.getVolumePaths(1)
                }
            }
        }

        Row {
            id: buttonsOpenVolume
            spacing: 25
            anchors.horizontalCenter: parent.horizontalCenter
            y: historique.y + historique.height + 5

            UI.ButtonBordered {
                id: buttonOpen
                height: combo.height
                text: qsTr("Open...") + Translation.tr
                width: 120
                onClicked: fileDialog.open()
                color_: palette.green
            }

            UI.ButtonBordered {
                id: buttonDevide
                height: combo.height
                text: qsTr("Select Device")
                width: 120
                color_: palette.green
                onClicked: {
                    qmlRequest("devices", "")
                    changeSubWindowTitle(qsTr("Please select a device"))
                    devicesSelection.opacity = 1.0
                }
            }
        }

        FileDialog {
            id: fileDialog
            title: qsTr("Please choose a file") + Translation.tr
            folder: shortcuts.home
            onAccepted: {
                if(historique.pressed === false)
                    UserSettings.addVolumePath(fileDialog.fileUrl)
                openVolume_Form.moving(fileDialog.fileUrl)
                combo.model = UserSettings.getVolumePaths(0)
            }
            onRejected: {
            }
        }

        Behavior on y {
            NumberAnimation {
                id: anim;
                duration: app.duration;
                easing.type: Easing.OutQuad;
                onRunningChanged: {
                    if (!anim.running) {
                        appendPassword();
                    }
                }
            }
        }
    }

    Item {
        id: password
        //visible: false
        y: item.y + item.height
        anchors.horizontalCenter: parent.horizontalCenter
        opacity: 0.0

        Text {
            id: password_txt
            y: 0
            x: -combo.width/2
            text: qsTr("Password: ")  + Translation.tr
            font.pointSize: 11
            lineHeightMode: Text.FixedHeight
            horizontalAlignment: Text.AlignRight
            lineHeight: 15
            color: '#719c24'
        }

        Text {
            id: keyfile_txt
            y: 0
            x: 0
            visible: use_Keyfiles.checked ? true : false
            text: qsTr("Keyfile(s): ")  + Translation.tr
            font.pointSize: 11
            lineHeightMode: Text.FixedHeight
            horizontalAlignment: Text.AlignRight
            lineHeight: 15
            color: '#719c24'
        }

        UI.SecureTextField {
            id: password_value
            y: password_txt.y + password_txt.height + 10
            x: -combo.width/2-2
            radius_: 3
            width: use_Keyfiles.checked ? combo.width/2-2 : combo.width
            horizontalAlignment: TextInput.AlignHCenter
            bordercolor: palette.darkInput
            height_: combo.height
            onValidated: {
                sendInfoVolume()
                var password_blank = new Array(password_value.length+1).join('#');
                password_value.text = password_blank
            }
            onShiftPressed: {
                if(isPressed)
                    shift.opacity = 1.0
                else
                    shift.opacity = 0.0
            }
        }

        Image {
            id: shift
            fillMode: Image.PreserveAspectFit
            source: "../ressource/caps.png"
            height: 40
            width: 40
            anchors.top: password_value.top
            anchors.right: password_value.left
            anchors.rightMargin: 5
            opacity: 0.0
            Behavior on opacity {
                NumberAnimation {
                    duration: app.duration/2;
                    easing.type: Easing.OutQuad;
                }
            }
        }

        UI.CustomComboBox {
            id: keyfiles_paths
            model: {
                var keyfiles = UserSettings.getFavoriteKeyFiles();
                for(var i = 0; i < keyfiles.length; ++i) listKeyfiles.push({ path: keyfiles[i] })
                return listKeyfiles
            }

            x: password_value.width - combo.width/2
            y: password_txt.y + password_txt.height + 10
            width: combo.width/2
            height: combo.height
            borderWidth: 0
            visible: use_Keyfiles.checked ? true : false
            delegate:
                ItemDelegate {
                    id: itemDelegate
                    width: keyfiles_paths.width
                    background: Rectangle {
                        color: itemDelegate.down ? palette.border : "transparent"
                        height: 40
                        width: keyfiles_paths.width
                        Text {
                            id: close
                            x: 10
                            color: palette.text
                            text: "×"
                            width: 30
                            anchors.verticalCenter: parent.verticalCenter
                            font.pointSize: 15
                        }
                        Text {
                            x: 30
                            width: parent.width - 50
                            text: modelData.path
                            elide: Text.ElideRight
                            anchors.verticalCenter: parent.verticalCenter
                            font.pointSize: 9
                            color: palette.green
                        }
                        MouseArea {
                            id: delegateItem
                            hoverEnabled: true
                            anchors.fill: parent
                        }
                        MouseArea {
                            id: closeItem
                            hoverEnabled: true
                            anchors.fill: close
                            onClicked: {
                                if (index > -1) {
                                    listKeyfiles.splice(index, 1)
                                    keyfiles_paths.model = listKeyfiles
                                }
                            }
                        }
                        ToolTip {
                            parent: parent
                            text: modelData.path
                            visible: delegateItem.containsMouse
                            delay: 500
                            timeout: 5000
                        }
                        ToolTip {
                            parent: parent
                            text: qsTr("Remove this keyfile from the list")
                            visible: closeItem.containsMouse
                            delay: 500
                            timeout: 5000
                        }
                    }
                    highlighted: keyfiles_paths.highlightedIndex === index
            }
        }

        Rectangle {
            visible: use_Keyfiles.checked ? true : false
            anchors.left: keyfiles_paths.right
            anchors.leftMargin: 5
            color: palette.darkInput
            height: 20
            width: 20
            radius: 5
            border.width: 1
            anchors.verticalCenter: keyfiles_paths.verticalCenter
            border.color: palette.blue
            Text {
                anchors.centerIn: parent;
                text: qsTr("×")
                font.pointSize: 15
                color: palette.text
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    listKeyfiles = []
                    keyfiles_paths.model = []
                }
            }
        }

        FileDialog {
            id: addKeyfiles
            title: qsTr("Please choose a keyfile") + Translation.tr
            folder: shortcuts.home
            selectMultiple: true
            onAccepted: {
                var text = "";
                if(addKeyfiles.fileUrls.length > 0)
                {
                    use_Keyfiles.checked = true;
                    UserSettings.setSetting("MountV-UseKeyFiles", 1)
                }
                for(var path in addKeyfiles.fileUrls) {
                    listKeyfiles.push({path: addKeyfiles.fileUrls[path]});
                }
                for(var i in listKeyfiles) {
                    text = text + listKeyfiles[i] + "; ";
                }
                keyfiles_paths.model = listKeyfiles;
            }
            onRejected: {
            }
        }

        UI.CheckBox {
            id: display
            text_: qsTr("Display password")  + Translation.tr
            x: combo.x
            anchors.top: password_value.bottom
            anchors.topMargin: 5
            height: 20
            size_: 20
            sizeText: 10
            checked: {
                var isChecked = UserSettings.getSetting("MountV-ShowPassword")
                return (isChecked === "1") ? true : false;
            }
            onCheckedChanged: {
                if(display.checked == true) {
                    UserSettings.setSetting("MountV-ShowPassword", 1)
                    password_value.echoMode = TextInput.Normal;
                } else {
                    UserSettings.setSetting("MountV-ShowPassword", 0)
                    password_value.echoMode = TextInput.Password;
                }
            }

        }

        UI.CheckBox {
            id: use_Keyfiles
            text_: qsTr("Use keyfiles") + Translation.tr
            x: combo.x
            anchors.top: display.bottom
            anchors.topMargin: 5
            height: 20
            size_: 20
            sizeText: 10
            checked: {
                var isChecked = UserSettings.getSetting("MountV-UseKeyFiles")
                return (isChecked === "1") ? true : false;
            }
            onCheckedChanged: {
                //TODO : action
                if(use_Keyfiles.checked == true) {
                    UserSettings.setSetting("MountV-UseKeyFiles", 1)
                    keyfile_txt.visible = true
                } else {
                    UserSettings.setSetting("MountV-UseKeyFiles", 0)
                    keyfile_txt.visible = false
                }
            }
        }

        UI.CheckBox {
            id: useMountOptions
            text_: qsTr("Use current \"Mount Options\" configuration")  + Translation.tr
            x: combo.x
            anchors.top: use_Keyfiles.bottom
            anchors.topMargin: 5
            height: 20
            size_: 20
            sizeText: 10
            checked: (UserSettings.getSetting("Pref-useCurrentMO") === "1") ? true : false
            onCheckedChanged: {
                if(useMountOptions.checked == true)
                    UserSettings.setSetting("Pref-useCurrentMO", 1)
                else
                    UserSettings.setSetting("Pref-useCurrentMO", 0)
            }
        }

        UI.ButtonBordered {
            id: buttonKeyfiles
            x: combo.x + combo.width - 150
            y: display.y
            height: 30
            text: qsTr("Keyfiles...") + Translation.tr
            width: 150
            color_: palette.green
            onClicked: {
                addKeyfiles.open();
            }
        }

        UI.ButtonBordered {
            id: buttonMountOption
            x: combo.x + combo.width - 150
            y: buttonKeyfiles.y + buttonKeyfiles.height + 5
            height: 30
            text: qsTr("Mount Options...") + Translation.tr
            width: 150
            color_: palette.green
            onClicked: {
                mountOptions.opacity = 1.0
            }
        }
        Behavior on opacity {
            NumberAnimation {
                id: anim2
                duration: 500;
                easing.type: Easing.OutQuad;
                onRunningChanged: {
                    if(!anim2.running) {
                        password_value.focus = true;
                    }
                }
            }
        }
    }

    UI.ButtonBordered {
        id: validation
        anchors.horizontalCenter: openVolume_Form.horizontalCenter
        anchors.bottom: openVolume_Form.bottom
        text: qsTr("Mount Volume") + Translation.tr
        opacity: 0.0
        color_: palette.blue
        onClicked: {
            var password_blank = "";
            if(sudo_.isVisible === false && UserSettings.getSetting("MountV-UseKeyFiles") === "0")
            {
                sendInfoVolume()
                password_blank = new Array(password_value.length+1).join('#');
                password_value.text = password_blank
            }
            else if(sudo_.isVisible === false && UserSettings.getSetting("MountV-UseKeyFiles") === "1")
            {
                sendKeyFileInfoVolume()
                keyfiles_paths.model = []
            }
        }
        Behavior on opacity { NumberAnimation { duration: 500; easing.type: Easing.OutQuad; } }
    }

    // Device part
    Rectangle {
        id: devicesSelection
        visible: false
        opacity: 0.0
        anchors.fill:parent
        color: palette.darkSecond
        Behavior on opacity { NumberAnimation { id: anim_; duration: app.duration/2; easing.type: Easing.OutQuad; onRunningChanged: {
                    if(!anim_.running) {
                        devicesSelection.visible = !devicesSelection.visible
                    }
                } } }

        Component {
            id: deviceDelegate
            Item {
                id: elementDevice
                width: 320;
                height: 70;
                anchors.horizontalCenter: parent.horizontalCenter
                Behavior on height { NumberAnimation { duration: app.duration/3; easing.type: Easing.OutQuad; } }
                Behavior on width { NumberAnimation { duration: app.duration/3; easing.type: Easing.OutQuad; } }
                Rectangle {
                    id: contentDevice
                    anchors.fill: parent
                    color: '#3b3b3b'
                    border.color: palette.darkThird
                    border.width: 1
                    radius: 2

                    //Circle (number)
                    Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        width: 50
                        height: 50
                        radius: 50
                        color: palette.darkInput
                        x:10
                        Text {
                            color: palette.text
                            text: number
                            anchors.centerIn: parent
                            font.pixelSize: 15
                        }
                    }
                    Column {
                        id: infos
                        x: 70
                        y: 8
                        Text {
                            id: infos_Path
                            font.pixelSize: 18
                            color:palette.text;
                            text: '<b>Path:</b> ' + path
                        }
                        Text {
                            font.pixelSize: 11
                            color:palette.text;
                            text: '<b>Size:</b> ' + size
                        }
                        Text {
                            font.pixelSize: 11
                            color:palette.text;
                            text: '<b>Mount point:</b> ' + mountPoint
                        }
                    }

                    MouseArea {
                        id:area
                        anchors.fill:parent
                        hoverEnabled: true
                        cursorShape: area.containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
                        onEntered: {
                            contentDevice.color = "#454545"
                            elementDevice.width = 400
                            // elementDevice.height = 87
                        }
                        onExited: {
                            contentDevice.color = "#3B3B3B"
                            elementDevice.width = 320
                            // elementDevice.height = 70
                        }
                        onClicked: {
                            openVolume_Form.moving(path)
                            if(historique.pressed === false)
                                UserSettings.addVolumePath(path)
                            combo.model = UserSettings.getVolumePaths(0)
                            devicesSelection.opacity = 0.0
                            changeSubWindowTitle(qsTr("Open a GostCrypt Volume"))
                        }
                    }
                }
            }
        }

        ListModel {
            id: listDeviceModel
        }

        ListView {
            id: listOfDevices
            anchors.fill: parent
            delegate: deviceDelegate
            model: listDeviceModel
            focus: true
            ScrollBar.vertical: ScrollBar { snapMode: ScrollBar.SnapOnRelease }
            snapMode: GridView.SnapToRow
            clip: true
            spacing:10
            anchors.topMargin: 10
            anchors.bottomMargin: 10
        }
    }

    // mount option part
    Rectangle {
        id: mountOptions
        visible: false
        opacity: 0.0
        width: parent.width - 2
        x: 1
        height: parent.height
        color: palette.darkSecond
        Behavior on opacity {
            NumberAnimation {
                id: anim2_;
                duration: app.duration/2;
                easing.type: Easing.OutQuad;
                onRunningChanged: {
                    if(!anim2_.running) {
                        mountOptions.visible = !mountOptions.visible
                    }
                }
            }
        }

        MouseArea {
            id:area
            anchors.fill:parent
            hoverEnabled: true

        }

        Rectangle {
            id: block5
            color: "transparent"
            border.width: 1
            border.color: palette.border
            radius: 5
            width: parent.width - 100
            x: 1
            y: 10
            height: parent.height - 20
            anchors.horizontalCenter: parent.horizontalCenter
            Column {
                id: mountOptionsContent
                spacing: 15
                height: parent.height
                anchors.centerIn: parent
                y: 10
                Text {
                    id: text5
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 25
                    y: 5
                    color: palette.green
                    text: qsTr("Mount Options") + Translation.tr
                    font.pointSize: 11
                }

                UI.CheckBox {
                    id: timestamp
                    text_: qsTr("Preserve Timestamp of file")
                    anchors.horizontalCenter: parent.horizontalCenter
                    size_: 20
                    sizeText: 10
                    checked: {
                        var isChecked = UserSettings.getSetting("Pref-TimeStamp")
                        return (isChecked === "1") ? true : false;
                    }
                    onCheckedChanged: {
                        if(timestamp.checked == true)
                            UserSettings.setSetting("Pref-TimeStamp", 1)
                        else
                            UserSettings.setSetting("Pref-TimeStamp", 0)
                    }
                }



                Row {
                    spacing: 5
                    anchors.horizontalCenter: parent.horizontalCenter
                    Text {
                        text: qsTr("Volume protection :")
                        color: palette.text
                        font.pointSize: 10
                    }
                    ExclusiveGroup { id: group }
                    UI.CheckBox {
                        id: readOnly1
                        text_: qsTr("None")
                        size_: 20
                        sizeText: 10
                        exclusiveGroup: group
                        checked: {
                            var isChecked = UserSettings.getSetting("Pref-ReadOnly")
                            return (isChecked === "0") ? true : false;
                        }
                        onCheckedChanged: {
                            if(readOnly1.checked === true)
                                UserSettings.setSetting("Pref-ReadOnly", 0)
                        }
                    }

                    UI.CheckBox {
                        id: readOnly2
                        text_: qsTr("Normal volume Read Only")
                        size_: 20
                        sizeText: 10
                        exclusiveGroup: group
                        checked: {
                            var isChecked = UserSettings.getSetting("Pref-ReadOnly")
                            return (isChecked === "1") ? true : false;
                        }
                        onCheckedChanged: {
                            if(readOnly2.checked === true)
                                UserSettings.setSetting("Pref-ReadOnly", 1)
                        }
                    }

                    UI.CheckBox {
                        id: readOnly3
                        text_: qsTr("Hidden volume Read Only")
                        size_: 20
                        sizeText: 10
                        exclusiveGroup: group
                        checked: {
                            var isChecked = UserSettings.getSetting("Pref-ReadOnly")
                            return (isChecked === "2") ? true : false;
                        }
                        onCheckedChanged: {
                            if(readOnly3.checked === true)
                                UserSettings.setSetting("Pref-ReadOnly", 2)
                        }
                    }
                }

                UI.CheckBox {
                    id: useBackup
                    text_: qsTr("Open volume with its backup header")
                    anchors.horizontalCenter: parent.horizontalCenter
                    size_: 20
                    sizeText: 10
                    checked: {
                        var isChecked = UserSettings.getSetting("Pref-backup-headers")
                        return (isChecked === "1") ? true : false;
                    }
                    onCheckedChanged: {
                        if(useBackup.checked === true)
                            UserSettings.setSetting("Pref-backup-headers", 1)
                        else
                            UserSettings.setSetting("Pref-backup-headers", 0)
                    }
                }

                UI.CheckBox {
                    id: shared
                    text_: qsTr("Allow shared access to the volume")
                    anchors.horizontalCenter: parent.horizontalCenter
                    size_: 20
                    sizeText: 10
                    checked: {
                        var isChecked = UserSettings.getSetting("Pref-shared")
                        return (isChecked === "1") ? true : false;
                    }
                    onCheckedChanged: {
                        if(shared.checked === true)
                            UserSettings.setSetting("Pref-shared", 1)
                        else
                            UserSettings.setSetting("Pref-shared", 0)
                    }
                }

                Row {
                    spacing: 5
                    anchors.horizontalCenter: parent.horizontalCenter
                    Text {
                        text: qsTr("Mount the volume for the specified <b>user</b> (leave empty for the current user) :")
                        color: palette.text
                        font.pointSize: 10
                    }

                    UI.SecureTextField {
                        id: mountedForUser
                        type: true
                        width: 100
                        text: UserSettings.getSetting("Pref-user")
                        Keys.onReleased: UserSettings.setSetting("Pref-user", mountedForUser.text)
                    }

                }

                Row {
                    spacing: 5
                    anchors.horizontalCenter: parent.horizontalCenter
                    Text {
                        text: qsTr("Mount the volume for the specified <b>group</b> (leave empty for the current user) :")
                        color: palette.text
                        font.pointSize: 10
                    }

                    UI.SecureTextField {
                        id: mountedForGroup
                        type: true
                        width: 100
                        text: UserSettings.getSetting("Pref-group")
                        Keys.onReleased: UserSettings.setSetting("Pref-group", mountedForGroup.text)
                    }
                }
            }

            UI.ButtonBordered {
                id: saveChanges
                height: 40
                text: qsTr("Save changes") + Translation.tr
                anchors.horizontalCenter: parent.horizontalCenter
                width: 120
                onClicked: {
                    UserSettings.setSetting("Pref-user", mountedForUser.text)
                    UserSettings.setSetting("Pref-group", mountedForGroup.text)
                    mountOptions.opacity = 0.0
                }
                color_: palette.blue
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10
            }
        }
    }

    function moving(url) {
        password_value.focus = true;
        textTop.opacity = 0.0
        combo.y = 15
        item.anchors.centerIn = undefined;
        item.y = 0
        item.height = 155
        volumePath = url
    }

    function appendPassword() {
        //password.visible = true
        password.opacity = 1.0
        validation.opacity = 1.0
    }

    function initDrag(parameter) {
        moving(parameter.value)
        UserSettings.addVolumePath(parameter.value)
        combo.model = UserSettings.getVolumePaths(0)
    }

    function addHostDevice(device)
    {
        listDeviceModel.append(device)
    }

}
