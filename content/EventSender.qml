pragma Singleton
import QtQuick

QtObject {
    property Window darkModeSupportWindow: null
    signal startInitializingApplication()
    signal mainWindowClosing()
    signal locatePathInExplorer(path: string)
    signal startPluginScan()
    signal setSystemFontRendering(enabled: bool)
    signal setTranslationIndex(index: int)
}
