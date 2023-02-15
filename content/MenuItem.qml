import QtQuick
import QtQuick.Shapes
import QtQuick.Templates as T

T.MenuItem {
    id: root

    property int mnemonicTextLook: Mnemonic.MnemonicEnabled

    property int minimumSpaceBetweenTextAndShortcut: 30

    clip: false
    padding: 2
    implicitWidth: contentItem.implicitWidth + 2 * height
    implicitHeight: contentItem.height + topPadding + bottomPadding

    background: Rectangle {
        color: (root.enabled && root.highlighted)? Colors.mouseOverControlBackground: Colors.controlBackground
    }
    contentItem: Item {
        implicitWidth: contentText.contentWidth + contentShortcut.contentWidth + root.minimumSpaceBetweenTextAndShortcut
        height: contentText.contentHeight
        anchors.left: parent? parent.left: undefined
        anchors.leftMargin: parent? parent.height: undefined
        anchors.verticalCenter: parent? parent.verticalCenter: undefined
        Label {
            id: contentText
            text: root.mnemonicTextLook === Mnemonic.MnemonicEnabled? Mnemonic.text(root.text):
                root.mnemonicTextLook === Mnemonic.MnemonicEnabledWithUnderline? Mnemonic.textWithUnderline(root.text):
                root.text
            anchors.left: parent.left
            color: root.enabled? Colors.content: Colors.disabledContent
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
        Label {
            id: contentShortcut
            text: root.action? root.action.shortcut: ""
            anchors.right: parent.right
            anchors.rightMargin: parent.height + root.height / 2
            color: root.enabled? Colors.content: Colors.disabledContent
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }
    arrow: Item {
        anchors.right: parent.right
        anchors.top: parent.top
        width: height
        height: root.height
        Shape {
        visible: root.subMenu != null
            width: 8
            height: 8
            anchors.centerIn: parent
            layer.enabled: true
            layer.samples: 4
            antialiasing: true
            smooth: true
            ShapePath {
                strokeWidth: 1
                strokeColor: root.enabled? Colors.content: Colors.disabledContent
                startX: 2; startY: 0
                PathLine { x: 6; y: 4 }
                PathLine { x: 2; y: 8 }
                fillColor: strokeColor
                fillRule: ShapePath.WindingFill
            }
        }
    }
    indicator: CheckBox {
        anchors.left: parent.left
        anchors.leftMargin: (root.height - height) / 2
        anchors.verticalCenter: parent.verticalCenter
        width: height
        height: contentText.font.pixelSize
        enabled: root.enabled
        visible: root.checkable
        checked: root.checked
    }
}
