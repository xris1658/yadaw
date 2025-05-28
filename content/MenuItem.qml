import QtQuick
import QtQuick.Shapes
import QtQuick.Templates as T

import "MnemonicFunctions.js" as MnemonicFunctions

T.MenuItem {
    id: root

    property int mnemonicTextLook: Mnemonic.MnemonicEnabled
    property string mnemonicRegex: Constants.mnemonicRegex
    property string mnemonicRegexReplaceWith: Constants.mnemonicRegexReplaceWith

    property int minimumSpaceBetweenTextAndShortcut: 30

    property bool showIndicatorAsRadio: false

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
            text: Constants.isMnemonicSupported?
                    root.mnemonicTextLook === Mnemonic.MnemonicEnabled? MnemonicFunctions.mnemonicText(root.text):
                        root.mnemonicTextLook === Mnemonic.MnemonicEnabledWithUnderline? MnemonicFunctions.mnemonicTextWithUnderline(root.text):
                        root.text:
                MnemonicFunctions.removeMnemonicFromText(root.text, root.mnemonicRegex, root.mnemonicRegexReplaceWith)
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
    indicator: Item {
        width: height
        height: root.height
        Rectangle {
            anchors.centerIn: parent
            width: height
            height: contentText.font.pixelSize / 2
            radius: width / 2
            color: Colors.content
            visible: root.showIndicatorAsRadio && root.checked
        }
        Rectangle {
            width: height
            height: contentText.font.pixelSize
            anchors.left: parent.left
            anchors.leftMargin: (root.height - height) / 2
            anchors.verticalCenter: parent.verticalCenter
            color: "transparent"
            border.color: Colors.controlBorder
            visible: root.checkable && (!root.showIndicatorAsRadio)
            Shape {
                width: 80
                height: 80
                transformOrigin: Item.Center
                scale: parent.width > parent.height?
                    (parent.height - 2) / height:
                    (parent.width - 2) / width
                anchors.centerIn: parent
                visible: root.checked
                layer.enabled: true
                layer.smooth: true
                layer.samples: 2
                ShapePath {
                    strokeWidth: 1
                    strokeColor: fillColor
                    startX: 10; startY: 30
                    PathLine { x: 30; y: 50 }
                    PathLine { x: 70; y: 10 }
                    PathLine { x: 70; y: 30 }
                    PathLine { x: 30; y: 70 }
                    PathLine { x: 10; y: 50 }
                    PathLine { x: 10; y: 30 }
                    fillColor: root.enabled? Colors.content: Colors.disabledContent
                    fillRule: ShapePath.WindingFill
                }
            }
        }
    }
    Connections {
        target: subMenu
        enabled: Global.enableMenuPopup
        property bool opened: false
        function onOpened() {
            subMenu.isSubMenu = true;
            subMenu.isMenuBarMenu = menu.isMenuBarMenu;
            let subMenuPopup = subMenu.nativePopup;
            if(subMenuPopup) {
                let globalPoint = root.mapToGlobal(subMenu.overlap, -1 * subMenu.topPadding);
                subMenuPopup.locate(
                    Qt.rect(
                        globalPoint.x, globalPoint.y,
                        root.width - subMenu.overlap * 2, root.height + 2 * subMenu.bottomPadding
                    ),
                    Qt.Horizontal
                );
                subMenuPopup.showWithoutActivating();
                subMenuPopup.width = subMenu.implicitWidth;
                subMenuPopup.height = Math.min(
                    subMenuPopup.screen.desktopAvailableHeight,
                    subMenu.implicitHeight
                );
                subMenu.parent = subMenuPopup.contentItem;
                subMenu.width = subMenuPopup.width;
                subMenu.height = subMenuPopup.height;
                subMenu.x = 0;
                subMenu.y = 0;
                let quickMenuBarEventFilterModel = menu.isMenuBarMenu?
                    Global.quickMenuBarEventFilterModel:
                    Global.nativePopupEventFilterModel;
                if(quickMenuBarEventFilterModel && !opened) {
                    let menuPopup = root.menu.nativePopup;
                    if(menuPopup) {
                        quickMenuBarEventFilterModel.popupShouldReceiveKeyEvents(
                            menuPopup, false
                        );
                    }
                    quickMenuBarEventFilterModel.append(subMenuPopup, true);
                    opened = true;
                }
            }
        }
        function onClosed() {
            let menuPopup = root.menu.nativePopup;
            if(menuPopup) {
                let quickMenuBarEventFilterModel = menu.isMenuBarMenu?
                    Global.quickMenuBarEventFilterModel:
                    Global.nativePopupEventFilterModel;
                if(quickMenuBarEventFilterModel) {
                    quickMenuBarEventFilterModel.popupShouldReceiveKeyEvents(
                        menuPopup, true
                    );
                    let subMenuPopup = subMenu.nativePopup;
                    if(subMenuPopup) {
                        quickMenuBarEventFilterModel.remove(subMenuPopup);
                    }
                }
            }
            subMenu.isMenuBarMenu = false;
            opened = false;
        }
    }
    Connections {
        target: subMenu && subMenu.nativePopup? subMenu.nativePopup: null
        function onMousePressedOutside() {
            subMenu.nativePopup.close();
        }
    }
    Connections {
        target: menu
        function onAboutToHide() {
            if(subMenu) {
                subMenu.isSubMenu = false;
            }
        }
    }
}
