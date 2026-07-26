import QtQml
import QtQuick
import QtQuick.Templates as T
import QtQuick.Layouts

T.Menu {
    id: root

    property NativePopup nativePopup: nativePopup
    property bool customPressedOutsideEvent: false
    property bool isSubMenu: false
    property bool isMenuBarMenu: false
    property string mnemonicRegex
    property string mnemonicRegexReplaceWith
    padding: 1
    overlap: 5
    popupType: T.Popup.Window

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
        contentWidth + leftPadding + rightPadding) + (contentHeight < implicitContentHeight? scrollBar.width: 0)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
        contentHeight + topPadding + bottomPadding)

    background: Rectangle {
        id: bg
        implicitWidth: 100
        implicitHeight: 10
        // Workaround of visible white gap on high DPI displays.
        // Qt .
        // Guess it's that items in `Menu` cannot fully cover the entire content area
        // due to some differences of rounding while Qt automatically converts between
        // native pixels and device-independent pixels.
        color: Colors.controlBackground
        border.color: Colors.controlBorder
        border.width: 4
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
    // Locates a popup according to size of it, an adjacent rectangle that the
    // popup locates next to and the relative position between the menu and the
    // adjacent rectangle.
    // Parameters:
    // - adjacentRect: the adjacent rectangle that the popup locates next to.
    //   The coordinates are global (relative to the screen).
    // - orientation: a `Qt.Orientation` that indicates whether the popup shows
    //   above/below, or left/right to the adjacent rectangle.
    //   - If the orientation is `Qt.Horizontal`, then the popup shows at right
    //     of `adjacentRect` or left of `adjacentRect` if there's not enough
    //     space at right of `adjacentRect`. The popup is top-aligned with
    //     `adjacentRect` or bottom-aligned if there's not enough space.
    //   - If the orientation is `Qt.Vertical`, then the popup show below or
    //     above the rectangle if there's not enuogh space. The popup is
    //     left-aligned with `adjacentRect` or right-aligned with the screen if
    //     there's not enough space.
    //
    // Example:
    // - For a top-level `Menu` of `MenuBar`, `adjacentRect` is the global
    //   geometry of its `MenuBarItem`, and `orientation` is `Qt.Vertical`.
    // - For a sub menu, `adjacentRect` is the upper level `MenuItem`, and
    //   `orientation` is `Qt.Horizontal`.
    function globalPosition(adjacentRect: rect, orientation: int): point {
        if(orientation == Qt.Horizontal) {
            let posX = adjacentRect.x + adjacentRect.width;
            if(posX + width >= screen.desktopAvailableWidth) {
                posX = Math.max(0, adjacentRect.x - width);
            }
            let posY = adjacentRect.y;
            if(adjacentRect.y + height >= screen.desktopAvailableHeight) {
                posY = Math.max(0, adjacentRect.y + adjacentRect.height - height);
            }
            return Qt.point(posX, posY);
        }
        else if(orientation == Qt.Vertical) {
            let posY = adjacentRect.y + adjacentRect.height;
            if(posY + height >= screen.desktopAvailableHeight) {
                posY = Math.max(0, adjacentRect.y - height);
            }
            let posX = adjacentRect.x;
            if(posX + width >= screen.desktopAvailableWidth) {
                posX = screen.desktopAvailableWidth - width;
            }
            return Qt.point(posX, posY);
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
