import QtQuick

Button {
    id: root
    property bool bypassed: false
    leftPadding: bypassedButton.width
    rightPadding: indicator.width
    text: "Send"

    implicitWidth: label.contentWidth + leftPadding + rightPadding
    implicitHeight: implicitContentHeight + topPadding + bottomPadding + controlRow.height

    contentItem: Label {
        id: label
        text: root.text
        color: root.checked?
            Colors.checkedButtonContent:
            Colors.content
        anchors.fill: parent
        anchors.leftMargin: root.leftPadding
        anchors.rightMargin: root.rightPadding
        anchors.topMargin: root.topPadding
        anchors.bottomMargin: root.bottomPadding + controlRow.height
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
    Button {
        id: bypassedButton
        width: height
        height: root.height - controlRow.height
        z: 2
        property int contentDiameter: height * 0.625
        leftInset: (width - contentDiameter) / 2
        rightInset: leftInset
        topInset: (height - contentDiameter) / 2
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
    Label {
        id: indicator
        height: root.height - controlRow.height
        width: height
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: root.rightInset
        anchors.topMargin: root.topInset
        text: "\u25bc"
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color: label.color
    }
    Row {
        id: controlRow
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.bottomMargin: root.maxBorderWidth
        anchors.leftMargin: root.maxBorderWidth
        Button {
            id: sendFromButton
            border.width: 0
            topPadding: 1
            bottomPadding: 1
            contentItem: Label {
                text: "PF"
                font.pointSize: Qt.application.font.pointSize * 0.9
            }
        }
        Button {
            id: sendFaderButton
            border.width: 0
            topPadding: 1
            bottomPadding: 1
            width: (root.width - root.maxBorderWidth * 2 - sendFromButton.width - sendPolarityInverterButton.width) * 0.6
            contentItem: Label {
                text: "-12.0"
                verticalAlignment: Label.AlignVCenter
                horizontalAlignment: Label.AlignHCenter
                font.pointSize: Qt.application.font.pointSize * 0.9
            }
        }
        Button {
            id: sendPanningButton
            border.width: 0
            topPadding: 1
            bottomPadding: 1
            width: root.width - root.maxBorderWidth * 2 - sendFromButton.width - sendFaderButton.width - sendPolarityInverterButton.width
            contentItem: Label {
                text: "25L"
                verticalAlignment: Label.AlignVCenter
                horizontalAlignment: Label.AlignHCenter
                font.pointSize: Qt.application.font.pointSize * 0.9
            }
        }
        Button {
            id: sendPolarityInverterButton
            border.width: 0
            checkable: true
            topPadding: 1
            bottomPadding: 1
            contentItem: Label {
                text: "∅"
                font.pointSize: Qt.application.font.pointSize * 0.9
            }
        }
    }
}
