pragma Singleton
import QtQuick
import QtQuick.Controls as QC

QtObject {
    id: root
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
    signal pluginScanComplete()
    onPluginScanComplete: {
        mainWindow.pluginScanComplete();
    }
    signal messageDialog(message: string, title: string, icon: int, modal: bool)
    onMessageDialog: (message, title, icon, modal) => {
        let component = Qt.createComponent("./MessageDialog.qml");
        if(component.status === Component.Ready) {
            let dialog = component.createObject(root);
            dialog.message = message;
            dialog.title = title;
            dialog.standardButtons = QC.DialogButtonBox.Ok;
            dialog.icon = icon;
            if(modal) {
                dialog.modality = Qt.NonModal;
            }
            dialog.showNormal();
        }
        else {
            console.log(component.errorString());
        }
    }
}