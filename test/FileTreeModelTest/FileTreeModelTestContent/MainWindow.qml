import QtQuick
import QtQuick.Controls
import QtQuick.Window

Window {
    id: root
    visible: true
    width: 500
    height: 500
    color: "#202020"
    property alias treeModel: treeView.model
    TreeView {
        id: treeView
        anchors.fill: parent
        contentWidth: width
        delegate: ItemDelegate {
            id: treeDelegate
            width: treeView.contentWidth

            // Assigned to by TreeView:
            required property TreeView treeView
            required property bool isTreeNode
            required property bool expanded
            required property int hasChildren
            required property int depth
            leftPadding: indicator.x + height
            rightPadding: 5
            text: ftm_name
            TapHandler {
                onTapped: treeView.toggleExpanded(row)
            }
            Label {
                id: indicator
                width: height
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                visible: treeDelegate.isTreeNode && treeDelegate.hasChildren
                x: (treeDelegate.depth * parent.height)
                text: treeDelegate.expanded? "\u25be": "\u25b8"
            }
        }
    }
}