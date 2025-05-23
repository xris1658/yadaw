import QtQml
import QtQuick
import QtQuick.Templates as T

T.ComboBox {
    id: root

    property alias radius: background.radius
    property alias border: background.border
    property string enabledRole
    property bool hideDisabledItem: false

    topPadding: 3
    bottomPadding: 3
    leftPadding: 5
    rightPadding: 5
    spacing: 3

    clip: true

    implicitWidth: 0
    implicitHeight: contentItem.contentHeight + topPadding + bottomPadding

    background: Rectangle {
        id: background
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
        color: root.editable? "transparent": root.enabled? Colors.content: Colors.disabledContent
        anchors.left: parent.left
        anchors.leftMargin: Math.max((root.height - contentHeight) / 2, root.leftPadding)
        anchors.right: indicator.left
        elide: Label.ElideRight
        anchors.verticalCenter: parent.verticalCenter
        TextField {
            id: textField
            visible: editable
            text: root.editText
            anchors.fill: parent
            anchors.topMargin: -root.topPadding
            anchors.bottomMargin: -root.bottomPadding
            anchors.leftMargin: -parent.anchors.leftMargin
            anchors.rightMargin: root.rightPadding
            inputMethodHints: root.inputMethodHints
            validator: root.validator
        }
    }
    indicator: Item {
        width: indicatorLabel.contentWidth
        height: root.height
        anchors.right: root.right
        anchors.rightMargin: root.rightPadding
        anchors.top: root.top
        Label {
            id: indicatorLabel
            anchors.centerIn: parent
            text: "\u25bc"
            color: root.enabled? Colors.content: Colors.disabledContent
        }
    }
    popup: Menu {
        y: root.height - 1
        z: 1
        width: root.width + (scrollBar.visible? scrollBar.width: 0)
        height: Math.min(listView.contentHeight + topPadding + bottomPadding, root.Window.height)
        contentItem: ListView {
            id: listView
            clip: true
            model: root.delegateModel
            currentIndex: root.currentIndex
            interactive: false
            ScrollBar.vertical: ScrollBar {
                id: scrollBar
                visible: listView.contentHeight > listView.height
            }
        }
        Component.onCompleted: {
            for(let i = 0; i < count; ++i) {
                listView.itemAtIndex(i).width = root.popup.width;
            }
        }
        onOpened: {
            root.forceActiveFocus();
        }
    }
    delegate: MenuItem {
        id: menuItem
        minimumSpaceBetweenTextAndShortcut: textField.leftPadding + textField.rightPadding
        width: root.width
        height: enabled? root.implicitHeight: 0
        enabled: root.enabledRole?
            (Array.isArray(root.model)?
                 modelData[root.enabledRole]:
                 model[root.enabledRole])
            : true
        text: root.textRole?
            (Array.isArray(root.model)?
                modelData[root.textRole]:
                model[root.textRole])
            : modelData
        clip: true
        visible: ((!enabled) && hideDisabledItem)? false: true
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
            height: contentHeight
            anchors.right: indicator.visible? indicator.left: parent.right
            anchors.left: parent.left
            color: menuItem.enabled? Colors.content: Colors.disabledContent
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
            let newWidth = contentItem.contentWidth + height + minimumSpaceBetweenTextAndShortcut;
            root.implicitWidth = Math.max(root.implicitWidth, newWidth);
        }
    }
}
