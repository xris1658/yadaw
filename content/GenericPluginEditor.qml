import QtQml
import QtQuick

Window {
    id: root

    property bool destroyingPlugin: false
    property alias pluginName: titleLabel.text
    property alias parameterListModel: parameterList.model

    flags: Qt.Dialog
    modality: Qt.NonModal
    width: 400
    height: 400
    color: Colors.background
    Item {
        id: header
        width: root.width
        height: titleLabel.contentHeight + 6
        Label {
            id: titleLabel
            font.pointSize: Qt.application.font.pointSize * 1.5
            anchors.fill: parent
            anchors.margins: 3
            text: "Plugin Name Here"
        }
        Rectangle {
            width: parent.width
            height: 1
            anchors.bottom: parent.bottom
            color: Colors.border
        }
    }
    Item {
        width: parent.width
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        ListView {
            id: parameterList
            anchors.fill: parent
            anchors.rightMargin: vbar.visible? vbar.width: 0
            anchors.bottomMargin: hbar.visible? hbar.height: 0
            orientation: Qt.Vertical
            clip: true
            ScrollBar.vertical: vbar
            ScrollBar.horizontal: hbar
            flickableDirection: Flickable.AutoFlickDirection
            boundsBehavior: Flickable.StopAtBounds
            delegate: Item {
                width: parent.width
                Label {
                    anchors.fill: parent
                    text: pplm_name
                }
            }
            reuseItems: true
        }
        ScrollBar {
            id: vbar
            anchors.top: parent.top
            anchors.right: parent.right
            visible: parameterList.height < parameterList.contentHeight
            height: parameterList.height
            orientation: Qt.Vertical
        }
        ScrollBar {
            id: hbar
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            visible: parameterList.width < parameterList.contentWidth
            width: parameterList.width
            orientation: Qt.Horizontal
        }
    }
    onClosing: (close) => {
        if(!root.destroyingPlugin) {
            close.accepted = false;
            root.hide();
        }
    }
}
