import QtQuick

Button {
    id: root
    property bool bypassed: false
    leftPadding: bypassedButton.width

    implicitWidth: label.contentWidth + leftPadding + rightPadding
    implicitHeight: label.contentHeight + topPadding + bottomPadding

    checkable: true
    contentItem: Label {
        id: label
        text: root.text
        color: root.checked?
            Colors.checkedButtonContent:
            Colors.content
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
}
