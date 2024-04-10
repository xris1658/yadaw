import QtQuick

import QtQml.Models

Item {
    id: root
    property alias listView: listView
    property alias model: listView.model
    property alias vbar: vbar
    property alias hbar: hbar
    property ListModel headerListModel: ListModel {
        dynamicRoles: true
    }
    property bool headerResizeable
    ListView {
        id: listView
        anchors.fill: parent
        anchors.rightMargin: vbar.visible? vbar.width: 0
        anchors.bottomMargin: hbar.visible? hbar.height: 0
        ScrollBar.vertical: vbar
        ScrollBar.horizontal: hbar
        clip: true
        model: root.headerListModel
        orientation: ListView.Vertical
        headerPositioning: ListView.OverlayHeader
        header: ListView {
            z: 2
            id: headerListView
            width: listView.width
            property int textPadding: 2
            property int minimumColumnWidth: 10
            interactive: false
            model: root.headerListModel
            orientation: ListView.Horizontal
            Rectangle {
                id: headerBackground
                z: -1
                width: Math.max(parent.contentWidth, parent.width)
                height: parent.height
                color: Colors.controlBackground
                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: Colors.border
                }
            }
            delegate: MouseArea {
                id: headerItemDelegate
                height: headerLabel.height
                acceptedButtons: Qt.LeftButton
                Label {
                    id: headerLabel
                    leftPadding: headerListView.textPadding
                    rightPadding: headerResizeMouseArea.width
                    topPadding: headerListView.textPadding
                    bottomPadding: headerListView.textPadding
                    text: title
                    // clip: true
                }
                Rectangle {
                    anchors.right: parent.right
                    width: 1
                    height: parent.height
                    color: Colors.border
                }
                MouseArea {
                    id: headerResizeMouseArea
                    visible: root.headerResizeable
                    property double originalMouseX
                    anchors.right: parent.right
                    width: 5
                    height: parent.height
                    cursorShape: Qt.SizeHorCursor
                    preventStealing: true
                    onPressed: (mouse) => {
                        originalMouseX = mouseX;
                    }
                    onMouseXChanged: (mouse) => {
                        const delta = mouseX - originalMouseX;
                        if(headerItemDelegate.width + delta < headerListView.minimumColumnWidth) {
                            headerItemDelegate.width = headerListView.width;
                        }
                        else {
                            headerItemDelegate.width += delta;
                            originalMouseX = mouseX;
                        }
                    }
                }
                onWidthChanged: {
                    headerListView.model.setProperty(index, "columnWidth", width);
                }
                Component.onCompleted: {
                    width = headerListView.model.get(index).columnWidth;
                    if(index === 0) {
                        headerListView.height = height;
                    }
                }
            }
            onHeightChanged: {
                vbar.anchors.topMargin = height;
                headerBackground.height = height;
                listView.contentY = -1 * height;
            }
            Component.onCompleted: {
                listView.contentWidth = contentWidth;
            }
            onContentWidthChanged: {
                listView.contentWidth = contentWidth;
            }
        }
        highlight: Rectangle {
            color: Colors.highlightControlBackground
        }
        highlightFollowsCurrentItem: true
        highlightMoveDuration: 0
    }
    ScrollBar {
        id: vbar
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: listView.header? listView.header.height: 0
        anchors.bottom: listView.bottom
        visible: listView.height < listView.contentHeight
        orientation: Qt.Vertical
    }
    ScrollBar {
        id: hbar
        anchors.left: listView.left
        anchors.right: listView.right
        anchors.bottom: parent.bottom
        visible: listView.width < listView.contentWidth
        orientation: Qt.Horizontal
    }
}
