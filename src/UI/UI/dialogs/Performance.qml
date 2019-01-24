import QtQuick 2.0
import "../" as UI

Item {
    id: top;
    signal update()

    Connections {
        target: top
        onUpdate: {
            UserSettings.setSetting("Perf-Use", (use.checked === false) ? 0 : 1);
            UserSettings.setSetting("Perf-NumberOfCore", typePerf.currentText);
            catchClose();
        }
    }

    Text {
        id: text
        anchors.horizontalCenter: parent.horizontalCenter
        y: 10
        anchors.bottomMargin: 30
        color: palette.text
        font.pointSize: 11
        text: {
            return "";
        }
    }
    Column {
        anchors.topMargin: 15
        anchors.top: text.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 50
        spacing: 50
        Row {
            width: 600
            spacing: 15
            anchors.horizontalCenter: parent.horizontalCenter
            UI.CheckBox {
                id: use
                text_: qsTr("Do not use following number of processors for encryption/decryption: ")
                height: 40
                checked: {
                    var isChecked = UserSettings.getSetting("Perf-Use")
                    return (isChecked === "1") ? true : false;
                }
                onCheckedChanged: {
                    if(use.checked == true)
                        UserSettings.setSetting("Perf-Use", 1)
                    else
                        UserSettings.setSetting("Perf-Use", 0)
                }
            }
            UI.CustomComboBox {
                id: typePerf
                model: {
                    return getNumberOfCore(UserSettings.getNumberOfCore());
                }
                width: 60
                height: 40
            }
        }

        Text {
            text: qsTr("Note that the Hyper-Threading technology provides "
                       +"multiple local cores per a single physical core. "
                       +"<br>When Hyper Theading is enabled, the number selected "
                       +"above represents the number of logical processors/cores.")
            width: 600
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHLeft
            color: palette.text
            font.pointSize: 11
            anchors.horizontalCenter: parent.horizontalCenter
        }

    }

    Row {
        spacing: 15
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        UI.ButtonBordered {
            id: saveButton
            height: 40
            text: qsTr("Benchmark") + Translation.tr
            width: 120
            onClicked: openSubWindow("dialogs/Benchmark.qml", qsTr("Benchmark"), qsTr("Benchmark"), 429, {"name" : "", "value" : ""})
            color_: palette.green
            anchors.bottomMargin: 10
        }
        UI.ButtonBordered {
            id: saveChanges
            height: 40
            text: qsTr("Save changes") + Translation.tr
            width: 120
            onClicked: top.update()
            color_: palette.blue
            anchors.bottomMargin: 10
        }
    }

    function getNumberOfCore(nb) {;
        var model = [];
        for(var i = 1; i< nb; i++)
            model.push(i);
        typePerf.model = model;
        typePerf.currentIndex = parseInt(UserSettings.getSetting("Perf-NumberOfCore")) - 1;
    }
}
