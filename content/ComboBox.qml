import QtQml
import QtQuick
import QtQuick.Templates as T

T.ComboBox {
    id: root

    topPadding: 3
    bottomPadding: 3
    leftPadding: 5
    rightPadding: 5

    implicitHeight: contentItem.contentHeight + topPadding + bottomPadding

    background: Rectangle {
        width: root.width
        height: root.height
        border.color: root.enabled? Colors.controlBorder: Colors.disabledControlBorder
        border.width: root.activeFocus? 2: 1
        color: (!root.enabled)? Colors.background:
            root.down? Colors.pressedControlBackground:
            root.hovered? Colors.mouseOverControlBackground:
            Colors.controlBackground
    }
    contentItem: Label {
        text: root.displayText
        color: root.enabled? Colors.content: Colors.disabledContent
        anchors.left: parent.left
        anchors.leftMargin: Math.max((root.height - contentHeight) / 2, root.leftPadding)
        anchors.verticalCenter: parent.verticalCenter
    }
    indicator: Item {
        width: height
        height: root.height
        anchors.right: root.right
        anchors.top: root.top
        Label {
            anchors.centerIn: parent
            text: "\u25bc"
            color: root.enabled? Colors.content: Colors.disabledContent
        }
    }
    popup: Menu {
        y: root.height - 1
        z: 1
        width: root.width
        height: listView.contentHeight
        implicitHeight: contentHeight
        contentItem: ListView {
            id: listView
            clip: true
            model: root.delegateModel
            currentIndex: root.currentIndex
            interactive: false
            Component.onCompleted: {
                for(let i = 0; i < count; ++i) {
                    itemAtIndex(i).width = root.popup.width;
                }
            }
        }
    }
    delegate: MenuItem {
        id: menuItem
        minimumSpaceBetweenTextAndShortcut: 0
        text: root.textRole?
            (Array.isArray(root.model)?
                modelData[root.textRole]:
                model[root.textRole])
            : modelData
        height: root.height
        width: root.width
        highlighted: root.highlightedIndex === index
        background: Rectangle {
            color: (menuItem.enabled && menuItem.highlighted)?
                Colors.mouseOverControlBackground:
                Colors.controlBackground
        }
        contentItem: Label {
            id: contentText
            leftPadding: 5
            text: menuItem.text
            width: menuItem.width - menuItem.height
            anchors.left: parent.left
            color: root.enabled? Colors.content: Colors.disabledContent
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
        indicator: Item {
            visible: root.currentIndex === index
            anchors.right: parent.right
            anchors.rightMargin: (parent.height - height) / 2
            anchors.verticalCenter: parent.verticalCenter
            width: height
            height: parent.height
            Rectangle {
                anchors.centerIn: parent
                width: parent.height / 3
                height: width
                radius: width / 2
            }
        }
        Component.onCompleted: {
            let newWidth = contentItem.width + height;
            root.implicitWidth = Math.max(root.implicitWidth, newWidth);
        }
    }
}
