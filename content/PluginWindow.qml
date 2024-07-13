import QtQuick
import QtQuick.Window

Window {
    id: root
    flags: Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint
    modality: Qt.NonModal
    color: Colors.background
    property bool destroyingPlugin: false
    property Window pluginFrame: pluginFrame
    width: pluginFrame.width
    height: pluginFrame.height + topBar.height
    onClosing: (close) => {
        if(!root.destroyingPlugin) {
            close.accepted = false;
            root.hide();
        }
    }
    onWidthChanged: {
        pluginFrame.width = width;
    }
    onHeightChanged: {
        pluginFrame.height = height - topBar.height;
    }
    Rectangle {
        id: topBar
        width: parent.width
        height: 20
    }
    // Embedded into `root`. Since embedding windows is not available in QML
    // codes until Qt 6.7, we have to do this in C++ codes. (although coupling
    // is often a bad idea...)
    // See `YADAW::Controller::createPluginWindow()`.
    Window {
        id: pluginFrame
        flags: Qt.FramelessWindowHint
        visibility: root.visibility
        transientParent: root
        x: 0
        y: topBar.height
        onClosing: (close) => {
            if (!root.destroyingPlugin) {
                close.accepted = false;
            }
        }
    }
}