import QtQuick 2.7
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4
import "../" as UI

Item {
    id: top
    property alias page : content.item
    property int currentPage: 1
    property var typeBranch
    property var volumeInfos: {
        "CONTAINER_TYPE": 0,
        "CONTAINER_SIZE": "",                       //file size and type (KB, MB, GB) (standard volumes)
        "VOLUME_TYPE": 0,
        "NORMAL_OR_HIDDEN": 0,                      //normal or direct mode (hidden)
        "VOLUME_PATH": "",                          //volume path (standard/hidden volume)
        "VOLUME_PWD": "",                           //volume password (hidden+direct volume)
        "VOLUME_KEYFILES": [],                      //volume keyfile(s) (hidden+direct volume)
        "ALGORITHM_HASH_NAMES": ["", ""],           //name of the algorithm (standard volume) and name of the hash algorithm
        "VOLUME_NEW_PASSWORD": ["", ""],            //volume password with verification (standard volume)
        "VOLUME_NEW_KEYFILES": [],                  //volume keyfile(s)
        "FORMAT_INFOS": ["", "", false],            //file system, cluster & dynamic(bool) (standard volumes)
        "HIDDEN_ALGORITHM_HASH_NAMES": ["", ""],    //HIDDEN VOLUME : algorithm & hash
        "HIDDEN_VOLUME_SIZE": 0.0,                  //HIDDEN VOLUME: volume size
        "HIDDEN_VOLUME_PASSWORD": ["",""],          //HIDDEN VOLUME: volume password (with verification)
        "HIDDEN_VOLUME_KEYFILES": [],               //HIDDEN VOLUME: volume keyfile(s)
        "HIDDEN_FORMAT_INFOS":  ["", "", false]     //HIDDEN VOLUME: file system, cluster & dynamic(bool) (standard volumes)*/
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
        //"VOLUME_FORMAT": 10,
        "VOLUME_OUTER_CONTENTS": 11,
        "VOLUME_HIDDEN_VOLUME": 12,
        "VOLUME_END": 666
    }

    property var texts: {
        "PAGE_4": qsTr("A GostCrypt volume can reside in a file (called GostCrypt container),"
                       +" which can reside on a hard disk, on a USB flash drive, etc. A GostCrypt"
                       +" container is just like any normal file (it can be, for example, moved or deleted as"
                       +" any normal file). Click 'Select File' to choose a filename for the container and"
                       +" to select the location where you wish the container to be created.<br><br><b>WARNING</b>: If you select"
                       +" an existing file, GostCrypt will NOT encrypt it; the file will be deleted and replaced with"
                       +" the newly created GostCrypt container. You will be able to encrypt existing files (later"
                       +" on) by moving them to the GostCrypt container that you are about to create now."),

        "PAGE_4_OUTER": qsTr("Select the location of the outer volume to be created (within its volume the hidden volume will be created later on)"
                       +"<br>A GostCrypt volume can reside in a file (called GostCrypt container),"
                       +" which can reside on a hard disk, on a USB flash drive, etc. A GostCrypt"
                       +" container is just like any normal file (it can be, for example, moved or deleted as"
                       +" any normal file). Click 'Select File' to choose a filename for the container and"
                       +" to select the location where you wish the container to be created.<br><br><b>WARNING</b>: If you select"
                       +" an existing file, GostCrypt will NOT encrypt it; the file will be deleted and replaced with"
                       +" the newly created GostCrypt container. You will be able to encrypt existing files (later"
                       +" on) by moving them to the GostCrypt container that you are about to create now."),

        "PAGE_4_HIDDEN": qsTr("Select the location of the GostCrypt volume within which you wish to create a hidden volume."),

        "PAGE_8_NORMAL_1": qsTr("<b>Free space on drive : "),

        "PAGE_8_NORMAL_2": qsTr("Please specify the size of the container you want to create.<br>If"
                                +" you create a dynamic (sparse-file) container, this parameter will specify its maximum possible size."
                                +"<br>Note that possible size of an NTFS volume is 3792 KB."),

        "PAGE_8_HIDDEN_1": qsTr("<b>Maximum possible hidden volume size for this volume is "),

        "PAGE_8_HIDDEN_2": qsTr("Please specify the size of the hidden volume to create. The minimum possible "
                                +"size of a hidden volume is 40KB (or 3664KB if it is fortmatted as NTFS). "
                                +"The maximum possible size you can specify for the hidden volume is displayed above."),

        "PAGE_9_NORMAL": qsTr("     It is very important that you choose a good password. You should avoid"
                              +" choosing one that contains only a single word that can be found in a dictionary (or a combination of 2, 3 or 4 such words)."
                              +" It should not contain any names or dates of birth. It sould not be easy to guess."
                              +" A good password is a random combination of upper and lower case letters, numbers, and special characters"
                              +", such as @  = $ * + etc. We recommend choosing a password consisting of more than 20 characters (the longer, the better)."
                              +" The maximum possible length is 64 characters."),

        "PAGE_9_HIDDEN": qsTr(" Please choose a password for the hidden volume. It is very important that you choose a good password. You should avoid"
                              +" choosing one that contains only a single word that can be found in a dictionary (or a combination of 2, 3 or 4 such words)."
                              +" It should not contain any names or dates of birth. It sould not be easy to guess."
                              +" A good password is a random combination of upper and lower case letters, numbers, and special characters"
                              +", such as @  = $ * + etc. We recommend choosing a password consisting of more than 20 characters (the longer, the better)."
                              +" The maximum possible length is 64 characters."),

        "HIDDEN_CREATED": qsTr("The hidden GostCrypt volume has been successfully created and is ready for use. If all "
                               + "the instructions have been followed and if the precautions and requirements listed in the section "
                               + "\"Security Requirements and Precautions Pertaining to Hidden Volumes\" in the GostCrypt User's Guide "
                               + "are followed, it should be impossible to prove that the hidden volume exists, event when the outer "
                               + "volume is mounted.<br><br>WARNING: IF YOU DO NOT PROTECT THE HIDDEN VOLUME (FOR INFORMATION ON HOW "
                               + "TO DO SO, REFER TO THE SECTION \"PROTECTION OF HIDDEN VOLUMES AGAINST DAMAGE\" IN THE GOSTCRYPT USER'S "
                               + "GUIDE), DO NOT WRITE TO THE OUTER VOLUME. OTHERWISE, YOU MAY OVERWRITE AND DAMAGE THE HIDDEN VOLUME!"),
    }

    //All choices possibilities
    //Format : "page id / loading bar value"
    property var pathBase: [{0: 1, 1: 1},
                            {0: 2, 1: 1},
                            {0: 3, 1: 1},
                            {0: 4, 1: 2}]

    property var pathFileNormal: [{0: 7, 1: 3},
                                  {0: 8, 1: 4},
                                  {0: 9, 1: 4}]

    property var pathFileHiddenNormal: [
        {0: 6, 1: 3},
        {0: 7, 1: 3},
                                        {0: 8, 1: 3},
                                        {0: 9, 1: 3},
                                        {0: 11, 1: 3},
        {0: 5, 1: 4},
                                                {0: 12, 1: 4},
                                                {0: 7, 1: 4},
                                                {0: 8, 1: 4},
                                                {0: 9, 1: 4},
                                                {0: 11, 1: 4}]

    property var pathFileHiddenDirect: [{0: 5, 1: 2},
                                        {0: 12, 1: 3},
                                        {0: 7, 1: 3},
                                        {0: 8, 1: 3},
                                        {0: 9, 1: 4},
                                        {0: 11, 1: 4}]

    property var path: {"type": pathBase, "position": 0}

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

        onSourceChanged: animation.running = true

                NumberAnimation {
                    id: animation
                    target: loader.item
                    property: "opacity"
                    from: 0
                    to: 100
                    duration: app.duration
                    easing.type: Easing.InExpo
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

    Row {
        spacing: 22
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: steps.top
        anchors.topMargin: -5
        anchors.right: top.right;
        anchors.rightMargin: 5

        UI.ButtonBordered {
            id: help_
            height: 25
            width: 80
            color_: palette.blue
            text: qsTr("Help")
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
        switch(currentPage)
        {
            /* Page1.qml
             * * Create an encrypted file container
             * * Encrypt a non-system partition/volume
             */
        case progress.VOLUME_TYPE:
            volumeInfos.CONTAINER_TYPE = content.item.type
            if(volumeInfos.CONTAINER_TYPE === 0) //FILE
            {
                path.position += 1;
                changePage(qsTr("Volume Type"), currentPage)
                manageButtons(true, false);
            }
            else //DEVICE
            {
                path.position += 1;
            }

            break;

            /* Page2.qml
             * * Standard GostCrypt Volume
             * * Hidden GostCrypt Volume
             */
        case progress.VOLUME_ISHIDDEN:
            volumeInfos.VOLUME_TYPE = content.item.type
            if(direction === 1) //1 => normal
            {
                // Choice: Standard GostCrypt Volume
                if(volumeInfos.VOLUME_TYPE === 0)
                {
                    path.position += 2;
                    changePage(qsTr("Volume Location"), currentPage)
                    path.type = pathFileNormal; path.position = 0;
                    manageButtons(true, true);
                    content.item.message = texts.PAGE_4
                    content.item.type = 0
                }else {
                    //Choice : Hidden GostCrypt Volume
                    path.position += 1;
                    changePage(qsTr("Volume Creation Mode"), currentPage)
                    manageButtons(true, true);
                }
            }else{
                path.position -= 1;
                changePage(qsTr("GostCrypt Volume Creation Wizard"), currentPage)
                manageButtons(false, false);
            }
            break;



            /* Page3.qml (hidden volume)
             * * Normal mode
             * * Direct mode
             */
        case progress.DIRECT_NORMAL:
            volumeInfos.NORMAL_OR_HIDDEN = content.item.type
            if(direction === 1) //1 => normal
            {
                path.position += 1
                changePage(qsTr("Volume Location"), currentPage)
                //normal
                if(volumeInfos.NORMAL_OR_HIDDEN === 0)
                {
                    path.type = pathFileHiddenNormal; path.position = 0;
                    content.item.message = texts.PAGE_4_OUTER
                    content.item.type = 2
                    content.item.setFileDialog(false)
                }else{ //direct mode (Quick)
                    path.type = pathFileHiddenDirect; path.position = 0;
                    content.item.message = texts.PAGE_4_HIDDEN
                    content.item.type = 1
                    content.item.setFileDialog(true)
                }
            }else{
                path.position -= 1;
                changePage(qsTr("Volume Type"), currentPage)
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
            if(direction === 1 && (volumeInfos.VOLUME_PATH !== "")) //1 => normal
            {
                switch (content.item.type) {
                case 0:
                    changePage(qsTr("Encryption Options"), currentPage)
                    qmlRequest("algorithms", "")
                    break;
                case 1:
                    changePage(qsTr("Volume Password"), currentPage)
                    break;
                case 2:
                    changePage(qsTr("Outer Volume"), currentPage)
                    break;
                }
            }else if(direction !== 1){
                path.type = pathBase
                switch (content.item.type) {
                case 0:
                    path.position = 1
                    changePage(qsTr("Volume Type"), currentPage)
                    manageButtons(true, false);
                    break;
                case 1:
                case 2:
                    path.position = 2
                    changePage(qsTr("Volume Creation Mode"), currentPage)
                    break;
                }
            }
            break;


            /* Page5.qml
             * Existing volume password (hidden)
             */
        case progress.VOLUME_DIRECT_PWD: //volume password (hidden/direct)
            volumeInfos.VOLUME_PWD = content.item.password
            volumeInfos.VOLUME_KEYFILES = content.item.listKeyfiles
            if(direction === 1 && (volumeInfos.VOLUME_PWD !== "" || volumeInfos.VOLUME_KEYFILES.length > 0)) //1 => normal
            {
                path.position += 1
                changePage(qsTr("Hidden Volume"), currentPage)
                content.item.type = 3
            }else if(direction !== 1){
                path.type = pathBase; path.position = 3;
                changePage(qsTr("Volume Location"), currentPage)
                content.item.message = texts.PAGE_4_HIDDEN
                content.item.type = 1
                content.item.setFileDialog(true)
            }
            break;


            /* Page6.qml
             * Outer Volume information (hidden/normal)
             */
        case progress.VOLUME_OUTER_VOLUME: //outer volume message (hidden/normal)
            if(direction === 1)
            {
                path.position += 1
                changePage(qsTr("Encryption Options"), currentPage)
                qmlRequest("algorithms", "")
                content.item.type = 1
            }else{
                path.type = pathBase; path.position = 3;
                changePage(qsTr("Volume Location"), currentPage)
                path.type = pathFileHiddenNormal; path.position = 0;
                content.item.message = texts.PAGE_4_OUTER
                content.item.type = 2
                content.item.setFileDialog(false)
            }
            break;



            /* Page7.qml
             * Algorithm and hash used to format the current volume
             */
        case progress.VOLUME_ALGO: //algorithm & hash (standard volume)
            typeBranch = content.item.type
            //type 0 & 1 (normal) => volumeInfos.ALGORITHM_HASH_NAMES, else volumeInfos.HIDDEN_ALGORITHM_HASH_NAMES
            if(content.item.type !== 2 && content.item.type !== 3)
                volumeInfos.ALGORITHM_HASH_NAMES = content.item.used
            else
                volumeInfos.HIDDEN_ALGORITHM_HASH_NAMES = content.item.used
            if(direction === 1)
            {
                path.position += 1;
                if(typeBranch === 0) {
                    changePage(qsTr("Volume Size"), currentPage)
                    content.item.setText(texts.PAGE_8_NORMAL_1 + "50Go" + "<br>", texts.PAGE_8_NORMAL_2)
                }
                else {
                    changePage(qsTr("Hidden Volume Size"), currentPage)
                    content.item.setText(texts.PAGE_8_HIDDEN_1 + "50Go"+"</b>", texts.PAGE_8_HIDDEN_2)
                }
                content.item.type = typeBranch
            }else{
                switch(content.item.type) {
                case 0:
                    path.type = pathBase; path.position = 3;
                    changePage(qsTr("Volume Location"), currentPage)
                    path.type = pathFileNormal; path.position = 0;
                    content.item.message = texts.PAGE_4
                    content.item.type = 0
                    content.item.setFileDialog(false)
                    break;
                case 1:
                    path.position -= 1;
                    changePage(qsTr("Outer Volume"), currentPage)
                    break;
                case 2:
                case 3:
                    path.position -= 1;
                    changePage(qsTr("Hidden Volume"), currentPage)
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
            if(content.item.type !== 2 && content.item.type !== 3)
            {
                volumeInfos.CONTAINER_SIZE = content.item.sizeType[0] + content.item.sizeType[1]
            }
            else
                volumeInfos.HIDDEN_VOLUME_SIZE = content.item.relativeSize
            if(direction === 1 && (content.item.sizeType && content.item.sizeType[0] > 0 || (typeBranch === 3 || typeBranch === 2))) //1 => normal
            {
                path.position += 1;
                if(typeBranch !== 3 && typeBranch !== 2) {
                    changePage(qsTr("Volume Password"), currentPage)
                    content.item.text_ = texts.PAGE_9_NORMAL
                }
                else {
                    changePage(qsTr("Hidden Volume Password"), currentPage)
                    content.item.text_ = texts.PAGE_9_HIDDEN
                }
                content.item.type = typeBranch
            }else if(direction !== 1){
                path.position -= 1;
                if(typeBranch !== 3 && typeBranch !== 2) {
                    changePage(qsTr("Encryption Options"), currentPage)
                    content.item.type = typeBranch
                    qmlRequest("algorithms", "")
                }
                else {
                    changePage(qsTr("Hidden Volume Encryption Options"), currentPage)
                    content.item.type = typeBranch
                    qmlRequest("algorithms", "")
                }
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
            if(content.item.type !== 2 && content.item.type !== 3) {
                volumeInfos.VOLUME_NEW_PASSWORD[0] = content.item.password[0]
                volumeInfos.VOLUME_NEW_PASSWORD[1] = content.item.password[1]
                volumeInfos.VOLUME_NEW_KEYFILES = content.item.listKeyfiles
            }else {
                volumeInfos.HIDDEN_VOLUME_PASSWORD[0] = content.item.password[0]
                volumeInfos.HIDDEN_VOLUME_PASSWORD[1] = content.item.password[1]
                volumeInfos.HIDDEN_VOLUME_KEYFILES = content.item.listKeyfiles
            }
            //TODO : short password
            if(direction === 1
                    && (content.item.password[0] !== ""
                    && content.item.password[0] === content.item.password[1])
                    ||
                    content.item.listKeyfiles.length > 0
                    ) //1 => normal
            {
                if(typeBranch !== 1)
                {
                    createVolume();
                    content.source = "PageEnd.qml"
                    if(typeBranch === 1)
                        manageButtons(false, true)
                    else
                        manageButtons(false, false)
                }
                else
                {
                    path.position += 1;
                    changePage(qsTr("Outer Volume Contents"), currentPage)
                    manageButtons(false, true)
                }
            }else if(direction !== 1){
                path.position -= 1;
                if(typeBranch !== 3 && typeBranch !== 2) {
                    changePage(qsTr("Volume Size"), currentPage)
                    content.item.setText(texts.PAGE_8_NORMAL_1 + "50Go" + "</b>", texts.PAGE_8_NORMAL_2)
                }
                else {
                    changePage(qsTr("Hidden Volume Size"), currentPage)
                    content.item.setText(texts.PAGE_8_HIDDEN_1 + "50Go"+"</b>", texts.PAGE_8_HIDDEN_2)
                }
                content.item.type = typeBranch
            }else{
                openErrorMessage(qsTr("Different passwords or no Keyfiles"), qsTr("The passwords are different or empties, or you do not have a password or keyfile. <br>Please try again."))
            }
            break;



            /* Page11.qml
             * Outer Volume Contents
             */
        case progress.VOLUME_OUTER_CONTENTS:
            typeBranch = content.item.type
            if(direction === 1) //1 => normal
            {
                path.position += 1;
                changePage(qsTr("Hidden Volume"), currentPage)
            }
            break;



            /* Page12.qml
             * Hidden Volume part (information)
             */
        case progress.VOLUME_HIDDEN_VOLUME:
            typeBranch = content.item.type
            if(direction === 1) //1 => normal
            {
                path.position += 1;
                changePage(qsTr("Hidden Volume Encryption Options"), currentPage)
                qmlRequest("algorithms", "")
                content.item.type = typeBranch
                 manageButtons(true, true)
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
    function changePage(title, current) {
        content.source = "Page"+path.type[path.position][0]+".qml"
        changeSubWindowTitle(title)
        currentPage = path.type[path.position][0]
        manageProgressBar(path.type[path.position][1])
    }

    function createVolume()
    {
        var param, i;
        if(volumeInfos.CONTAINER_TYPE === 0) //FILE
        {
            if(volumeInfos.VOLUME_TYPE === 0) //FILE - Normal mode
            {
                param = {
                    "type": 1,
                    "path": volumeInfos.VOLUME_PATH,
                    "size": volumeInfos.CONTAINER_SIZE,
                    "algorithm": volumeInfos.ALGORITHM_HASH_NAMES[0],
                    "hash": volumeInfos.ALGORITHM_HASH_NAMES[1],
                    "filesystem": volumeInfos.FORMAT_INFOS[0],
                    "nb-keyfiles": volumeInfos.VOLUME_NEW_KEYFILES.length,
                    "keyfiles": volumeInfos.VOLUME_NEW_KEYFILES, //TODO
                    "password": volumeInfos.VOLUME_NEW_PASSWORD[0],
                    "name": qsTr("Create volume"),
                    "desc": qsTr("Creating the ") + volumeInfos.VOLUME_PATH + qsTr(" volume...")
                }
                for(i in volumeInfos.VOLUME_NEW_KEYFILES)
                    param["keyfile"+i] = volumeInfos.VOLUME_NEW_KEYFILES[i];
            }
            else if(volumeInfos.VOLUME_TYPE === 1) //FILE - Hidden mode
            {
                if(volumeInfos.NORMAL_OR_HIDDEN === 0)
                {
                    //FILE - Hidden mode - Create outer and inner

                }else{

                    //FILE - Hidden mode - Create inner only
                    param = {
                        "type": 2,
                        "path": volumeInfos.VOLUME_PATH,
                        "nb-hkeyfiles": volumeInfos.HIDDEN_VOLUME_KEYFILES.length,
                        "halgorithm": volumeInfos.HIDDEN_ALGORITHM_HASH_NAMES[0],
                        "inner-size": volumeInfos.HIDDEN_VOLUME_SIZE,
                        "hhash": volumeInfos.HIDDEN_ALGORITHM_HASH_NAMES[1],
                        "hpassword": volumeInfos.HIDDEN_VOLUME_PASSWORD[0],
                        "hkeyfiles": volumeInfos.HIDDEN_VOLUME_KEYFILES,
                        "hfilesystem": volumeInfos.HIDDEN_FORMAT_INFOS[0],
                        "name": qsTr("Create volume"),
                        "desc": qsTr("Creating the ") + volumeInfos.VOLUME_PATH + qsTr(" volume...")
                    }
                    for(i in volumeInfos.VOLUME_NEW_KEYFILES)
                        param["hkeyfile"+i] = volumeInfos.HIDDEN_VOLUME_KEYFILES[i];
                }
            }
        }else if(volumeInfos.CONTAINER_TYPE === 1) //DEVICE
        {
            //TODO : Devices
        }
        qmlRequest("createvolume", param);
    }

    /*!
      * Fn : manages the loading bar
      */
    function manageProgressBar(value)
    {
        step1.checked = false
        step2.checked = false
        step3.checked = false
        step4.checked = false
        step5.checked = false

        switch(value)
        {
        case 1:
            step1.checked = true
            break;
        case 2:
            step2.checked = true
            break;
        case 3:
            step3.checked = true
            break;
        case 4:
            step4.checked = true
            break;
        case 5:
            step5.checked = true
            break;
        }
    }

}
