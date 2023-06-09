pragma Singleton
import QtQuick

QtObject {
    property Window darkModeSupportWindow: null
    signal startInitializingApplication()
    signal mainWindowClosing()
    signal locatePathInExplorer(path: string)
    signal audioGraphOutputDeviceIndexChanged(deviceIndex: int)
    signal startPluginScan()
    signal setSystemFontRendering(enabled: bool)
    signal setSystemFontRenderingWhileDebugging(enabled: bool)
    signal setTranslationIndex(index: int)
}
