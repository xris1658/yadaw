import QtQuick
import QtQuick.Templates as T

T.Button {
    id: root

    property int mnemonicTextLook: Mnemonic.MnemonicEnabled

    topPadding: 3
    bottomPadding: 3
    leftPadding: 5
    rightPadding: 5
    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0

    implicitWidth: contentItem.textWidth + leftPadding + rightPadding
    implicitHeight: contentItem.textHeight + topPadding + bottomPadding

    background: Rectangle {
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

    contentItem: Item {
        anchors.centerIn: root
        clip: true
        property alias textWidth: rootText.contentWidth
        property alias textHeight: rootText.contentHeight
        width: root.width - root.leftPadding - root.rightPadding
        height: rootText.contentHeight
        Label {
            id: rootText
            anchors.centerIn: parent
            text: root.mnemonicTextLook === Mnemonic.MnemonicEnabled? Mnemonic.text(root.text):
                root.mnemonicTextLook === Mnemonic.MnemonicEnabledWithUnderline? Mnemonic.textWithUnderline(root.text):
                root.text
            font.family: "Fira Sans"
            color: root.enabled? Colors.content: Colors.disabledContent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }
}
