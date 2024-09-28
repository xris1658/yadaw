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
    Connections {
        target: menu
        function onOpened() {
            let nativePopup = menu.nativePopup;
            if(nativePopup) {
                let globalPoint = root.mapToGlobal(0, 0);
                nativePopup.locate(
                    Qt.rect(
                        globalPoint.x, globalPoint.y, root.width, root.height
                    ),
                    Qt.Vertical
                );
                nativePopup.showNormal();
                nativePopup.width = menu.implicitWidth;
                nativePopup.height = Math.min(
                    nativePopup.screen.desktopAvailableHeight,
                    menu.implicitHeight
                );
                menu.parent = nativePopup.contentItem;
                menu.x = 0;
                menu.y = 0;
            }
        }
    }
}
