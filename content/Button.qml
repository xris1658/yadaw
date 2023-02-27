import QtQuick
import QtQuick.Templates as T

T.Button {
    id: root

    property int mnemonicTextLook: Mnemonic.MnemonicEnabled
    property alias radius: background.radius
    property alias border: background.border

    topPadding: 3
    bottomPadding: 3
    leftPadding: 5
    rightPadding: 5
    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0

    implicitWidth: label.contentWidth + leftPadding + rightPadding
    implicitHeight: label.contentHeight + topPadding + bottomPadding

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
        color: (!root.enabled)? Colors.background:
            root.down? Colors.pressedControlBackground:
            root.hovered? Colors.mouseOverControlBackground: Colors.controlBackground
    }

    contentItem: Label {
        id: label
        text: root.mnemonicTextLook === Mnemonic.MnemonicEnabled? Mnemonic.text(root.text):
            root.mnemonicTextLook === Mnemonic.MnemonicEnabledWithUnderline? Mnemonic.textWithUnderline(root.text):
            root.text
        anchors.fill: root
        anchors.leftMargin: root.leftPadding
        anchors.rightMargin: root.rightPadding
        anchors.topMargin: root.topPadding
        anchors.bottomMargin: root.bottomPadding
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}
