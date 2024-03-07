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
    // This is a temporary solution to locate and resize the plugin GUI in the
    // window, instead of letting the plugin GUI use the entire window area.
    // Currently this solution has some drawbacks:
    // - Plugin frame and plugin window will not gain focus at the same time,
    //   which is different from behavior of most DAWs.
    // - Resize operation of the plugin frame and the plugin window will not
    //   be performed at the same time, resulting in visible lags.
    // - On X11, the lag is more obvious. Even worse, resizing too fast might
    //   fail, making sizes of plugin frame and plugin window out of sync.
    // There are native solutions available.
    // (At least we can make it on Windows, since every native control is a
    // window and has an HWND to be passed to the plugin GUI instance. See
    // https://learn.microsoft.com/windows/win32/learnwin32/what-is-a-window- )
    Window {
        id: pluginFrame
        flags: Qt.Dialog | Qt.FramelessWindowHint
        visibility: root.visibility
        x: root.x
        y: root.y + 20
        onClosing: (close) => {
            if (!root.destroyingPlugin) {
                close.accepted = false;
                // A temporary fix for Windows so that the main window will gain
                // focus while some other windows are shown.
                // Otherwise, one of the other windows will gain focus, which
                // might lay above the main window. This is definitely not the
                // desired behavior.
                root.requestActivate();
                root.close();
            }
        }
    }
}