pragma Singleton
import QtQuick
import QtQuick.Controls as QC

QtObject {
    id: root
    property SplashScreen splashScreen: null
    property MainWindow mainWindow: null
    signal mainWindowReady()
    onMainWindowReady: {
        mainWindow.mainWindowReady();
    }
    signal mainWindowClosingAccepted()
    onMainWindowClosingAccepted: {
        mainWindow.closingAccepted();
    }
    signal setQtVersion(version: string)
    onSetQtVersion: (version) => {
        Global.qtVersion = version;
    }
    signal setSplashScreenText(text: string)
    onSetSplashScreenText: (text) => {
        splashScreen.text = text;
    }
    signal pluginScanComplete()
    onPluginScanComplete: {
        mainWindow.pluginScanComplete();
    }
    signal messageDialog(message: string, title: string, icon: int, modal: bool)
    onMessageDialog: (message, title, icon, modal) => {
        Global.messageDialog(message, title, icon, modal);
    }
}
