pragma Singleton
import QtQuick

QtObject {
    signal startInitializingApplication()
    signal mainWindowClosing()
    signal locatePathInExplorer(path: string)
    signal currentAudioBackendChanged()
    signal audioGraphOutputDeviceIndexChanged(deviceIndex: int)
    signal startPluginScan()
    signal setSystemFontRendering(enabled: bool)
    signal setSystemFontRenderingWhileDebugging(enabled: bool)
    signal setTranslationIndex(index: int)
    signal toggleMainWindowFullscreen()
    signal showNativePopup()
    signal prepareBatchUpdate()
    signal commitBatchUpdate()
}
