import QtQuick
import QtQuick.Window

Window {
    id: root
    flags: Qt.Dialog
    modality: Qt.NonModal
    property bool destroyingPlugin: false
    onClosing: (close) => {
        if(!root.destroyingPlugin) {
            close.accepted = false;
            root.hide();
        }
    }
}