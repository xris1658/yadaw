pragma Singleton
import QtQuick

QtObject {
    property MainWindow mainWindow: null
    signal mainWindowClosingAccepted()
    onMainWindowClosingAccepted: {
        mainWindow.closingAccepted();
    }
    signal setQtVersion(version: string)
    onSetQtVersion: (version) => {
        Global.qtVersion = version;
    }
}