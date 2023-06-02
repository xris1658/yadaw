import QtQuick
import QtQuick.Templates as T

T.TextField {
    id: root

    topPadding: 3
    bottomPadding: 3
    leftPadding: 5
    rightPadding: 5

    implicitHeight: placeholder.contentHeight + topPadding + bottomPadding

    font.family: "Fira Sans"

    color: root.enabled? Colors.content: Colors.disabledContent
    placeholderTextColor: Colors.secondaryContent
    selectionColor: Colors.secondaryContent
    selectedTextColor: Colors.controlBackground
    verticalAlignment: TextInput.AlignVCenter

    background: Rectangle {
        id: background
        anchors.fill: root
        anchors.leftMargin: root.leftInset
        anchors.rightMargin: root.rightInset
        anchors.topMargin: root.topInset
        anchors.bottomMargin: root.bottomInset
        color: root.enabled? Colors.controlBackground: Colors.background
        border.color: root.enabled?
            root.acceptableInput? Colors.controlBorder: Colors.unacceptableBorder:
            Colors.disabledControlBorder
        border.width: root.activeFocus? 2: 1
    }

    Label {
        id: placeholder
        z: background.z + 1
        anchors.fill: parent
        anchors.leftMargin: parent.leftPadding
        anchors.rightMargin: parent.rightPadding
        anchors.topMargin: parent.topPadding
        anchors.bottomMargin: parent.bottomPadding
        text: root.placeholderText
        color: root.placeholderTextColor
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        visible: root.displayText.length === 0
        elide: Text.ElideRight
    }
}
