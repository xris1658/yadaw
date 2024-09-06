pragma Singleton
import QtQuick

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
    signal setQtVersion(majorVersion: int, minorVersion: int, microVersion: int)
    onSetQtVersion: (majorVersion, minorVersion, microVersion) => {
        Global.qtMajorVersion = majorVersion;
        Global.qtMinorVersion = minorVersion;
        Global.qtMicroVersion = microVersion;
    }
    signal setQtCopyrightYear(copyrightYear: string)
    onSetQtCopyrightYear: (copyrightYear) => {
        Global.qtCopyrightYear = copyrightYear;
    }
    signal setFileBrowserName(name: string)
    onSetFileBrowserName: (name) => {
        Global.fileBrowserName = name;
    }
    signal setSplashScreenText(text: string)
    onSetSplashScreenText: (text) => {
        splashScreen.text = text;
    }
    signal pluginScanComplete()
    onPluginScanComplete: {
        mainWindow.pluginScanComplete();
    }
    signal setScanningDirectories(value: bool)
    onSetScanningDirectories: (value) => {
        mainWindow.pluginScanProgressWindow.scanningDirectories = value;
    }
    signal setPluginScanTotalCount(count: int)
    onSetPluginScanTotalCount: (count) => {
        mainWindow.pluginScanProgressWindow.totalFileCount = count;
    }
    signal setPluginScanProgress(count: int, currentName: string);
    onSetPluginScanProgress: (count, currentName) => {
        mainWindow.pluginScanProgressWindow.currentFileIndex = count;
        mainWindow.pluginScanProgressWindow.currentFileName = currentName;
    }
}
