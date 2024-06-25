import QtQuick

Button {
    id: root
    property bool bypassed: false
    leftPadding: bypassedButton.width
    rightPadding: indicator.width
    bottomPadding: controlRow.height
    text: "Send"

    implicitWidth: label.contentWidth + leftPadding + rightPadding
    implicitHeight: label.contentHeight + topPadding + bottomPadding + controlRow.height

    contentItem: Label {
        id: label
        text: root.text
        color: root.checked?
            Colors.checkedButtonContent:
            Colors.content
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: root.leftPadding
        anchors.rightMargin: root.rightPadding
        anchors.topMargin: root.topPadding
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
    Button {
        id: bypassedButton
        width: height
        height: root.height - root.bottomPadding
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
        height: root.height - root.bottomPadding
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
            border.width: 0
            implicitWidth: implicitContentWidth + leftPadding + rightPadding
            topPadding: 1
            bottomPadding: 1
            contentItem: Label {
                text: "PF"
                font.pointSize: Qt.application.font.pointSize * 0.9
            }
        }
        Button {
            border.width: 0
            implicitWidth: implicitContentWidth + leftPadding + rightPadding
            topPadding: 1
            bottomPadding: 1
            contentItem: Label {
                text: "-12.0 dB"
                font.pointSize: Qt.application.font.pointSize * 0.9
            }
        }
        Button {
            border.width: 0
            implicitWidth: implicitContentWidth + leftPadding + rightPadding
            topPadding: 1
            bottomPadding: 1
            contentItem: Label {
                text: "25L"
                font.pointSize: Qt.application.font.pointSize * 0.9
            }
        }
        Button {
            border.width: 0
            checkable: true
            implicitWidth: implicitContentWidth + leftPadding + rightPadding
            topPadding: 1
            bottomPadding: 1
            contentItem: Label {
                text: "∅"
                font.pointSize: Qt.application.font.pointSize * 0.9
            }
        }
    }
}
