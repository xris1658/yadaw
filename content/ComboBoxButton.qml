import QtQuick
import QtQuick.Layouts

Button {
    id: root

    checkable: true
    mnemonicTextLook: Mnemonic.MnemonicDisabled
    rightPadding: indicator.width

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
        color: (!root.enabled)?
            Colors.background:
            (root.checked || root.down)?
                Colors.pressedControlBackground:
                root.hovered?
                    Colors.mouseOverControlBackground:
                    Colors.controlBackground
    }

    contentItem: Label {
        id: label
        text: Constants.isMnemonicSupported?
                root.mnemonicTextLook === Mnemonic.MnemonicEnabled? MnemonicFunctions.mnemonicText(root.text):
                    root.mnemonicTextLook === Mnemonic.MnemonicEnabledWithUnderline? MnemonicFunctions.mnemonicTextWithUnderline(root.text):
                    root.text:
            MnemonicFunctions.removeMnemonicFromText(root.text, root.mnemonicRegex, root.mnemonicRegexReplaceWith)
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

    Label {
        id: indicator
        height: root.height - root.topInset - root.bottomInset
        width: height
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.rightMargin: root.rightInset
        anchors.topMargin: root.topInset
        anchors.bottomMargin: root.bottomInset
        text: "\u25bc"
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color: label.color
    }
}