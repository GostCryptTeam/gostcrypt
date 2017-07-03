import QtQuick 2.0

Item {
    property QtObject mainWindow_
    signal menuChanged(string name, int index)

    BackgroundFrame {
        name: "Help"
    }
}
