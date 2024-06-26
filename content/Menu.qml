import QtQml
import QtQuick
import QtQuick.Templates as T
import QtQuick.Layouts

T.Menu {
    id: root
    padding: 1

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
        contentWidth + leftPadding + rightPadding)
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
    Component.onCompleted: {
        for(let i = 0; i < root.count; ++i) {
            let item = root.itemAt(i);
            bg.implicitWidth = Math.max(bg.implicitWidth, item.implicitWidth);
        }
        if(scrollBar.visible) {
            bg.implicitWidth += scrollBar.width;
        }
    }
}
