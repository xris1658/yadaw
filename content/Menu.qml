import QtQml
import QtQuick
import QtQuick.Templates as T
import QtQuick.Layouts

T.Menu {
    id: root

    implicitWidth: 0
    implicitHeight: contentItem.contentHeight

    background: Rectangle {
        z: 2
        color: "transparent"
        border.color: Colors.controlBorder
    }
    contentItem: ListView {
        id: listView
        width: background.width
        height: background.height
        clip: true
        model: root.contentModel
        currentIndex: root.currentIndex
        interactive: false // TODO
        highlightMoveDuration: 0
        highlightResizeDuration: 0
    }
    delegate: MenuItem {
        z: 1
    }
    Component.onCompleted: {
        for(let i = 0; i < root.count; ++i) {
            let item = root.itemAt(i);
            root.implicitWidth = Math.max(root.implicitWidth, item.implicitWidth);
        }
    }
}
