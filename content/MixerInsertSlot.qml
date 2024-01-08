import QtQuick

Button {
    id: root
    property bool bypassed: false
    anchors.fill: parent
    leftPadding: bypassedButton.width
    rightPadding: replaceButton.width
    z: 1
    Button {
        id: bypassedButton
        width: height * 0.75
        height: parent.height
        z: 2
        property int contentDiameter: parent.height / 3
        leftInset: (width - contentDiameter) / 2
        rightInset: leftInset
        topInset: (height - contentDiameter) / 2
        bottomInset: topInset
        background: Rectangle {
            anchors.centerIn: parent
            width: parent.contentDiameter
            height: width
            radius: width / 2
            color: root.bypassed? Colors.background: "#40FF88"
            border.color: Colors.secondaryBackground
        }
        onClicked: {
            root.bypassed = !root.bypassed;
        }
    }
    Button {
        id: replaceButton
        anchors.right: parent.right
        width: height * 0.75
        height: parent.height
        z: 2
        leftInset: (parent.width - contentItem.contentWidth) / 2
        rightInset: leftInset
        topInset: (parent.height - contentItem.contentHeight) / 2
        bottomInset: topInset
        background: Item {}
        contentItem: Label {
            text: "\u25bc"
            anchors.centerIn: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pointSize: Qt.application.font.pointSize * 0.5
            anchors.leftMargin: (parent.width - contentWidth) / 2
            anchors.topMargin: (parent.height - contentHeight) / 2
        }
    }
}
