import QtQuick
import QtQuick.Templates as T

T.Dial {
    id: root

    property int diameter: 50
    leftInset: 0
    rightInset: 0
    topInset: 0
    bottomInset: 0
    implicitWidth: diameter + root.leftInset + root.rightInset
    implicitHeight: diameter + root.topInset + root.bottomInset

    background: Rectangle {
        width: diameter
        height: width
        radius: width / 2
        x: root.leftInset
        y: root.topInset
        color: (!root.enabled)?
            Colors.background:
            root.pressed?
                Colors.pressedControlBackground:
                root.hovered?
                    Colors.mouseOverControlBackground:
                    Colors.controlBackground
        border.width: root.activeFocus? 2: 1
        border.color: root.enabled? Colors.controlBorder:
            Colors.disabledControlBorder
    }

    contentItem: Item {
        width: diameter
        height: width
        x: root.leftInset
        y: root.topInset
        rotation: root.angle
        Rectangle {
            width: 2
            height: root.diameter * 0.375 - anchors.topMargin
            antialiasing: true
            color: root.enabled? Colors.content: Colors.disabledContent
            anchors.top: parent.top
            anchors.topMargin: 2
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
