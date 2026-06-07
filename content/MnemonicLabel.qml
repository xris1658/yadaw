import QtQuick.Templates as T
import QtQuick

// A sloppy implementation of static text with mnemonic support:
// https://learn.microsoft.com/en-us/windows/win32/controls/about-static-controls#text-static-control:~:text=Text%2E-,By,order%29%2E
T.Control {
    id: root

    property string text
    property int mnemonicTextLook: Mnemonic.MnemonicEnabled
    property string mnemonicRegex: Constants.mnemonicRegex
    property string mnemonicRegexReplaceWith: Constants.mnemonicRegexReplaceWith
    property Item mnemonicTarget: null
    readonly property Label label: label

    topPadding: label.topPadding
    bottomPadding: label.bottomPadding
    leftPadding: label.leftPadding
    rightPadding: label.rightPadding
    implicitWidth: implicitContentWidth + leftPadding + rightPadding
    implicitHeight: implicitContentHeight + topPadding + bottomPadding
    topInset: 0
    bottomInset: height
    leftInset: 0
    rightInset: width
    focus: false
    activeFocusOnTab: false

    background: Item {
        width: 0
        height: 0
    }
    contentItem: Label {
        id: label
        width: root.width
        height: root.height
        text: Constants.isMnemonicSupported?
                root.mnemonicTextLook === Mnemonic.MnemonicEnabled? MnemonicFunctions.mnemonicText(root.text):
                    root.mnemonicTextLook === Mnemonic.MnemonicEnabledWithUnderline? MnemonicFunctions.mnemonicTextWithUnderline(root.text):
                    root.text:
            MnemonicFunctions.removeMnemonicFromText(root.text, root.mnemonicRegex, root.mnemonicRegexReplaceWith)
        color: Colors.content
    }
    T.AbstractButton {
        width: 0
        height: 0
        text: root.text
        focus: false
        activeFocusOnTab: false
        enabled: Constants.isMnemonicSupported
        onClicked: {
            if(mnemonicTarget) {
                mnemonicTarget.forceActiveFocus(Qt.ShortcutFocusReason);
            }
        }
    }
}