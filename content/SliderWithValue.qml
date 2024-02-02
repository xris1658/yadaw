import QtQuick
import QtQuick.Templates as T

T.Slider {
    id: root

    property int thickness: 20
    property int length: 200
    property int grooveThickness: 4

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

    background: Rectangle {
        id: background
        anchors.fill: parent
        anchors.leftMargin: root.leftInset
        anchors.rightMargin: root.rightInset
        anchors.topMargin: root.topInset
        anchors.bottomMargin: root.bottomInset
        border.color: root.enabled? Colors.controlBorder: Colors.disabledControlBorder
        color: Colors.background
        Rectangle {
            width: root.position * (parent.width - parent.border.width * 2)
            height: parent.height - parent.border.width * 2
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: parent.border.width
            anchors.topMargin: parent.border.width
            color: Colors.sliderWithValueFilledBackground
        }
        Label {
            text: root.value
            anchors.left: parent.left
            anchors.leftMargin: (parent.height - contentHeight) / 2
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    handle: Item {
        id: handle
        width: 1
        height: parent.height
        opacity: 0
    }

}
