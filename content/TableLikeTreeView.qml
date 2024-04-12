import QtQuick

import QtQml.Models

Item {
    id: root
    property bool headerResizeable: true
    property ListView headerListView: headerListView
    property ListModel headerListModel: ListModel {
        dynamicRoles: true
    }
    property alias model: treeView.model
    property alias treeView: treeView
    ListView {
        id: headerListView
        z: 2
        width: parent.width
        ScrollBar.horizontal: hbar
        property int textPadding: 2
        property int minimumColumnWidth: 10
        model: root.headerListModel
        orientation: ListView.Horizontal
        delegate: MouseArea {
            id: headerItemDelegate
            height: headerLabel.height
            acceptedButtons: Qt.LeftButton
            Label {
                id: headerLabel
                width: parent.width
                leftPadding: headerListView.textPadding
                rightPadding: headerResizeMouseArea.width
                topPadding: headerListView.textPadding
                bottomPadding: headerListView.textPadding
                text: title
                elide: Label.ElideRight
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
                headerListModel.setProperty(index, "columnWidth", width);
            }
            Component.onCompleted: {
                width = headerListView.model.get(index).columnWidth;
                if(index === 0) {
                    headerListView.height = height;
                }
            }
        }
        Rectangle {
            z: -1
            anchors.fill: parent
            color: Colors.controlBackground
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: Colors.border
            }
        }
        onContentWidthChanged: {
            treeView.columnWidthProvider = (column) => {
                return headerListView.contentWidth;
            }
        }
    }
    TreeView {
        id: treeView
        property int columnWidth: headerListView.contentWidth
        z: 1
        anchors.fill: parent
        topMargin: headerListView.height
        anchors.rightMargin: vbar.visible? vbar.width: 0
        anchors.bottomMargin: hbar.visible? hbar.height: 0
        ScrollBar.vertical: vbar
        ScrollBar.horizontal: hbar
    }
    ScrollBar {
        id: vbar
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: headerListView? headerListView.height: 0
        anchors.bottom: treeView.bottom
        visible: treeView.height < treeView.contentHeight
        orientation: Qt.Vertical
    }
    ScrollBar {
        id: hbar
        anchors.left: treeView.left
        anchors.right: treeView.right
        anchors.bottom: parent.bottom
        visible: treeView.width < treeView.contentWidth
        orientation: Qt.Horizontal
    }
}
