import QtQuick
import QtQuick.Templates as T

import "MnemonicFunctions.js" as MnemonicFunctions

T.MenuBarItem {
    id: root

    property int mnemonicTextLook: Mnemonic.MnemonicEnabled
    property string mnemonicRegex: (menu && menu.mnemonicRegex)? menu.mnemonicRegex: Constants.mnemonicRegex
    property string mnemonicRegexReplaceWith: (menu && menu.mnemonicRegexReplaceWith)? menu.mnemonicRegexReplaceWith: Constants.mnemonicRegexReplaceWith

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
        text: Constants.isMnemonicSupported?
                root.mnemonicTextLook === Mnemonic.MnemonicEnabled? MnemonicFunctions.mnemonicText(root.text):
                    root.mnemonicTextLook === Mnemonic.MnemonicEnabledWithUnderline? MnemonicFunctions.mnemonicTextWithUnderline(root.text):
                    root.text:
            MnemonicFunctions.removeMnemonicFromText(root.text, root.mnemonicRegex, root.mnemonicRegexReplaceWith)
        color: root.enabled? Colors.content: Colors.disabledContent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
    QtObject {
        id: impl
        property bool menuOpenedWithClick: false
    }
    Connections {
        id: connectToMenu
        target: menu
        enabled: Global.enableMenuPopup
        property bool opened: false
        property bool menuOpenedWithClick: false
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
                nativePopup.showWithoutActivating();
                nativePopup.width = menu.implicitWidth;
                nativePopup.height = Math.min(
                    nativePopup.screen.desktopAvailableHeight,
                    menu.implicitHeight
                );
                menu.parent = nativePopup.contentItem;
                menu.x = 0;
                menu.y = 0;
                let quickMenuBarEventFilterModel = Global.quickMenuBarEventFilterModel;
                if(quickMenuBarEventFilterModel && !opened) {
                    quickMenuBarEventFilterModel.append(nativePopup, true);
                    opened = true;
                }
            }
        }
        function onClosed() {
            let nativePopup = menu.nativePopup;
            if(nativePopup) {
                menu.parent = root;
                let quickMenuBarEventFilterModel = Global.quickMenuBarEventFilterModel;
                if(quickMenuBarEventFilterModel) {
                    quickMenuBarEventFilterModel.remove(nativePopup);
                }
            }
            impl.menuOpenedWithClick = false;
            opened = false;
        }
    }
    Connections {
        target: menu && menu.nativePopup? menu.nativePopup: null
        enabled: target != null
        function onMousePressedOutside() {
            if(impl.menuOpenedWithClick) {
                impl.menuOpenedWithClick = false;
            }
            else {
                menu.customPressedOutsideEvent = false;
                menu.close();
            }
        }
    }
    onPressed: {
        menu.customPressedOutsideEvent = true;
        impl.menuOpenedWithClick = true;
    }
}
