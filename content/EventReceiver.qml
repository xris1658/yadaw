pragma Singleton
import QtQuick

QtObject {
    property SplashScreen splashScreen: null
    property MainWindow mainWindow: null
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
}