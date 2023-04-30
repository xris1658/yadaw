import QtQuick
import QtQuick.Templates as T

T.Slider {
    id: root

    property int thickness: 12
    property int length: 200
    property int grooveThickness: 4
    property int handleThickness: 12
    property int handleLength: 16

    property alias groove: groove

    width: horizontal? length: thickness
    height: horizontal? thickness: length

    topPadding: 3
    bottomPadding: 3
    leftPadding: 5
    rightPadding: 5
    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0

    background: Item {
        id: background
        anchors.fill: parent
        anchors.leftMargin: root.leftInset
        anchors.rightMargin: root.rightInset
        anchors.topMargin: root.topInset
        anchors.bottomMargin: root.bottomInset
        Rectangle {
            id: groove
            anchors.centerIn: parent
            width: root.vertical? root.grooveThickness: parent.width
            height: root.vertical? parent.height: root.grooveThickness
            color: root.enabled? Colors.controlBackground: Colors.background
            border.color: root.enabled? Colors.controlBorder: Colors.disabledControlBorder
        }
    }
    handle: Rectangle {
        id: handle
        width: root.vertical? root.handleThickness: root.handleLength
        height: root.vertical? root.handleLength: root.handleThickness
        color: root.enabled? Colors.content: Colors.disabledContent
        border.color: root.enabled? Colors.controlBorder: Colors.disabledControlBorder
        border.width: root.activeFocus? 2: 1
        anchors.verticalCenter: root.vertical? undefined: root.verticalCenter
        radius: 2
        x: root.vertical? (background.width - handle.width) / 2: (background.width - handle.width) * root.position
        y: root.vertical? (background.height - handle.height) * (1 - root.position): (background.height - handle.height) / 2
    }
}
