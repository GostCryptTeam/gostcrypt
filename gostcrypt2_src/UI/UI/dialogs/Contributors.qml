import QtQuick 2.7
import QtQuick.Controls 2.0
import "../" as UI

Item {
    id: top;
    signal close()

    Connections {
        target: top
        onClose: {
            catchClose();
        }
    }



    Flickable {
        id: flickable
        y: 15
        anchors.horizontalCenter: parent.horizontalCenter
        height: 280
        width: top.width-98
        clip: true
        contentWidth: content.width
        contentHeight: content.height
        flickableDirection: Flickable.VerticalFlick
        interactive: true
        boundsBehavior: Flickable.StopAtBounds

        ScrollBar.vertical: ScrollBar {
                parent: flickable.parent
                anchors.top: flickable.top
                anchors.left: flickable.right
                anchors.bottom: flickable.bottom
                active: true
            }

        Text {
            id: content
            width: parent.width
            wrapMode: Text.WordWrap
            color: palette.text
            font.pixelSize: 15
            font.family: "Helvetica"
            text: qsTr("<b>Portions of this software are based in part on the works of the following people:</b><br><br>"
            +"Paul Le Roux, Bruce Schneier, John Kelsey, Doug Whiting, David Wagner, Chris Hall, Niels Ferguson, "
            +"Lars Knudsen, Ross Anderson, Eli Biham, Joan Daemen, Vincent Rijmen, Phillip Rogaway, Hans Dobbertin, "
            +"Antoon Bosselaers, Bart Preneel, Paulo Barreto, Brian Gladman, Wei Dai, Peter Gutmann, and many others.<br><br><br>"
            +"<b>The following people are currently involved with the development of the GostCrypt project:</b><br><br>")
            +"<font color=\""+palette.green + "\">AMICELLI Paul, "
            +"BALK Evgeny, "
            +"BANNIER Arnaud, "
            +"BECLAIR Louis, "
            +"BERTIN Tristan, "
            +"BODIN Nicolas, "
            +"DAVID Baptiste, "
            +"EIJKHOUDT Arinm, "
            +"FILIOL Eric, "
            +"FLAVIGNY Octavie, "
            +"GALLIENNE Kevin, "
            +"GROOT Sebastiaan, "
            +"HEBERT Antoine, "
            +"LARDIER William, "
            +"LE FOURNIS Vincent, "
            +"SWAENEPOEL Guillaume, "
            +"THIEULEUX Jonathan, "
            +"VARO Quentin.</font><br><br><br>"
            + qsTr("Portions of this software:<br>\
            Copyright © 2003-2012 TrueCrypt Developers Association. All Rights Reserved.<br>\
            Copyright © 1998-2000 Paul Le Roux. All Rights Reserved.<br>\
            Copyright © 1998-2008 Brian Gladman. All Rights Reserved.<br>\
            Copyright © 2002-2004 Mark Adler. All Rights Reserved.<br>\
            <br>\
            This software as a whole:<br>\
            Copyright © 2012 TrueCrypt Developers Association. All rights reserved.<br>\
            <br>\
            A TrueCrypt Foundation Release")
        }
    }

    Row {
        spacing: 15
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        UI.ButtonBordered {
            id: benchmark
            height: 40
            text: qsTr("OK") + Translation.tr
            width: 120
            onClicked: top.close()
            color_: palette.blue
            anchors.bottomMargin: 10
        }
    }

}
