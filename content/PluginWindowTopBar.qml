import QtQuick

Item {
    id: root
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