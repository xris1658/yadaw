import QtQuick

Button {
    id: root
    property bool bypassed: false
    leftPadding: bypassedButton.width
    rightPadding: replaceButton.width

    implicitWidth: label.contentWidth + leftPadding + rightPadding
    implicitHeight: label.contentHeight + topPadding + bottomPadding
    contentItem: Label {
        id: label
        text: root.text
        color: Colors.content
        anchors.fill: root
        anchors.leftMargin: root.leftPadding
        anchors.rightMargin: root.rightPadding
        anchors.topMargin: root.topPadding
        anchors.bottomMargin: root.bottomPadding
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
    Button {
        id: bypassedButton
        width: height
        height: parent.height
        z: 2
        property int contentDiameter: parent.height * 0.625
        leftInset: (width - contentDiameter) / 2
        rightInset: leftInset
        topInset: (height - contentDiameter) / 2
        bottomInset: topInset
        background: Rectangle {
            anchors.centerIn: parent
            width: bypassedButton.contentDiameter
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
            anchors.leftMargin: (parent.width - contentWidth) / 2
            anchors.topMargin: (parent.height - contentHeight) / 2
        }
    }
}
