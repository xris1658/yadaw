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
    Item {
        id: topBar
        width: parent.width
        height: editRouteButton.height + padding * 2
        property int padding: 3
        Button {
            id: bypassedButton
            anchors.left: parent.left
            anchors.leftMargin: parent.padding * 2
            anchors.verticalCenter: parent.verticalCenter
            width: height
            height: parent.height - parent.padding * 4
            background: Rectangle {
                anchors.centerIn: parent
                width: parent.width
                height: width
                radius: width / 2
                color: root.bypassed? Colors.background: "#40FF88"
                border.color: Colors.secondaryBackground
            }
        }
        Button {
            id: undoButton
            anchors.right: redoButton.left
            anchors.rightMargin: parent.padding
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("Undo")
        }
        Button {
            id: redoButton
            anchors.right: presetButton.left
            anchors.rightMargin: parent.padding
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("Redo")
        }
        ComboBoxButton {
            id: presetButton
            anchors.centerIn: parent
            width: 200
        }
        Button {
            id: savePresetButton
            anchors.left: presetButton.right
            anchors.leftMargin: parent.padding
            anchors.verticalCenter: parent.verticalCenter
            text: "Save Preset..."
        }
        Button {
            id: editRouteButton
            anchors.right: parent.right
            anchors.rightMargin: parent.padding
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("Edit Route...")
        }
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