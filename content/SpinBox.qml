import QtQuick
import QtQuick.Shapes
import QtQuick.Templates as T

T.SpinBox {
    id: root

    topPadding: 3
    bottomPadding: 3
    leftPadding: 5
    rightPadding: 5
    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0

    implicitWidth: contentItem.implicitWidth
    implicitHeight: contentItem.implicitHeight

    up.hovered: upButton.hovered
    up.indicator: upButton.contentItem
    up.pressed: upButton.pressed

    down.hovered: downButton.hovered
    down.indicator: downButton.contentItem
    down.pressed: downButton.pressed

    background: Rectangle {
        id: background
        width: root.width
        height: root.height
        anchors.topMargin: root.topInset
        anchors.bottomMargin: root.bottomInset
        anchors.leftMargin: root.leftInset
        anchors.rightMargin: root.rightInset
        border.color: root.enabled? Colors.controlBorder:
            Colors.disabledControlBorder
        border.width: root.activeFocus? 2: 1
        color: (!root.enabled)?
            Colors.background:
            root.checked?
                root.down?
                    Colors.pressedCheckedButtonBackground:
                    root.hovered?
                        Colors.mouseOverCheckedButtonBackground:
                        Colors.checkedButtonBackground:
                root.down?
                    Colors.pressedControlBackground:
                    root.hovered?
                        Colors.mouseOverControlBackground:
                        Colors.controlBackground
    }
    contentItem: TextField {
        id: contentItem
        anchors.fill: parent
        padding: 0
        leftInset: root.leftInset
        rightInset: root.rightInset
        topInset: root.topInset
        bottomInset: root.bottomInset
        text: root.textFromValue(root.value, root.locale)
        validator: root.validator
        readOnly: !root.editable
        inputMethodHints: root.inputMethod
        Item {
            width: height
            height: parent.height - anchors.rightMargin * 2
            anchors.right: parent.right
            anchors.rightMargin: 2
            anchors.verticalCenter: parent.verticalCenter
            Button {
                id: upButton
                width: height * 1.5
                height: parent.height / 2
                anchors.right: parent.right
                anchors.top: parent.top
                text: "\u25b4"
                enabled: root.value < root.to
                background: Rectangle {
                    color: parent.down?
                        Colors.pressedControlBackground:
                        parent.hovered && parent.enabled?
                            Colors.mouseOverControlBackground:
                            Colors.controlBackground
                }
                onClicked: {
                    root.increase();
                }
            }
            Button {
                id: downButton
                border.width: 0
                width: height * 1.5
                height: parent.height / 2
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                text: "\u25be"
                enabled: root.value > root.from
                background: Rectangle {
                    color: parent.down?
                        Colors.pressedControlBackground:
                        parent.hovered && parent.enabled?
                            Colors.mouseOverControlBackground:
                            Colors.controlBackground
                }
                onClicked: {
                    root.decrease();
                }
            }
        }
    }
}
