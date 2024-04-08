import QtQuick
import QtQuick.Templates as T

T.MenuBar {
    id: root

    property int mnemonicTextLook: Mnemonic.MnemonicEnabled

    padding: 0
    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0
    spacing: 0

    implicitHeight: heightTest.height

    Item {
        clip: true
        MenuBarItem {
            id: heightTest
            width: 200
        }
    }

    background: Rectangle {
        color: Colors.menuBarBackground
    }

    contentItem: Row {
        spacing: root.spacing
        Repeater {
            model: root.contentModel
        }
    }

    delegate: MenuBarItem {}
}
