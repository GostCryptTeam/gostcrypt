import QtQuick 2.7
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4
import "../" as UI

Item {
    property int currentPage: 1
    property variant types: [
        0,              //container type
        0,              //volume type
        0,              //normal or direct mode (hidden)
        "",             //volume path (standard/hidden volume)
        "",             //volume password (hidden+direct volume)
        ["", ""],       //name of the algorithm (standard volume) and name of the hash algorithm
        [0, ""],        //volume size and type (KB, MB, GB) (standard volumes)
        ["", ""],       //volume password with verification (standard volume)
        ["","", false], //file system, cluster & dynamic(bool) (standard volumes)
        ["", ""],       //HIDDEN VOLUME : algorithm & hash
        [0, ""],        //HIDDEN VOLUME: volume size
        ["", ""],       //HIDDEN VOLUME: volume password (with verification)
        ["","", false], //HIDDEN VOLUME: file system, cluster & dynamic(bool) (standard volumes)
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
            maximumValue: 100
            value: 0
            y:20
            Behavior on value {
                NumberAnimation { duration: app.duration; easing.type: Easing.OutQuad; }
            }
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
        var typeBranch = 0;

        switch(currentPage)
        {
        case 1: //mode choice
            types[0] = content.item.type
            manageProgressBar(1,direction,0)
            if(direction === 1)
            {
                content.source = "Page2.qml"
                changeSubWindowTitle("Volume Type")
                content.item.setType(types[1] ? types[1] : 0)
                currentPage++
            }
            break;

        case 2: //volume type
            //the next page depends on the choice
            types[1] = content.item.type
            manageProgressBar(2,direction,types[1])
            if(direction === 1) //1 => normal
            {
                if(types[1] === 0)
                {
                    content.source = "Page4.qml"
                    content.item.message = qsTr("A GostCrypt volume can reside in a file (called GostCrypt container),"
                                           +" which can reside on a hard disk, on a USB flash drive, etc. A GostCrypt"
                                           +" container is just like any normal file (it can be, for example, moved or deleted as"
                                           +" any normal file). Click 'Select File' to choose a filename for the container and"
                                           +" to select the location where you wish the container to be created.<br><br><b>WARNING</b>: If you select"
                                           +" an existing file, GostCrypt will NOT encrypt it; the file will be deleted and replaced with"
                                           +" the newly created GostCrypt container. You will be able to encrypt existing giles (later"
                                           +" on) by moving them to the GostCrypt container that you are about to create now.")
                    content.item.type = 0
                    changeSubWindowTitle("Volume Location")
                    currentPage+=2
                }else //2 => hidden
                {
                    content.source = "Page3.qml"
                    changeSubWindowTitle("Volume Creation Mode")
                    currentPage++
                }
            }else{
                content.source = "Page1.qml"
                changeSubWindowTitle("GostCrypt Volume Creation Wizard")
                content.item.setType(types[0] ? types[0] : 0)
                currentPage--
            }
            break;

        case 3: //volume creation mode (hidden)
            types[2] = content.item.type
            manageProgressBar(3,direction,types[2])
            if(direction === 1) //1 => normal
            {
                //normal
                if(types[2] === 0)
                {
                    content.source = "Page4.qml"
                    content.item.message = qsTr("Select the location of the outer volume to be created (within its volume the hidden volume will be created later on)"
                                       +"<br>"
                                       +"A GostCrypt volume can reside in a file (called GostCrypt container),"
                                       +" which can reside on a hard disk, on a USB flash drive, etc. A GostCrypt"
                                       +" container is just like any normal file (it can be, for example, moved or deleted as"
                                       +" any normal file). Click 'Select File' to choose a filename for the container and"
                                       +" to select the location where you wish the container to be created.<br><b>WARNING</b>: If you select"
                                       +" an existing file, GostCrypt will NOT encrypt it; the file will be deleted and replaced with"
                                       +" the newly created GostCrypt container. You will be able to encrypt existing giles (later"
                                       +" on) by moving them to the GostCrypt container that you are about to create now.")
                    content.item.type = 2
                    content.item.setFileDialog(false)
                    changeSubWindowTitle("Volume Location")
                    currentPage+=1
                }else{ //direct mode (Quick)
                    content.source = "Page4.qml"
                    content.item.message = qsTr("Select the location of the GostCrypt volume within which you wish to create a hidden volume.")
                    content.item.type = 1
                    content.item.setFileDialog(true)
                    changeSubWindowTitle("Volume Location")
                    currentPage+=1
                }
            }else{
                content.source = "Page2.qml"
                changeSubWindowTitle("Volume Type")
                content.item.setType(types[1] ? types[1] : 0)
                currentPage--
            }
            break;

        case 4: //volume location
            types[3] = content.item.path
            if(direction === 1 && types[3] !== "") //1 => normal
            {
                manageProgressBar(4,direction,content.item.type)
                switch (content.item.type) {
                case 0:
                    content.source = "Page7.qml"
                    changeSubWindowTitle("Encryption Options")
                    currentPage+=3
                    break;
                case 1:
                    content.source = "Page5.qml"
                    changeSubWindowTitle("Volume Password")
                    currentPage+=1
                    break;
                case 2:
                    content.source = "Page6.qml"
                    changeSubWindowTitle(qsTr("Outer Volume"))
                    currentPage+=2
                    break;
                }
            }else if(direction !== 1){
                manageProgressBar(4,direction,content.item.type)
                switch (content.item.type) {
                case 0:
                    content.source = "Page2.qml"
                    changeSubWindowTitle("Volume Type")
                    content.item.setType(types[1] ? types[1] : 0)
                    currentPage-=2
                    break;
                case 1:
                case 2:
                    content.source = "Page3.qml"
                    changeSubWindowTitle("Volume Creation Mode")
                    content.item.setType(types[2] ? types[2] : 0)
                    currentPage-=1
                    break;
                }

            }
            break;

        case 5: //volume password (hidden/direct)
            console.log("password is :" + content.item.password)
            types[4] = content.item.password
            if(direction === 1 && types[4] !== "") //1 => normal
            {
                content.source = "Page12.qml"
                back_.setDisable(1)
                changeSubWindowTitle("Hidden Volume")
                manageProgressBar(5,direction,3)
                currentPage+=7
                content.item.type = 3
            }else if(direction !== 1){
                content.source = "Page4.qml"
                content.item.message = qsTr("Select the location of the GostCrypt volume within which you wish to create a hidden volume.")
                content.item.type = 1
                content.item.setFileDialog(true)
                changeSubWindowTitle("Volume Location")
                currentPage-=1
                manageProgressBar(5,direction,0)
            }
            break;

        case 6: //outer volume message (hidden/normal)
            if(direction === 1)
            {
                content.source = "Page7.qml"
                changeSubWindowTitle("Encryption Options")
                manageProgressBar(6,direction,0)
                currentPage+=1
                content.item.type = 1
            }else{
                content.source = "Page4.qml"
                content.item.message = qsTr("Select the location of the outer volume to be created (within its volume the hidden volume will be created later on)"
                                   +"<br>"
                                   +"A GostCrypt volume can reside in a file (called GostCrypt container),"
                                   +" which can reside on a hard disk, on a USB flash drive, etc. A GostCrypt"
                                   +" container is just like any normal file (it can be, for example, moved or deleted as"
                                   +" any normal file). Click 'Select File' to choose a filename for the container and"
                                   +" to select the location where you wish the container to be created.<br><b>WARNING</b>: If you select"
                                   +" an existing file, GostCrypt will NOT encrypt it; the file will be deleted and replaced with"
                                   +" the newly created GostCrypt container. You will be able to encrypt existing giles (later"
                                   +" on) by moving them to the GostCrypt container that you are about to create now.")
                content.item.type = 2
                content.item.setFileDialog(false)
                changeSubWindowTitle("Volume Location")
                currentPage-=2
                manageProgressBar(6,direction,0)
            }
            break;

        case 7: //algorithm & hash (standard volume)
            typeBranch = content.item.type
            console.log("ici type vaut " + typeBranch)
            manageProgressBar(7,direction,typeBranch)
            //type 0 & 1 (normal) => types[5], else types[9]
            if(content.item.type !== 2)
                types[5] = content.item.algoHash
            else
                types[9] = content.item.algoHash
            if(direction === 1)
            {
                content.source = "Page8.qml"
                changeSubWindowTitle("Volume Size")
                currentPage+=1
                content.item.type = typeBranch
            }else{
                switch(content.item.type) {
                case 0:
                    //go to page4
                    content.source = "Page4.qml"
                    content.item.message = qsTr("A GostCrypt volume can reside in a file (called GostCrypt container),"
                                           +" which can reside on a hard disk, on a USB flash drive, etc. A GostCrypt"
                                           +" container is just like any normal file (it can be, for example, moved or deleted as"
                                           +" any normal file). Click 'Select File' to choose a filename for the container and"
                                           +" to select the location where you wish the container to be created.<br><br><b>WARNING</b>: If you select"
                                           +" an existing file, GostCrypt will NOT encrypt it; the file will be deleted and replaced with"
                                           +" the newly created GostCrypt container. You will be able to encrypt existing giles (later"
                                           +" on) by moving them to the GostCrypt container that you are about to create now.")
                    content.item.type = 0
                    content.item.setFileDialog(false)
                    changeSubWindowTitle("Volume Location")
                    currentPage-=3
                    break;
                case 1:
                        //go to outer volume page
                        content.source = "Page6.qml"
                        changeSubWindowTitle(qsTr("Outer Volume"))
                        currentPage--
                    break;
                case 2:
                case 3:
                    //go to outer volume page
                    content.source = "Page12.qml"
                    back_.setDisable(1)
                    changeSubWindowTitle(qsTr("Hidden Volume"))
                    currentPage+=5
                    content.item.type = typeBranch
                    break;
                }
            }
            break;
        case 8://volume size (standard volume)
            console.log("branch is " + content.item.type)
            typeBranch = content.item.type
            //type 0 & 1 (normal) => types[6], else types[10]
            if(content.item.type !== 2)
                types[6] = content.item.sizeType
            else
                types[10] = content.item.sizeType
            if(direction === 1
                    && content.item.sizeType
                    && content.item.sizeType[0] > 0) //1 => normal
            {
                manageProgressBar(8,direction,typeBranch)
                content.source = "Page9.qml"
                changeSubWindowTitle("Volume Password")
                currentPage+=1
                content.item.type = typeBranch
                manageProgressBar(8,direction,typeBranch)
            }else if(direction !== 1){
                manageProgressBar(8,direction,typeBranch)
                content.source = "Page7.qml"
                changeSubWindowTitle("Encryption Options")
                currentPage-=1
                content.item.type = typeBranch
            }
            break;
        case 9: //volume password (standard volume)
            typeBranch = content.item.type
            console.log(content.item.password[0])
            console.log(content.item.password[1])
            //type 0 & 1 (normal) => types[7], else types[11]
            if(content.item.type !== 2) {
                types[7][0] = content.item.password[0]
                types[7][1] = content.item.password[1]
            }else {
                types[11][0] = content.item.password[0]
                types[11][1] = content.item.password[1]
            }
            //TODO : short password
            if(direction === 1
                    && content.item.password[0] !== ""
                    && content.item.password[0] === content.item.password[1]) //1 => normal
            {
                manageProgressBar(9,direction,typeBranch)
                console.log("type vaut " + content.item.type)
                content.source = "Page10.qml"
                changeSubWindowTitle("Volume Format")
                currentPage+=1
                content.item.type = typeBranch
                if(typeBranch === 0 || typeBranch === 2)
                    next_.text = qsTr("Format")
            }else if(direction !== 1){
                manageProgressBar(9,direction,typeBranch)
                content.source = "Page8.qml"
                changeSubWindowTitle("Volume Size")
                currentPage-=1
                content.item.type = typeBranch
            }else{
                openErrorMessage("Different passwords", "The passwords are different or empties. <br>Please try again.")
            }
            break;
        case 10:
             //format volume (standard volume)
            if(content.item.type !== 2) {
                types[8][0] = content.item.format[0]
                types[8][1] = content.item.format[1]
                types[8][2] = content.item.format[2]
            }else {
                types[12][0] = content.item.format[0]
                types[12][1] = content.item.format[1]
                types[12][2] = content.item.format[2]
            }
            typeBranch = content.item.type
            manageProgressBar(10,direction,typeBranch)
            //TODO : create volume here
            if(direction === 1) //1 => normal
            {
                switch(content.item.type) {
                case 0:
                    //TODO : creation volume normal mode
                    sendAllInfos()
                    Wizard.createVolume()
                    content.source = "PageEnd.qml"
                    changeSubWindowTitle("GostCrypt Volume Creation Wizard")
                    back_.visible = false
                    next_.visible = false
                    help_.visible = false
                    break;
                case 1:
                    content.source = "Page11.qml"
                    changeSubWindowTitle(qsTr("Outer Volume Contents"))
                    back_.setDisable(1)
                    currentPage+=1
                    break;
                case 2:
                case 3:
                    //TODO : create hidden volume here
                    openErrorMessage(qsTr("Warning"),
                                     qsTr("The hidden GostCrypt volume has been successfully created and is ready for use. If all "
                                          + "the instructions have been followed and if the precautions and requirements listed in the section "
                                          + "\"Security Requirements and Precautions Pertaining to Hidden Volumes\" in the GostCrypt User's Guide "
                                          + "are followed, it should be impossible to prove that the hidden volume exists, event when the outer "
                                          + "volume is mounted.<br><br>WARNING: IF YOU DO NOT PROTECT THE HIDDEN VOLUME (FOR INFORMATION ON HOW "
                                          + "TO DO SO, REFER TO THE SECTION \"PROTECTION OF HIDDEN VOLUMES AGAINST DAMAGE\" IN THE GOSTCRYPT USER'S "
                                          + "GUIDE), DO NOT WRITE TO THE OUTER VOLUME. OTHERWISE, YOU MAY OVERWRITE AND DAMAGE THE HIDDEN VOLUME!"), 13)
                    content.source = "PageEnd.qml"
                    changeSubWindowTitle("GostCrypt Volume Creation Wizard")
                    back_.visible = false
                    next_.visible = false
                    help_.visible = false
                    break;
                }
            }else if(direction !== 1){
                content.source = "Page9.qml"
                changeSubWindowTitle("Volume password")
                next_.text = qsTr("Next >")
                currentPage-=1
                content.item.type = typeBranch
            }

            break;

        case 11:
            typeBranch = content.item.type
            manageProgressBar(11,direction,0)
            if(direction === 1) //1 => normal
            {
                content.source = "Page12.qml"
                changeSubWindowTitle("Hidden Volume")
                back_.setDisable(1)
                currentPage+=1
                content.item.type = typeBranch
            }
            break;

        case 12:
            typeBranch = content.item.type
            manageProgressBar(12,direction,typeBranch)
            if(direction === 1) //1 => normal
            {
                content.source = "Page7.qml"
                changeSubWindowTitle("Encryption Options")
                currentPage-=5
                back_.setDisable(0)
                content.item.type = typeBranch
            }
            break;
        }

    }

    function manageProgressBar(posInitial, direction, branch)
    {
        console.log("changement de fenêtre : "+posInitial +" "+direction+ " "+ branch)
        switch(posInitial) {
        case 1:
            if(direction === 1)
                bar.value = 5
            break;
        case 2:
            if(direction === 1)
                if(branch === 0)
                    bar.value = 20
                else
                    bar.value = 10
             else
                bar.value = 0
            break;
        case 3:
            if(direction === 1)
                bar.value = 20
            else
                bar.value = 5
            break;
        case 4:
            if(direction === 1) {
                switch(branch){
                case 0:
                    bar.value = 40
                    break;
                case 1:
                    bar.value = 25
                    break;
                case 2:
                    bar.value = 30
                    break;
                }
            }else{
                switch(branch){
                case 0:
                    bar.value = 5
                    break;
                case 1:
                    bar.value = 10
                    break;
                case 2:
                    bar.value = 10
                    break;
                }
            }
            break;
        case 5:
            if(direction === 1)
                bar.value = 35
            else
                bar.value = 20
            break;
        case 6:
            if(direction === 1)
                bar.value = 40
            else
                bar.value = 20
            break;
        case 7:
            if(direction === 1) {
                switch(branch){
                case 0:
                    bar.value = 50
                    break;
                case 1:
                case 3:
                    bar.value = 50
                    break;
                case 2:
                    bar.value = 88
                    break;
                }
            }else{
                switch(branch){
                case 0:
                    bar.value = 20
                    break;
                case 1:
                case 3:
                    bar.value = 30
                    break;
                case 2:
                    bar.value = 80
                    break;
                }
            }
            break;
        case 8:
            if(direction === 1) {
                switch(branch){
                case 0:
                    bar.value = 60
                    break;
                case 1:
                case 3:
                    bar.value = 60
                    break;
                case 2:
                    bar.value = 92
                    break;
                }
            }else{
                switch(branch){
                case 0:
                    bar.value = 40
                    break;
                case 1:
                case 3:
                    bar.value = 40
                    break;
                case 2:
                    bar.value = 82
                    break;
                }
            }
            break;
        case 9:
            if(direction === 1) {
                switch(branch){
                case 0:
                case 3:
                    bar.value = 85
                    break;
                case 1:
                    bar.value = 70
                    break;
                case 2:
                    bar.value = 96
                    break;
                }
            }else{
                switch(branch){
                case 0:
                    bar.value = 50
                    break;
                case 1:
                case 3:
                    bar.value = 50
                    break;
                case 2:
                    bar.value = 88
                    break;
                }
            }
            break;
        case 10:
            if(direction === 1) {
                switch(branch){
                case 0:
                case 3:
                    bar.value = 100
                    break;
                case 1:
                    bar.value = 75
                    break;
                case 2:
                    bar.value = 100
                    break;
                }
            }else{
                switch(branch){
                case 0:
                case 3:
                    bar.value = 60
                    break;
                case 1:
                    bar.value = 60
                    break;
                case 2:
                    bar.value = 92
                    break;
                }
            }
            break;
        case 11:
            if(direction === 1)
                bar.value = 80
            break;
        case 12:
            if(direction === 1)
                if(branch !== 3)
                    bar.value = 82
                else
                    bar.value = 40
            break;
        }
    }
}
