import QtQuick
import QtQuick.Templates as T

T.ToolTip {
    id: root

    topPadding: 3
    bottomPadding: 3
    leftPadding: 5
    rightPadding: 5
    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0

    y: -height

    implicitWidth: contentItem.contentWidth + leftPadding + rightPadding
    implicitHeight: contentItem.contentHeight + topPadding + bottomPadding

    background: Rectangle {
        id: background
        width: root.width
        height: root.height
        anchors.topMargin: root.topInset
        anchors.bottomMargin: root.bottomInset
        anchors.leftMargin: root.leftInset
        anchors.rightMargin: root.rightInset
        border.color: Colors.border
        border.width: 1
        color: Colors.controlBackground
    }

    contentItem: Label {
        id: contentItem
        text: root.text
        wrapMode: Label.Wrap
        color: Colors.content
    }
}
