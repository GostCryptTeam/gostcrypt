import QtQuick 2.7
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4
import "../" as UI

Item {
    property alias page : content.item
    property int currentPage: 1
    property var volumeInfos: {
        "CONTAINER_TYPE": 0,
        "VOLUME_TYPE": 0,
        "NORMAL_OR_HIDDEN": 0,                  //normal or direct mode (hidden)
        "VOLUME_PATH": "",                      //volume path (standard/hidden volume)
        "VOLUME_PWD": "",                       //volume password (hidden+direct volume)
        "ALGORITHM_HASH_NAMES": ["", ""],       //name of the algorithm (standard volume) and name of the hash algorithm
        "VOLUME_SIZE": [0, ""],                 //volume size and type (KB, MB, GB) (standard volumes)
        "VOLUME_NEW_PASSWORD": ["", ""],        //volume password with verification (standard volume)
        "FORMAT_INFOS": ["", "", false],        //file system, cluster & dynamic(bool) (standard volumes)
        "HIDDEN_ALGORITHM_HASH": ["", ""],      //HIDDEN VOLUME : algorithm & hash
        "HIDDEN_VOLUME_SIZE": [0, ""],          //HIDDEN VOLUME: volume size
        "HIDDEN_VOLUME_PASSWORD": ["",""],      //HIDDEN VOLUME: volume password (with verification)
        "HIDDEN_FORMAT_INFOS":  ["","", false]  //HIDDEN VOLUME: file system, cluster & dynamic(bool) (standard volumes)*/
    }
    property var progress: {
        "VOLUME_TYPE": 1,
        "VOLUME_ISHIDDEN": 2,
        "DIRECT_NORMAL": 3,
        "VOLUME_PATH": 4,
        "VOLUME_DIRECT_PWD": 5,
        "VOLUME_OUTER_VOLUME": 6,
        "VOLUME_ALGO": 7,
        "VOLUME_SIZE": 8,
        "VOLUME_PWD": 9,
        "VOLUME_FORMAT": 10,
        "VOLUME_OUTER_CONTENTS": 11,
        "VOLUME_HIDDEN_VOLUME": 12,
        "VOLUME_END": 666
    }

    signal next()
    signal back()
    signal cancel()
    signal help()

    x:0
    y:0
    anchors.topMargin: 0

    Loader {
        id:content
        source: "Page1.qml"
        x:0
        y:0
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

    Row {
        id: steps
        spacing: 30
        anchors.horizontalCenter: parent.horizontalCenter
        y: 275
        StepComponent {
            id: step1
            checked: true
        }
        StepComponent {
            id: step2
        }
        StepComponent {
            id: step3
        }
        StepComponent {
            id: step4
        }
        StepComponent {
            id: step5
        }

    }

    NextPreviousButton {
        id: previous
        x: -60
        y: 270/2 - 70
        onPressed: manageWizard(-1)
        visible: false
    }

    NextPreviousButton {
        id: next
        x: parent.width + 20
        y: 270/2 - 70
        type: true
        onPressed: manageWizard(1)
        visible: false
    }

    function manageButtons(a_,b_)
    {
        previous.visible = a_;
        next.visible = b_;
    }

    function manageWizard(direction)
    {
        var typeBranch = 0;
        switch(currentPage)
        {


            /* Page1.qml
             * * Create an encrypted file container
             * * Encrypt a non-system partition/volume
             */
        case progress.VOLUME_TYPE:
            volumeInfos.CONTAINER_TYPE = content.item.type
            manageProgressBar(1,direction,0)
            if(direction === 1)
            {
                changePage(2, qsTr("Volume Type"), currentPage)
                manageButtons(true, false);
            }
            break;



            /* Page2.qml
             * * Standard GostCrypt Volume
             * * Hidden GostCrypt Volume
             */
        case progress.VOLUME_ISHIDDEN:
            volumeInfos.VOLUME_TYPE = content.item.type
            manageProgressBar(2,direction,content.item.type)
            if(direction === 1) //1 => normal
            {
                // Choice: Standard GostCrypt Volume
                if(volumeInfos.VOLUME_TYPE === 0)
                {
                    changePage(4, qsTr("Volume Location"), currentPage)
                    manageButtons(true, true);
                    content.source = "Page4.qml"
                    content.item.message = qsTr("A GostCrypt volume can reside in a file (called GostCrypt container),"
                                           +" which can reside on a hard disk, on a USB flash drive, etc. A GostCrypt"
                                           +" container is just like any normal file (it can be, for example, moved or deleted as"
                                           +" any normal file). Click 'Select File' to choose a filename for the container and"
                                           +" to select the location where you wish the container to be created.<br><br><b>WARNING</b>: If you select"
                                           +" an existing file, GostCrypt will NOT encrypt it; the file will be deleted and replaced with"
                                           +" the newly created GostCrypt container. You will be able to encrypt existing files (later"
                                           +" on) by moving them to the GostCrypt container that you are about to create now.")
                    content.item.type = 0
                }else { //Choice : Hideden GostCrypt Volume
                    changePage(3, qsTr("Volume Creation Mode"), currentPage)
                    manageButtons(true, true);
                }
            }else{
                changePage(1, qsTr("GostCrypt Volume Creation Wizard"), currentPage)
                manageButtons(false, false);
            }
            break;



            /* Page3.qml (hidden volume)
             * * Normal mode
             * * Direct mode
             */
        case progress.DIRECT_NORMAL:
            volumeInfos.NORMAL_OR_HIDDEN = content.item.type
            manageProgressBar(3,direction,volumeInfos.NORMAL_OR_HIDDEN)
            if(direction === 1) //1 => normal
            {
                changePage(4, qsTr("Volume Location"), currentPage)
                //normal
                if(volumeInfos.NORMAL_OR_HIDDEN === 0)
                {
                    content.item.message = qsTr("Select the location of the outer volume to be created (within its volume the hidden volume will be created later on)"
                                       +"<br>"
                                       +"A GostCrypt volume can reside in a file (called GostCrypt container),"
                                       +" which can reside on a hard disk, on a USB flash drive, etc. A GostCrypt"
                                       +" container is just like any normal file (it can be, for example, moved or deleted as"
                                       +" any normal file). Click 'Select File' to choose a filename for the container and"
                                       +" to select the location where you wish the container to be created.<br><b>WARNING</b>: If you select"
                                       +" an existing file, GostCrypt will NOT encrypt it; the file will be deleted and replaced with"
                                       +" the newly created GostCrypt container. You will be able to encrypt existing files (later"
                                       +" on) by moving them to the GostCrypt container that you are about to create now.")
                    content.item.type = 2
                    content.item.setFileDialog(false)
                }else{ //direct mode (Quick)
                    content.item.message = qsTr("Select the location of the GostCrypt volume within which you wish to create a hidden volume.")
                    content.item.type = 1
                    content.item.setFileDialog(true)
                }
            }else{
                changePage(2, qsTr("Volume Type"), currentPage)
                manageButtons(true, false);
            }
            break;



            /* Page4.qml
             * Volume path
             * type 0 : normal
             * type 1 : path of an existing volume (hidden)
             * type 2 : path of a new volume (hidden)
             */
        case progress.VOLUME_PATH:
            volumeInfos.VOLUME_PATH = content.item.path
            if(direction === 1 && volumeInfos.VOLUME_PATH !== "") //1 => normal
            {
                manageProgressBar(4,direction,content.item.type)
                switch (content.item.type) {
                case 0:
                    changePage(7, qsTr("Encryption Options"), currentPage)
                    qmlRequest("algorithms", "")
                    break;
                case 1:
                    changePage(5, qsTr("Volume Password"), currentPage)
                    break;
                case 2:
                    changePage(6, qsTr("Outer Volume"), currentPage)
                    break;
                }
            }else if(direction !== 1){
                manageProgressBar(4,direction,content.item.type)
                switch (content.item.type) {
                case 0:
                    changePage(2, qsTr("Volume Type"), currentPage)
                    manageButtons(true, false);
                    break;
                case 1:
                case 2:
                    changePage(3, qsTr("Volume Creation Mode"), currentPage)
                    content.item.setType(volumeInfos.NORMAL_OR_HIDDEN ? volumeInfos.NORMAL_OR_HIDDEN : 0)
                    break;
                }
            }
            break;


            /* Page5.qml
             * Existing volume password (hidden)
             */
        case progress.VOLUME_DIRECT_PWD: //volume password (hidden/direct)
            volumeInfos.VOLUME_PWD = content.item.password
            if(direction === 1 && volumeInfos.VOLUME_PWD !== "") //1 => normal
            {
                changePage(12, qsTr("Hidden Volume"), currentPage)
                back_.setDisable(1)
                manageProgressBar(5,direction,3)
                content.item.type = 3
            }else if(direction !== 1){
                changePage(4, qsTr("Volume Location"), currentPage)
                content.item.message = qsTr("Select the location of the GostCrypt volume within which you wish to create a hidden volume.")
                content.item.type = 1
                content.item.setFileDialog(true)
                manageProgressBar(5,direction,0)
            }
            break;



            /* Page6.qml
             * Outer Volume information (hidden/normal)
             */
        case progress.VOLUME_OUTER_VOLUME: //outer volume message (hidden/normal)
            if(direction === 1)
            {
                changePage(7, qsTr("Encryption Options"), currentPage)
                qmlRequest("algorithms", "")
                manageProgressBar(6,direction,0)
                content.item.type = 1
            }else{
                changePage(4, qsTr("Volume Location"), currentPage)
                content.item.message = qsTr("Select the location of the outer volume to be created (within its volume the hidden volume will be created later on)"
                                   +"<br>"
                                   +"A GostCrypt volume can reside in a file (called GostCrypt container),"
                                   +" which can reside on a hard disk, on a USB flash drive, etc. A GostCrypt"
                                   +" container is just like any normal file (it can be, for example, moved or deleted as"
                                   +" any normal file). Click 'Select File' to choose a filename for the container and"
                                   +" to select the location where you wish the container to be created.<br><b>WARNING</b>: If you select"
                                   +" an existing file, GostCrypt will NOT encrypt it; the file will be deleted and replaced with"
                                   +" the newly created GostCrypt container. You will be able to encrypt existing files (later"
                                   +" on) by moving them to the GostCrypt container that you are about to create now.")
                content.item.type = 2
                content.item.setFileDialog(false)
                manageProgressBar(6,direction,0)
            }
            break;



            /* Page7.qml
             * Algorithm and hash used to format the current volume
             */
        case progress.VOLUME_ALGO: //algorithm & hash (standard volume)
            typeBranch = content.item.type
            manageProgressBar(7,direction,typeBranch)
            //type 0 & 1 (normal) => volumeInfos.ALGORITHM_HASH_NAMES, else volumeInfos.HIDDEN_ALGORITHM_HASH
            if(content.item.type !== 2)
                volumeInfos.ALGORITHM_HASH_NAMES = content.item.used
            else
                volumeInfos.HIDDEN_ALGORITHM_HASH = content.item.used
            if(direction === 1)
            {
                if(typeBranch !== 3 && typeBranch !== 2) {
                    changePage(8, qsTr("Volume Size"), currentPage)
                    content.item.setText(qsTr("<b>Free space on drive : ") + "50Go"/*Wizard.getfreeSpace()*/+"</b>",
                                         qsTr("Please specify the size of the container you want to create.<br><br>If"
                                              +" you create a dynamic (sparse-file) container, this parameter will specify its maximum possible size."
                                              +"<br><br>Note that possible size of an NTFS volume is 3792 KB."))
                }
                else {
                    changePage(8, qsTr("Hidden Volume Size"), currentPage)
                    content.item.setText(qsTr("<b>Maximum possible hidden volume size for this volume is " + "50Go"/*Wizard.getfreeSpace()*/+"</b>"),
                                         qsTr("Please specify the size of the hidden volume to create. The minimum possible "
                                              +"size of a hidden volume is 40KB (or 3664KB if it is fortmatted as NTFS). "
                                              +"The maximum possible size you can specify for the hidden volume is displayed above."))
                }
                content.item.type = typeBranch
            }else{
                switch(content.item.type) {
                case 0:
                    changePage(4, qsTr("Volume Location"), currentPage)
                    content.item.message = qsTr("A GostCrypt volume can reside in a file (called GostCrypt container),"
                                           +" which can reside on a hard disk, on a USB flash drive, etc. A GostCrypt"
                                           +" container is just like any normal file (it can be, for example, moved or deleted as"
                                           +" any normal file). Click 'Select File' to choose a filename for the container and"
                                           +" to select the location where you wish the container to be created.<br><br><b>WARNING</b>: If you select"
                                           +" an existing file, GostCrypt will NOT encrypt it; the file will be deleted and replaced with"
                                           +" the newly created GostCrypt container. You will be able to encrypt existing files (later"
                                           +" on) by moving them to the GostCrypt container that you are about to create now.")
                    content.item.type = 0
                    content.item.setFileDialog(false)
                    break;
                case 1:
                    changePage(6, qsTr("Outer Volume"), currentPage)
                    break;
                case 2:
                case 3:
                    changePage(12, qsTr("Hidden Volume"), currentPage)
                    back_.setDisable(1)
                    content.item.type = typeBranch
                    break;
                }
            }
            break;




            /* Page8.qml
             * Volume size
             * in KB, MB or GB
             */
        case progress.VOLUME_SIZE://volume size (standard volume)
            typeBranch = content.item.type
            //type 0 & 1 (normal) => volumeInfos.VOLUME_SIZE, else volumeInfos.HIDDEN_VOLUME_SIZE
            if(content.item.type !== 2)
                volumeInfos.VOLUME_SIZE = content.item.sizeType
            else
                volumeInfos.HIDDEN_VOLUME_SIZE = content.item.sizeType
            if(direction === 1
                    && content.item.sizeType
                    && content.item.sizeType[0] > 0) //1 => normal
            {
                if(typeBranch !== 3 && typeBranch !== 2) {
                    changePage(9, qsTr("Volume Password"), currentPage)
                    content.item.setText(qsTr("     It is very important that you choose a good password. You should avoid"
                                              +" choosing one that contains only a single word that can be found in a dictionary (or a combination of 2, 3 or 4 such words)."
                                              +" It should not contain any names or dates of birth. It sould not be easy to guess."
                                              +" A good password is a random combination of upper and lower case letters, numbers, and special characters"
                                              +", such as @  = $ * + etc. We recommend choosing a password consisting of more than 20 characters (the longer, the better)."
                                              +" The maximum possible length is 64 characters."))
                }
                else {
                    changePage(9, qsTr("Hidden Volume Password"), currentPage)
                    content.item.setText(qsTr(" Please choose a password for the hidden volume. It is very important that you choose a good password. You should avoid"
                                              +" choosing one that contains only a single word that can be found in a dictionary (or a combination of 2, 3 or 4 such words)."
                                              +" It should not contain any names or dates of birth. It sould not be easy to guess."
                                              +" A good password is a random combination of upper and lower case letters, numbers, and special characters"
                                              +", such as @  = $ * + etc. We recommend choosing a password consisting of more than 20 characters (the longer, the better)."
                                              +" The maximum possible length is 64 characters."))
                }
                manageProgressBar(8,direction,typeBranch)
                content.item.type = typeBranch
            }else if(direction !== 1){
                if(typeBranch !== 3 && typeBranch !== 2) {
                    changePage(7, qsTr("Encryption Options"), currentPage)
                    qmlRequest("algorithms", "")
                }
                else {
                    changePage(7, qsTr("Hidden Volume Encryption Options"), currentPage)
                    qmlRequest("algorithms", "")
                }
                manageProgressBar(8,direction,typeBranch)
                content.item.type = typeBranch
            }
            break;




            /* Page9.qml
             * Volume password (twice)
             * TODO : use keyfiles
             */
        case progress.VOLUME_PWD: //volume password (standard volume)
            typeBranch = content.item.type

            //type 0 & 1 (normal) => volumeInfos.VOLUME_NEW_PASSWORD, else volumeInfos.HIDDEN_VOLUME_PASSWORD
            if(content.item.type !== 2) {
                volumeInfos.VOLUME_NEW_PASSWORD[0] = content.item.password[0]
                volumeInfos.VOLUME_NEW_PASSWORD[1] = content.item.password[1]
            }else {
                volumeInfos.HIDDEN_VOLUME_PASSWORD[0] = content.item.password[0]
                volumeInfos.HIDDEN_VOLUME_PASSWORD[1] = content.item.password[1]
            }
            //TODO : short password
            if(direction === 1
                    && content.item.password[0] !== ""
                    && content.item.password[0] === content.item.password[1]) //1 => normal
            {
                if(typeBranch !== 3 && typeBranch !== 2)
                    changePage(10, qsTr("Volume Format"), currentPage)
                else
                    changePage(10, qsTr("Hidden Volume Format"), currentPage)
                manageProgressBar(9,direction,typeBranch)
                content.item.type = typeBranch
                //if(typeBranch === 0 || typeBranch === 2 || typeBranch === 3)
                next_.text = qsTr("Format")
            }else if(direction !== 1){
                if(typeBranch !== 3 && typeBranch !== 2) {
                    changePage(8, qsTr("Volume Size"), currentPage)
                    content.item.setText(qsTr("<b>Free space on drive " + ""/*Wizard.getfreeSpace()*/+"</b>"),
                                         qsTr("Please specify the size of the container you want to create.<br><br>If" //TODO GET FREE SPACE
                                              +" you create a dynamic (sparse-file) container, this parameter will specify its maximum possible size."
                                              +"<br><br>Note that possible size of an NTFS volume is 3792 KB."))
                }
                else {
                    changePage(8, qsTr("Hidden Volume Size"), currentPage)
                    content.item.setText(qsTr("<b>Maximum possible hidden volume size for this volume is " + Wizard.getfreeSpace()+"</b>"),
                                         qsTr("Please specify the size of the hidden volume to create. The minimum possible "
                                              +"size of a hidden volume is 40KB (or 3664KB if it is fortmatted as NTFS). "
                                              +"The maximum possible size you can specify for the hidden volume is displayed above."))
                }

                manageProgressBar(9,direction,typeBranch)
                content.item.type = typeBranch
            }else{
                openErrorMessage(qsTr("Different passwords"), qsTr("The passwords are different or empties. <br>Please try again."))
            }
            break;




            /* Page10.qml
             * Volume format : random, filesystem, cluster, dynamic
             */
        case progress.VOLUME_FORMAT:
             //format volume (standard volume)
            if(content.item.type !== 2) {
                volumeInfos.FORMAT_INFOS[0] = content.item.format[0]
                volumeInfos.FORMAT_INFOS[1] = content.item.format[1]
                volumeInfos.FORMAT_INFOS[2] = content.item.format[2]
            }else {
                volumeInfos.HIDDEN_FORMAT_INFOS[0] = content.item.format[0]
                volumeInfos.HIDDEN_FORMAT_INFOS[1] = content.item.format[1]
                volumeInfos.HIDDEN_FORMAT_INFOS[2] = content.item.format[2]
            }
            typeBranch = content.item.type
            manageProgressBar(10,direction,typeBranch)
            //TODO : create volume here
            if(direction === 1) //1 => normal
            {
                switch(content.item.type) {
                case 0:
                    createVolume(0);
                    content.source = "PageEnd.qml"
                    back_.visible = false
                    next_.visible = false
                    help_.visible = false
                    break;
                case 1:
                    changePage(11, qsTr("Outer Volume Contents"), currentPage)
                    next_.text = qsTr("Next >")
                    back_.setDisable(1)
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
                    changeSubWindowTitle(qsTr("GostCrypt Volume Creation Wizard"))
                    back_.visible = false
                    next_.visible = false
                    help_.visible = false
                    break;
                }
            }else if(direction !== 1){
                if(typeBranch !== 3 && typeBranch !== 2)
                    changePage(9, qsTr("Volume password"), currentPage)
                else
                    changePage(9, qsTr("Hidden Volume password"), currentPage)
                next_.text = qsTr("Next >")
                content.item.type = typeBranch
            }
            break;




            /* Page11.qml
             * Outer Volume Contents
             */
        case progress.VOLUME_OUTER_CONTENTS:
            typeBranch = content.item.type
            manageProgressBar(11,direction,0)
            if(direction === 1) //1 => normal
            {
                changePage(12, qsTr("Hidden Volume"), currentPage)
                back_.setDisable(1)
                content.item.type = typeBranch
            }
            break;




            /* Page12.qml
             * Hidden Volume part (information)
             */
        case progress.VOLUME_HIDDEN_VOLUME:
            typeBranch = content.item.type
            manageProgressBar(12,direction,typeBranch)
            if(direction === 1) //1 => normal
            {
                changePage(7, qsTr("Hidden Volume Encryption Options"), currentPage)
                back_.setDisable(0)
                content.item.type = typeBranch
            }
            break;



            /* PageEnd.qml
             * Done !
             */
        case progress.VOLUME_END:

            break;
        }

    }

    /*!
      * Fn : manages the page loading
      */
    function changePage(number, title, current) {
        content.source = "Page"+number+".qml"
        changeSubWindowTitle(title)
        currentPage+=(number-current)
    }

    function createVolume(type)
    {
        switch(type)
        {
        case 0: //normal without hidden
            qmlRequest("createvolume", {
                           "type": 1,
                           "path": volumeInfos.VOLUME_PATH,
                           "size": volumeInfos.VOLUME_SIZE,
                           "encryptionAlgorithm": volumeInfos.ALGORITHM_HASH_NAMES[0],
                           "volumeHeaderKdf": "", //TODO
                           "filesystem": volumeInfos.FORMAT_INFOS[0],
                           "keyfiles": "", //TODO
                           "password": volumeInfos.VOLUME_NEW_PASSWORD[0],
                       });
            break;
        }
    }

    /*!
      * Fn : manages the loading bar
      */
    function manageProgressBar(posInitial, direction, branch)
    {

    }

}
