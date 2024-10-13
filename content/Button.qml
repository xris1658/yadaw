import QtQuick
import QtQuick.Templates as T

import "MnemonicFunctions.js" as MnemonicFunctions

T.Button {
    id: root

    property int mnemonicTextLook: Mnemonic.MnemonicEnabled
    property alias radius: background.radius
    property alias border: background.border
    property alias backgroundColor: background.color
    property alias contentColor: label.color
    readonly property int maxBorderWidth: 2

    topPadding: 3
    bottomPadding: 3
    leftPadding: 5
    rightPadding: 5
    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0

    implicitWidth: implicitContentWidth + leftPadding + rightPadding
    implicitHeight: implicitContentHeight + topPadding + bottomPadding

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
        border.width: root.activeFocus? root.maxBorderWidth: 1
        color: (!root.enabled)?
            Colors.background:
            root.checked?
                root.down?
                    Colors.pressedCheckedButtonBackground:
                    root.hovered?
                        Colors.mouseOverCheckedButtonBackground:
                        Colors.checkedButtonBackground:
                root.down?
                    Colors.pressedControlBackground:
                    root.hovered?
                        Colors.mouseOverControlBackground:
                        Colors.controlBackground
    }

    contentItem: Label {
        id: label
        text: root.mnemonicTextLook === Mnemonic.MnemonicEnabled? MnemonicFunctions.mnemonicText(root.text):
            root.mnemonicTextLook === Mnemonic.MnemonicEnabledWithUnderline? MnemonicFunctions.mnemonicTextWithUnderline(root.text):
            root.text
        color: (!root.enabled)? Colors.disabledContent:
            root.checked?
                Colors.checkedButtonContent:
                Colors.content
        // anchors.fill: root
        // anchors.leftMargin: root.leftPadding
        // anchors.rightMargin: root.rightPadding
        // anchors.topMargin: root.topPadding
        // anchors.bottomMargin: root.bottomPadding
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    function locatePopupWindow(popupWindow: Window, belowY: int, aboveY: int) {
        let windowCoordinate = root.mapToGlobal(root.leftInset, belowY);
        if(windowCoordinate.y + popupWindow.height >= popupWindow.screen.desktopAvailableHeight) {
            windowCoordinate = mapToGlobal(root.leftInset, 0 + aboveY - popupWindow.height);
        }
        windowCoordinate.x = Math.min(windowCoordinate.x, popupWindow.screen.desktopAvailableWidth - popupWindow.width);
        popupWindow.x = windowCoordinate.x;
        popupWindow.y = windowCoordinate.y;
    }
}
