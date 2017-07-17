import QtQuick 2.7
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4
import "../" as UI

Item {
    property int currentPage: 1
    property variant types: [
        0, //container type
        0, //volume type
        0, //normal or direct mode (hidden)
        0, //volume path (standard volume)
        "", //volume path (hidden+direct volume)
        "", //volume password (hidden+direct volume)
        "", //volume path (hidden+normal volume)
        ["", ""], //name of the algorithm (standard volume) and name of the hash algorithm
        [0, ""], //volume size and type (KB, MB, GB) (standard volumes)
        ["", ""], //volume password with verification (standard volume)
        ["","", false] //file system, cluster & dynamic(bool) (standard volumes)
    ]
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
            maximumValue: 7
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
            id: help_
            color_: palette.green
            text: qsTr("Help")
        }
        UI.GSButtonBordered {
            id: back_
            color_: palette.blue
            text: qsTr("< Back")
            onClicked: manageWizard(0)
        }
        UI.GSButtonBordered {
            id: next_
            color_: palette.blue
            text: qsTr("Next >")
            onClicked: manageWizard(1)
        }
    }

    function sendAllInfos(){
        /*property variant types: [
            0, //container type
            0, //volume type
            0, //normal or direct mode (hidden)
            "", //volume path (standard volume)
            "", //volume path (hidden+direct volume)
            "", //volume password (hidden+direct volume)
            "", //volume path (hidden+normal volume)
            ["", ""], //name of the algorithm (standard volume) and name of the hash algorithm
            [0, ""], //volume size and type (KB, MB, GB) (standard volumes)
            ["", ""], //volume password with verification (standard volume)
            ["","", false] //file system, cluster & dynamic(bool) (standard volumes)
        ]*/
        // container type = don't care
        Wizard.setType(types[2])
        Wizard.setPath(types[3])
        Wizard.setPassword(types[9])
        Wizard.setEA(types[6][0])
        Wizard.setVolumeHeaderKdf(types[6][1])
        bytes = types[8][0]*1024
        if(types[8][1] === "MB")
            bytes = bytes*1024
        if(types[8][1] === "GB")
            bytes = bytes*1024*1024
        Wizard.setSize(bytes)
    }

    function manageWizard(direction)
    {
        switch(currentPage)
        {
        case 1: //mode choice
            types[0] = content.item.type
            console.log("Valeur choisie (1) : " + types[0])
            if(direction === 1)
            {
                switch(types[0])
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
                changeSubWindowTitle("Volume Type")
                content.item.setType(types[1] ? types[1] : 0)
                bar.value += 1
                currentPage++
            }
            break;

        case 2: //volume type
            //the next page depends on the choice
            types[1] = content.item.type
            console.log("Valeur choisie (2) : " + types[1])
            if(direction === 1) //1 => normal
            {
                if(types[1] === 0)
                {
                    content.source = "Page4.qml"
                    changeSubWindowTitle("Volume Location")
                    bar.value += 1
                    currentPage+=2
                }else //2 => hidden
                {
                    content.source = "Page3.qml"
                    changeSubWindowTitle("Volume Creation Mode")
                    bar.value += 1
                    currentPage++
                }
            }else{
                content.source = "Page1.qml"
                changeSubWindowTitle("GostCrypt Volume Creation Wizard")
                content.item.setType(types[0] ? types[0] : 0)
                bar.value -= 1
                currentPage--
            }
            break;

        case 3: //volume creation mode (hidden)
            types[2] = content.item.type
            console.log("Valeur choisie (3) : " + types[2])
            if(direction === 1) //1 => normal
            {
                //normal
                if(types[2] === 0)
                {
                    content.source = "Page7.qml"
                    changeSubWindowTitle("Volume Location")
                    currentPage+=4
                }else{ //direct mode (Quick)
                    content.source = "Page5.qml"
                    changeSubWindowTitle("Volume Location")
                    currentPage+=1
                }
            }else{
                content.source = "Page2.qml"
                changeSubWindowTitle("Volume Type")
                content.item.setType(types[1] ? types[1] : 0)
                bar.value -= 1
                currentPage--
            }
            break;

        case 4: //volume location
            types[3] = content.item.path
            console.log("Valeur choisie (4) : " + types[3])
            if(direction === 1 && types[3] !== "") //1 => normal
            {
                content.source = "Page8.qml"
                changeSubWindowTitle("Encryption Options")
                bar.value += 1
                currentPage+=4
            }else if(direction !== 1){
                content.source = "Page2.qml"
                changeSubWindowTitle("Volume Type")
                content.item.setType(types[1] ? types[1] : 0)
                bar.value -= 1
                currentPage-=2
            }
            break;

        case 5: //volume location (hidden/direct)
            types[4] = content.item.path
            console.log("Valeur choisie (4) : " + types[4])
            if(direction === 1 && types[4] !== "") //1 => normal
            {
                content.source = "Page6.qml"
                changeSubWindowTitle("outer Volume Password")
                currentPage+=1
            }else{
                content.source = "Page3.qml"
                changeSubWindowTitle("Volume Creation Mode")
                content.item.setType(types[2] ? types[2] : 0)
                currentPage-=2
            }
            break;

        case 6: //pasword volume (hidden/direct)
            types[5] = content.item.password
            console.log("Valeur choisie (4) : " + types[4])
            if(direction === 1 && types[5] !== "") //1 => normal
            {

            }else{
                content.source = "Page5.qml"
                changeSubWindowTitle("Volume Location")
                currentPage-=1
            }
            break;

        case 7: //volume location (hidden/normal)
            types[6] = content.item.path
            if(direction === 1 && types[6] !== "") //1 => normal
            {

            }else{
                content.source = "Page3.qml"
                changeSubWindowTitle("Volume Creation Mode")
                content.item.setType(types[2] ? types[2] : 0)
                currentPage-=4
            }
            break;
        case 8: //algorithm & hash (standard volume)
            types[7] = content.item.algoHash
            console.log("8 : " + types[7][0] + " "+ types[7][1])
            if(direction === 1) //1 => normal
            {
                content.source = "Page9.qml"
                changeSubWindowTitle("Volume Size")
                bar.value += 1
                currentPage+=1
            }else{
                content.source = "Page4.qml"
                changeSubWindowTitle("Volume Location")
                bar.value -= 1
                currentPage-=4
            }
            break;
        case 9: //volume size (standard volume)
            types[8] = content.item.sizeType
            console.log("9 : " + types[8][0] + " "+ types[8][1])
            if(direction === 1 && types[8] && types[8][0] > 0) //1 => normal
            {
                content.source = "Page10.qml"
                changeSubWindowTitle("Volume Password")
                bar.value += 1
                currentPage+=1
            }else if(direction !== 1){
                content.source = "Page8.qml"
                changeSubWindowTitle("Encryption Options")
                bar.value -= 1
                currentPage-=1
            }
            break;
        case 10: //volume password (standard volume)
            types[9] = content.item.password
            console.log("10 : " + types[9])
            //TODO : short password
            if(direction === 1 && types[9][0] !== "" && types[9][0] === types[9][1]) //1 => normal
            {
                content.source = "Page11.qml"
                changeSubWindowTitle("Volume Format")
                next_.text = qsTr("Format")
                bar.value += 1
                currentPage+=1
            }else if(direction !== 1){
                content.source = "Page9.qml"
                changeSubWindowTitle("Volume Password")
                bar.value -= 1
                currentPage-=1
            }else{
                openErrorMessage("Different passwords", "The passwords are different or empties. <br>Please try again.")
            }

            break;
        case 11: //format volume (standard volume)
            types[10] = content.item.format
            console.log("11 : " + types[9])
            sendAllInfos()
            Wizard.createVolume()
            //TODO : create volume here
            if(direction === 1) //1 => normal
            {
                content.source = "PageEnd.qml"
                changeSubWindowTitle("GostCrypt Volume Creation Wizard")
                back_.visible = false
                next_.visible = false
                help_.visible = false
                bar.value += 1
                currentPage+=1
            }else if(direction !== 1){
                content.source = "Page10.qml"
                changeSubWindowTitle("Volume password")
                next_.text = qsTr("Next >")
                bar.value -= 1
                currentPage-=1
            }
            break;
        case 12: //Done !
            //TODO : create volume here
            if(direction !== 1){
                content.source = "Page11.qml"
                changeSubWindowTitle("Volume Format")
                next_.text = qsTr("Format")
                back_.visible = false
                next_.visible = false
                help_.visible = false
                bar.value -= 1
                currentPage-=1
            }
            break;
        }

    }
}
