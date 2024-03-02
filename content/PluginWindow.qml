import QtQuick
import QtQuick.Window

Window {
    id: root
    flags: Qt.Dialog
    modality: Qt.NonModal
    color: Colors.background
    property bool destroyingPlugin: false
    property Window pluginFrame: pluginFrame
    width: pluginFrame.width
    height: pluginFrame.height + 20
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
        pluginFrame.height = height - 20;
    }
    Window {
        id: pluginFrame
        flags: Qt.Dialog | Qt.FramelessWindowHint
        visibility: root.visibility
        x: root.x
        y: root.y + 20
    }
}