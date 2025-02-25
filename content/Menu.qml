import QtQml
import QtQuick
import QtQuick.Templates as T
import QtQuick.Layouts

T.Menu {
    id: root

    property NativePopup nativePopup: nativePopup
    property bool customPressedOutsideEvent: false
    property bool isSubMenu: false
    property string mnemonicRegex
    property string mnemonicRegexReplaceWith
    padding: 1
    overlap: 5

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
        contentWidth + leftPadding + rightPadding) + (contentHeight < implicitContentHeight? scrollBar.width: 0)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
        contentHeight + topPadding + bottomPadding)

    background: Rectangle {
        id: bg
        implicitWidth: 100
        implicitHeight: 10
        color: "transparent"
        border.color: Colors.controlBorder
    }
    contentItem: ListView {
        id: listView
        implicitHeight: contentHeight
        clip: true
        model: root.contentModel
        currentIndex: root.currentIndex
        interactive: false // TODO
        highlightMoveDuration: 0
        highlightResizeDuration: 0
        ScrollBar.vertical: ScrollBar {
            id: scrollBar
            visible: listView.contentHeight > listView.height
        }
    }
    delegate: MenuItem {
        z: 1
    }
    // A workaround to show `Menu`s out of its main window.
    // Qt 6.8 added `Popup.popupType` to show `Popup`s on a separate window, or
    // even better, show `Menu`s as native ones. Check out the documentation:
    // https://doc.qt.io/qt-6/qml-qtquick-controls-popup.html#popup-type
    // `Popup.popupType` in Qt 6.8 still has some undesired behavior. For
    // instance, the main window loses focus while there's a mouse click in
    // `Menu` with `popupType` of `Window`.
    // I'd better use my own solution for now. Maybe I'll refine the bahavior in
    // the future by using event filters.
    NativePopup {
        id: nativePopup
        onMousePressedOutside: {
            root.isSubMenu = false;
            if(!root.customPressedOutsideEvent) {
                root.close();
            }
        }
    }
    Component.onCompleted: {
        for(let i = 0; i < root.count; ++i) {
            let item = root.itemAt(i);
            bg.implicitWidth = Math.max(bg.implicitWidth, item.implicitWidth);
        }
        if(scrollBar.visible) {
            bg.implicitWidth += scrollBar.width;
        }
    }
    onClosed: {
        nativePopup.close();
    }
}
