import QtQuick
import QtQuick.Shapes
import QtQuick.Templates as T

T.CheckBox {
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
        implicitWidth: implicitHeight
        implicitHeight: root.contentItem.contentHeight
        width: height
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
            anchors.fill: parent
            anchors.margins: parent.padding * 2
            visible: root.checkState === Qt.PartiallyChecked
            color: root.enabled? Colors.content: Colors.disabledContent
        }
        Shape {
            width: 80
            height: 80
            scale: parent.width > parent.height?
                (parent.height - 2 * parent.padding) / height:
                (parent.width - 2 * parent.padding) / width
            anchors.centerIn: parent
            visible: root.checkState === Qt.Checked
            layer.enabled: true
            layer.smooth: true
            layer.samples: 2
            ShapePath {
                strokeWidth: 1
                strokeColor: fillColor
                startX: 10; startY: 30
                PathLine { x: 30; y: 50 }
                PathLine { x: 70; y: 10 }
                PathLine { x: 70; y: 30 }
                PathLine { x: 30; y: 70 }
                PathLine { x: 10; y: 50 }
                PathLine { x: 10; y: 30 }
                fillColor: root.enabled? Colors.content: Colors.disabledContent
                fillRule: ShapePath.WindingFill
            }
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
