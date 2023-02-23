import QtQuick
import QtQuick.Templates as T

T.TabButton {
    id: root

    topPadding: 3
    bottomPadding: 3
    leftPadding: 5
    rightPadding: 5
    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0

    implicitWidth: label.contentWidth + leftPadding + rightPadding
    implicitHeight: label.contentHeight + topPadding + bottomPadding

    background: Rectangle {
        anchors.fill: parent
        anchors.topMargin: root.topInset
        anchors.bottomMargin: root.bottomInset
        anchors.leftMargin: root.leftInset
        anchors.rightMargin: root.rightInset
        color: (!root.enabled)? Colors.background:
            root.down? Colors.pressedControlBackground:
            root.hovered? Colors.mouseOverControlBackground:
            root.checked? Colors.highlightControlBackground:
            Colors.controlBackground
    }

    contentItem: Label {
        id: label
        anchors.fill: parent
        anchors.leftMargin: root.leftPadding
        anchors.rightMargin: root.rightPadding
        anchors.topMargin: root.topPadding
        anchors.bottomMargin: root.bottomPadding
        text: root.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}
