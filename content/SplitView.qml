import QtQuick
import QtQuick.Templates as T

T.SplitView {
    id: root

    handle: Rectangle {
        implicitWidth: 5
        implicitHeight: 5
        color: T.SplitHandle.pressed? Colors.pressedControlBackground:
            root.enabled && T.SplitHandle.hovered? Colors.mouseOverControlBackground:
            "transparent"
    }
}
