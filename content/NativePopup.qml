import QtQuick

Window {
    id: root
    flags: Qt.Tool | Qt.FramelessWindowHint | Qt.WindowDoesNotAcceptFocus
    visible: false
    signal mousePressedOutside()
    property QtObject activeFocusObject: null
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
    function locate(adjacentRect: rect, orientation: int) {
        if(orientation == Qt.Horizontal) {
            let posX = adjacentRect.x + adjacentRect.width;
            if(posX + width >= screen.desktopAvailableWidth) {
                posX = Math.max(0, adjacentRect.x - width);
            }
            let posY = adjacentRect.y;
            if(adjacentRect.y + height >= screen.desktopAvailableHeight) {
                posY = Math.max(0, adjacentRect.y + adjacentRect.height - height);
            }
            x = posX;
            y = posY;
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
            x = posX;
            y = posY;
        }
    }
}