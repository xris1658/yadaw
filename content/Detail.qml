import QtQuick

Rectangle {
    id: root
    color: Colors.background
    border.color: Colors.controlBorder
    clip: true

    property string text
    property string secondaryText
    property string informativeText

    Flickable {
        id: contents
        anchors.fill: parent
        anchors.margins: root.border.width
        contentWidth: scrollBar.visible? width - scrollBar.width: width
        contentHeight: columnShell.height
        ScrollBar.vertical: ScrollBar {
            id: scrollBar
            visible: contents.height < contents.contentHeight
        }
        Item {
            id: columnShell
            width: parent.width
            height: column.height + column.anchors.margins * 2
            Column {
                id: column
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 10
                spacing: Qt.application.font.pixelSize * 0.5
                Label {
                    width: column.width
                    text: root.text
                    color: Colors.content
                    font.bold: true
                    font.pointSize: Qt.application.font.pointSize * 1.5
                    wrapMode: Text.Wrap
                }
                Label {
                    width: column.width
                    text: root.secondaryText
                    color: Colors.content
                    wrapMode: Text.Wrap
                }
                Item {
                    width: column.width
                    height: Qt.application.font.pixelSize * 0.5
                }
                Label {
                    width: column.width
                    text: root.informativeText
                    color: Colors.secondaryContent
                    wrapMode: Text.Wrap
                }
            }
        }
    }
}
