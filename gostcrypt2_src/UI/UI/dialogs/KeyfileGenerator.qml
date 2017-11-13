import QtQuick 2.7
import "../" as UI

Item {
    id: top;
    signal generateKeyfile()

    Connections {
        target: top
        onUpdate: {
            qmlRequest("kfgenerator", {"prf": pRFType.currentText})
            catchClose();
        }
    }

    Column {
        y: 5
        anchors.horizontalCenter: parent.horizontalCenter
        width: 500
        spacing: 10
      Text {
          id: text
          anchors.horizontalCenter: parent.horizontalCenter
          color: palette.roundFilled
          height: 50
          width: parent.width
          wrapMode: Text.WordWrap
          horizontalAlignment: Text.AlignHCenter
          text: qsTr("IMPORTANT: Move your mouse as randomly as"
                    +" possible within this window. The longer you move"
                    +" it, the better. This significantly increases the"
                    +" cryptographic strength of the keyfile.") + Translation.tr
          font.pointSize: 9
      }
      Row {
          anchors.topMargin: 10
          anchors.horizontalCenter: parent.horizontalCenter
          spacing: 15
          Text {
              id: text2
              color: palette.text
              text: qsTr("Mixing PRF:") + Translation.tr
              font.pointSize: 11
          }
          UI.GSCustomComboBox {
              id: pRFType
              model: ["GOST R 34.11-2012","GOST R 31.11-94", "Whirlpool"]
              width: 200
              height: 30
          }
          UI.GSCheckBox {
              id: displayPool
              text_: qsTr("Display pool content") + Translation.tr
              size_: 20
              sizeText: 10
              anchors.topMargin: 25
              textColor: palette.green
              checked: {
                  var isChecked = UserSettings.getSetting("KFG-POOL")
                  return (isChecked === "1") ? true : false;
              }
              onCheckedChanged: {
                  if(displayPool.checked == true)
                      UserSettings.setSetting("KFG-POOL", 1)
                  else
                      UserSettings.setSetting("KFG-POOL", 0)
              }
          }
      }
      Rectangle {
          id: block
          color: "transparent"
          border.width: 1
          border.color: palette.border
          radius: 5
          width: top.width-100
          height: 195
          anchors.horizontalCenter: parent.horizontalCenter
          property alias text1: pool.text

          Text {
              id: text3
              anchors.horizontalCenter: parent.horizontalCenter
              height: 25
              y: 5
              color: palette.green
              text: qsTr("Current Pool Content") + Translation.tr
              font.pointSize: 11
          }

          Text {
              id: pool
              anchors.top: text3.bottom
              anchors.topMargin: 3
              visible: displayPool.checked ? true : false
              color: palette.text
              font.letterSpacing: 5
              font.pixelSize: 8
              anchors.horizontalCenter: parent.horizontalCenter
              width: parent.width-20
          }
      }
    }


    Row {
        spacing: 15
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        UI.GSButtonBordered {
            id: saveButton
            height: 40
            text: qsTr("Generate and save keyfile...") + Translation.tr
            width: 230
            onClicked: updatePool() //top.update()
            color_: palette.green
            anchors.bottomMargin: 10
        }
    }

    function updatePool(pool) {
        var a = "97f5d55145e0d0c57bea8f70dffc4ce7be19c1e7b6e24e9fa0f6b09494fdd97c0a549cc020fb52c72546c3cefea266b9ea02a5ecebe7ae8c5e4e7ca58e603e1a65ac089fa55607b1adaafe89ae4d51fded0fe8f7d85250239018d780fedcf9f460ebc242d4ba16ddd9dacb594b8c3408a150de2aa74083970d61be22e4be82902cdd68a45b609c924248e7eacd0961e4a23496cd454fe939422bc1df7a685197afb67790c0070b199ac711ce542e698f88a647b25027fe256e6dabf10bd135968d85a5b7b7a2340fc567932babcee1a6daebf3e8abe0ea680aa9d767de6f678b7fcc7d72f333a07989963824d482d1636638e513f69a2dc526a3642f839df6a7d86364953f99124deed05a2e25fe6fb713bc7effbdbaf9e23acf11bb4d6cda950c9ad6e15c9a0f7e2bc6407128da8f85f46d31e9dccce83c08802dbb4513ec30"
        var text = chunk(chunk(a, 2).join(" "), 69).join("<br>");
        block.text1 = text;
    }

   function chunk(str, n) {
       var ret = [];
       var i;
       var len;

       for(i = 0, len = str.length; i < len; i += n) {
          ret.push(str.substr(i, n))
       }

       return ret
   }
}
