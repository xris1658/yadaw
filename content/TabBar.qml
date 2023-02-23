import QtQuick
import QtQuick.Templates as T

T.TabBar {
    id: root
    padding: 0
    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0

    implicitWidth: listView.width + leftPadding + rightPadding
    implicitHeight: listView.height + topPadding + bottomPadding

    background: Item {
        width: root.width
        height: root.height
        anchors.topMargin: root.topInset
        anchors.bottomMargin: root.bottomInset
        anchors.leftMargin: root.leftInset
        anchors.rightMargin: root.rightInset
    }

    contentItem: ListView {
        id: listView
        width: contentWidth
        height: tabButton.height
        currentIndex: root.currentIndex
        orientation: ListView.Horizontal
        spacing: root.spacing
        highlightMoveDuration: 0
        highlightResizeDuration: 0
        model: root.contentModel
        boundsBehavior: Flickable.StopAtBounds
    }

    Item {
        TabButton {
            // Used to set height of the control - is there a better way?
            id: tabButton
            visible: false
        }
    }
}
