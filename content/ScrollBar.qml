import QtQuick
import QtQuick.Templates as T

T.ScrollBar {
    id: root

    property int thickness: 12

    width: horizontal? undefined: thickness
    height: horizontal? thickness: undefined

    minimumSize: horizontal? height / width: width / height
    policy: T.ScrollBar.AlwaysOn
    visible: policy !== T.ScrollBar.AlwaysOff
    background: Rectangle {
        color: Colors.controlBackground
    }
    contentItem: Rectangle {
        width: root.horizontal? Math.max(root.size, root.minimumSize) * root.width: root.width
        height: root.horizontal? root.height: Math.max(root.size, root.minimumSize) * root.height
        color: root.pressed? Colors.scrollBarHandlePressed:
            root.hovered? Colors.scrollBarHandleHighlight:
            Colors.scrollBarHandle
        border.width: root.enabled? 1: 0
        border.color: Colors.controlBorder
        opacity: (root.policy === T.ScrollBar.AlwaysOn) || (root.active && root.size < 1.0)? 1: 0
    }
}
