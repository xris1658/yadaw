import QtQuick
import QtQuick.Templates as T

import "MnemonicFunctions.js" as MnemonicFunctions

T.MenuBarItem {
    id: root

    property int mnemonicTextLook: Mnemonic.MnemonicEnabled

    leftPadding: 10
    rightPadding: 10
    topPadding: 3
    bottomPadding: 3
    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0

    implicitWidth: contentItem.contentWidth + leftPadding + rightPadding
    implicitHeight: contentItem.contentHeight + topPadding + bottomPadding

    background: Rectangle {
        color: (root.enabled && root.highlighted)? Colors.highlightMenuBarBackground: Colors.menuBarBackground
        border.width: 0
    }

    contentItem: Label {
        id: rootText
        anchors.centerIn: parent
        text: root.mnemonicTextLook === Mnemonic.MnemonicEnabled? MnemonicFunctions.mnemonicText(root.text):
            root.mnemonicTextLook === Mnemonic.MnemonicEnabledWithUnderline? MnemonicFunctions.mnemonicTextWithUnderline(root.text):
            root.text
        color: root.enabled? Colors.content: Colors.disabledContent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}
