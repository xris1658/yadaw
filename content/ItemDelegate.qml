import QtQuick
import QtQuick.Templates as T

import "MnemonicFunctions.js" as MnemonicFunctions

T.ItemDelegate {
    id: root

    property int mnemonicTextLook: Mnemonic.MnemonicDisabled

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
        color: (!root.enabled)? Colors.background:
            root.highlighted? Colors.highlightControlBackground:
            root.hovered? Colors.mouseOverControlBackground: "transparent"
    }

    contentItem: Label {
        id: label
        text: root.mnemonicTextLook === Mnemonic.MnemonicEnabled? MnemonicFunctions.mnemonicText(root.text):
            root.mnemonicTextLook === Mnemonic.MnemonicEnabledWithUnderline? MnemonicFunctions.mnemonicTextWithUnderline(root.text):
            root.text
        color: root.enabled? Colors.content: Colors.disabledContent
        anchors.fill: root
        anchors.leftMargin: root.leftPadding
        anchors.rightMargin: root.rightPadding
        anchors.topMargin: root.topPadding
        anchors.bottomMargin: root.bottomPadding
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}
