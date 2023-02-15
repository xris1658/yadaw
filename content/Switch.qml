import QtQuick
import QtQuick.Templates as T
import QtQuick.Shapes

T.Switch {
    id: root

    implicitWidth: indicator.width + spacing + contentItem.contentWidth + leftPadding + rightPadding
    implicitHeight: contentItem.height + topPadding + bottomPadding

    padding: 0
    spacing: 5
    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0

    background: Item {
        width: root.width
        height: root.height
        anchors.fill: root
    }
    indicator: Rectangle {
        property int padding: 2
        width: height * 1.5
        height: root.height - root.topPadding - root.bottomPadding
        anchors.left: root.left
        anchors.leftMargin: root.leftPadding
        anchors.top: root.top
        anchors.topMargin: root.topPadding
        border.color: root.enabled? Colors.controlBorder:
            Colors.disabledControlBorder
        border.width: root.activeFocus? 2: 1
        color: (!root.enabled)? Colors.background:
            root.down? Colors.pressedControlBackground:
            root.hovered? Colors.mouseOverControlBackground: Colors.controlBackground
        Rectangle {
            id: partialCheckedIndicator
            x: root.checked? parent.width - width - parent.padding * 2: parent.padding * 2
            y: parent.padding * 2
            width: parent.width / 2 - parent.padding * 2
            height: parent.height - y * 2
            color: root.enabled? Colors.content: Colors.disabledContent
        }
    }
    contentItem: Label {
        anchors.left: root.indicator.right
        anchors.leftMargin: root.spacing
        anchors.verticalCenter: root.verticalCenter
        height: contentHeight
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        text: root.text
        color: root.enabled? Colors.content: Colors.disabledContent
    }
}