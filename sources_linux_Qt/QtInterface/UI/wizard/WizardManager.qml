import QtQuick 2.7
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4
import "../" as UI

Item {
    property int currentPage: 1
    signal next()
    signal back()
    signal cancel()
    signal help()
    x:0
    y:0
    anchors.topMargin: 0

    Rectangle {
        color: palette.darkThird
        width: parent.width
        height: 20

        ProgressBar {
            id: bar
            width: parent.width
            height: 2
            maximumValue: 8
            value: 0
            y:20
            style: ProgressBarStyle {
                background: Rectangle {
                    implicitWidth: bar.width
                    implicitHeight: bar.height
                    color: palette.text
                    border.color: palette.border
                    border.width: 1
                }
                progress: Rectangle {
                    color: palette.blue
                }
            }
        }
    }

    Loader {
        id:content
        source: "Page1.qml"
        x:0
        y:30
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width-40
        height: parent.height-40
    }

    Row {
        spacing: 22
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 5

        UI.GSButtonBordered {
            color_: palette.green
            text: qsTr("Help")
        }
        UI.GSButtonBordered {
            color_: palette.blue
            text: qsTr("< Back")
            onClicked: manageWizard(0)
        }
        UI.GSButtonBordered {
            color_: palette.blue
            text: qsTr("Next >")
            onClicked: manageWizard(1)
        }
    }

    function manageWizard(direction)
    {
        switch(currentPage)
        {
        case 1: //mode choice
            if(direction === 1)
            {
                var type = loader.item.type
                switch(type)
                {
                    //TODO : bind with C++
                case 0:
                    //Wizard.
                    break;
                case 1:
                    break;
                case 2:
                    break;
                }
                content.source = "Page2.qml"
                bar.value += 1
                currentPage++
            }
            break;

        case 2: //volume type
            //the next page depends on the choice
            var type2 = loader.item.type

            if(direction === 1) //1 => normal
            {
                if(type2 === 0)
                {
                    content.source = "Page4.qml"
                    bar.value += 2
                    currentPage+=2
                }else //2 => hidden
                {
                    content.source = "Page3.qml"
                    bar.value += 1
                    currentPage++
                }
            }else{
                content.source = "Page1.qml"
                bar.value -= 1
                currentPage--
            }
            break;

        case 3: //volume type
            //the next page depends on the choice
            var type3 = loader.item.type

            if(direction === 1) //1 => normal
            {
                content.source = "Page4.qml"
                bar.value += 1
                currentPage+=1
            }else{
                content.source = "Page2.qml"
                bar.value -= 1
                currentPage--
            }
            break;

        case 4: //volume type
            //the next page depends on the choice
            var type4 = loader.item.type

            if(direction === 1) //1 => normal
            {
                content.source = "Page5.qml"
                bar.value += 1
                currentPage+=1
            }else{
                if(type2 === 0)
                {
                    content.source = "Page2.qml"
                    bar.value -= 2
                    currentPage-=2
                }else //2 => hidden
                {
                    content.source = "Page3.qml"
                    bar.value -= 1
                    currentPage--
                }
            }
            break;
        }
    }
}
