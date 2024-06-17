import QtQuick

Rectangle {
    id: root
    color: Colors.background
    border.color: Colors.border
    height: 300

    property alias inputRouteListModel: inputRouteList.model
    property alias outputRouteListModel: outputRouteList.model
    property Window audioIOSelectorWindow

    Rectangle {
        id: splitter
        color: Colors.border
        anchors.horizontalCenter: parent.horizontalCenter
        width: 1
        height: root.height
    }
    Rectangle {
        color: "transparent"
        border.width: 1
        border.color: Colors.border
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: splitter.left
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.bottomMargin: 0
        anchors.rightMargin: -1 * splitter.width
        RouteList {
            id: inputRouteList
            anchors.fill: parent
            anchors.margins: 5
            audioIOSelectorWindow: root.audioIOSelectorWindow
            isInput: true
        }
    }
    Rectangle {
        color: "transparent"
        border.color: Colors.border
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: splitter.right
        anchors.rightMargin: 0
        anchors.topMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: -1 * splitter.width
        RouteList {
            id: outputRouteList
            anchors.fill: parent
            anchors.margins: 5
            audioIOSelectorWindow: root.audioIOSelectorWindow
            isInput: false
        }
    }
}