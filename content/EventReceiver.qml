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
    signal setQtCopyrightYear(copyrightYear: string)
    onSetQtCopyrightYear: (copyrightYear) => {
        Global.qtCopyrightYear = copyrightYear;
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
    signal createPluginWindow()
    onCreatePluginWindow: {
        let component = Qt.createComponent(
            "qrc:content/PluginWindow.qml",
            mainWindow
        );
        if(component.status == Component.Ready) {
            var window = component.createObject(mainWindow);
            mainWindow.pluginWindow = window;
            EventSender.pluginWindowReady();
        }
        else {
            console.log("onCreatePluginWindow() failed with error:",
                component.errorString()
            );
        }
    }
    signal createGenericPluginEditor()
    onCreateGenericPluginEditor: {
        let component = Qt.createComponent(
            "qrc:content/GenericPluginEditor.qml",
            mainWindow
        );
        if(component.status == Component.Ready) {
            var window = component.createObject(mainWindow);
            mainWindow.genericPluginEditor = window;
            EventSender.genericPluginEditorReady();
        }
        else {
            console.log("onCreateGenericPluginEditor() failed with error:",
                component.errorString()
            );
        }
    }
}
