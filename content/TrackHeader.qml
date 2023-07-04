import QtQuick

Rectangle {
    id: root
    property int index
    property alias name: nameText.text
    property alias trackColor: header.color
    color: Colors.background
    Rectangle {
        id: header
        width: indexText.width
        height: root.height
        anchors.left: parent.left
        Label {
            id: indexText
            font.pointSize: Qt.application.font.pointSize * 0.75
            width: contentHeight * 3
            verticalAlignment: Label.AlignVCenter
            horizontalAlignment: Label.AlignHCenter
            topPadding: 3
            bottomPadding: 3
            text: index + 1
        }
    }
    Item {
        id: content
        width: root.width - header.width - 3
        height: root.height
        anchors.right: parent.right
        Row {
            Column {
                Label {
                    id: nameText
                    width: content.width
                    verticalAlignment: Label.AlignVCenter
                    horizontalAlignment: Label.AlignLeft
                    topPadding: 3
                    bottomPadding: 3
                }
                Row {
                    spacing: 3
                    Button {
                        text: "M"
                        topPadding: 1
                        bottomPadding: 1
                    }
                    Button {
                        text: "S"
                        topPadding: 1
                        bottomPadding: 1
                    }
                    Button {
                        text: "∅"
                        topPadding: 1
                        bottomPadding: 1
                    }
                    Button {
                        text: "R"
                        topPadding: 1
                        bottomPadding: 1
                    }
                }
            }
        }
    }
}
