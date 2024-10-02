import QtQml
import QtQuick
import QtQuick.Templates as T
import QtQuick.Layouts

T.Menu {
    id: root

    property NativePopup nativePopup: nativePopup
    property bool customPressedOutsideEvent: false
    property bool isSubMenu: false
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
    exit: Transition {
        id: menuTransition
        enabled: root.isSubMenu
        NumberAnimation {
            properties: "opacity"
            easing.type: Easing.BezierSpline
            easing.bezierCurve: [0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1]
            duration: root.isSubMenu? 250: 0
            from: 1.0
            to: 0.0
        }
    }
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
