import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4 as OLDControl
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
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

    Row {
        id: topRow
        y: 15
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10

        Row {
            spacing: 5
            Text {
                text: qsTr("Buffer Size:")+ "  "
                color: palette.text
                font.pointSize: 11
                verticalAlignment: Text.AlignVCenter
                height: 40
            }
            UI.GSCustomComboBox {
                id: combo
                width: 250
                height: 40
                model: {
                    var paths = ["100 KiB", "500 KiB", "1 MiB", "5 MiB", "100 MiB", "200 MiB", "500 MiB", "1 GiB"];
                    return paths;
                }
            }
        }

        UI.GSButtonBordered {
            id: selectPKCS
            height: combo.height
            text: qsTr("Benchmark") + Translation.tr
            width: 120
            onClicked: qmlRequest("benchmark", "")
            color_: palette.green
        }

    }

    Row {
        id: tableRow
        width: 550
        height: 150
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: topRow.bottom
        anchors.topMargin: 10
        spacing: 10


        OLDControl.TableView {
            id: table
            width: 550
            OLDControl.TableViewColumn {
                role: "algo"
                title: "Algorithm"
                width: 125
            }
            OLDControl.TableViewColumn {
                role: "enc"
                title: "Encryption speed"
                width: 125
            }
            OLDControl.TableViewColumn {
                role: "dec"
                title: "Decryption speed"
                width: 125
            }
            OLDControl.TableViewColumn {
                role: "mean"
                title: "mean"
                width: 125
            }
            style: TableViewStyle {
                frame: Rectangle {
                    border{
                        color: palette.darkInput
                    }
                }
                headerDelegate: Rectangle {
                    height: textItem.implicitHeight * 1.2
                    width: textItem.implicitWidth
                    color: palette.darkInput
                    Text {
                        id: textItem
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: styleData.textAlignment
                        anchors.leftMargin: 12
                        text: styleData.value
                        elide: Text.ElideRight
                        color: palette.text
                        renderType: Text.NativeRendering
                    }
                    Rectangle {
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 1
                        anchors.topMargin: 1
                        width: 1
                        color: palette.darkInput
                    }
                }
                rowDelegate: Rectangle {
                    color: {
                        var baseColor = styleData.alternate ? palette.darkSecond : palette.darkThird
                        return styleData.selected ? palette.dark : baseColor
                    }
                }

                activateItemOnSingleClick : true
                alternateBackgroundColor : palette.darkSecond
                backgroundColor : palette.darkThird
                highlightedTextColor : palette.blue
                textColor : palette.text


            }

            model: ListModel {
                id: libraryModel
            }
        }
    }

    Row {
        width: 550
        height: 150
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: tableRow.bottom
        anchors.topMargin: 10

        Text {
            text: qsTr("Speed is affected by CPU load and storage \
                        device characteristics.<br>Theses tests take place in RAM.\
                        <br>According to the user preferences, ") + numberOfCores() + qsTr(" cores are used.")
            width: 550
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHLeft
            color: palette.text
            font.pointSize: 11
        }

    }


    Row {
        spacing: 15
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        UI.GSButtonBordered {
            id: save
            height: 40
            text: qsTr("Close") + Translation.tr
            width: 120
            onClicked: top.update()
            color_: palette.blue
            anchors.bottomMargin: 10
        }
    }

    function updateTableview(object) {
        table.model.clear();
        //var keyfiles = UserSettings.getFavoriteKeyFiles();
        //for(var i = 0; i < keyfiles.length; ++i) table.model.append({ path: keyfiles[i] })
        //TODO : MAJ table with returned valued
        table.model.append({ algo: "AAAAAAAAA", enc: "352MB/s", dec: "248Mb/s", mean: "245Mb/s" })
    }

    function numberOfCores() {
        if(UserSettings.getSetting("Perf-Use") === "1") return parseInt(UserSettings.getSetting("Perf-NumberOfCore"));
        return UserSettings.getNumberOfCore();
    }
}
