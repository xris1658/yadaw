pragma Singleton
import QtQuick

QtObject {
    property Window darkModeSupportWindow: null
    signal startInitializingApplication()
    signal mainWindowClosing()
    signal addWindowForDarkModeSupport()
    signal removeWindowForDarkModeSupport()
    signal locatePathInExplorer(path: string)
}
