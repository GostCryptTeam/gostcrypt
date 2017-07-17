import QtQuick 2.0
import "LoadVolume.js" as LoadVolume

Item {
    /*!
        \title Connections
        \brief QML slots that receive C++ signals
     */
    Connections {
        /*!
          Connects the C++ object (connectSignals.h) to the window
          */
        target: ConnectSignals;
        onSendSubWindowVolumeInfos: {
            subWindow.catchClose();
            pageLoader.item.loadVolume(aMount, aAlgo, aPath, aSize);
        }
        onSendSubWindowAskSudoPassword: {
            /*subWindow.opacity = subWindow.opacity = 1.0
            subWindow.visible = subWindow.visible = true
            if(subWindow.w !== "../dialogs/GSConnectSudo.qml")
            {
                subWindow.w = "../dialogs/GSConnectSudo.qml"
                subWindow.title = 'Enter your password'
                subWindow.loadForm()
                subWindow.changeSubWindowHeight(200);
            }*/
            console.log("Demande de sudo");
            app.toggleSudo(1)
        }
        onSendSubWindowConfirmSudoPassword:  {
            subWindow.catchClose();
        }
        onSendSubWindowErrorMessage: {
            openErrorMessage(aTitle, aContent);
        }
    }
}
