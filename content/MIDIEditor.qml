import QtQuick

Item {
    id: root

    Grid {
        columns: 3
        rowSpacing: 0
        columnSpacing: 0
        Item {
            id: upperLeft
            width: pianoKeysPlaceholder.width
            height: timeline.height
        }
        Item {
            id: timelinePlaceholder
            width: root.width - upperLeft.width - timelineOptionsPlaceholder.width
            height: timeline.height
            Timeline {
                id: timeline
                width: parent.width // FIXME
            }
        }
        Button {
            id: timelineOptionsPlaceholder
            width: vbarPlaceholder.width
            height: timeline.height
            text: "\u25bc"
            border.width: 0
        }
        SplitView {
            id: pianoKeysPlaceholder
            orientation: Qt.Vertical
            width: pianoKeys.width
            height: root.height - upperLeft.height - bottomLeft.height
            clip: true
            handle: Item {
                implicitWidth: 2
                implicitHeight: 2
                Rectangle {
                    width: parent.width
                    height: parent.height
                    anchors.right: parent.right
                    color: Colors.controlBorder
                }
            }
            PianoKeys {
                id: pianoKeys
                SplitView.minimumHeight: pianoKeysPlaceholder.height * 0.25
                reuseItems: true
                boundsBehavior: ListView.StopAtBounds
                ScrollBar.vertical: ScrollBar {
                    id: vbar
                    parent: vbarPlaceholder
                    height: pianoKeys.height
                }
            }
            Rectangle {
                id: expressionSelectorPlaceholder
                width: parent.width
                SplitView.preferredHeight: 128
                SplitView.minimumHeight: expressionSelector.height
                color: Colors.controlBackground
                ComboBox {
                    id: expressionSelector
                    anchors.bottom: parent.bottom
                    width: parent.width
                }
            }
        }
        Item {
            id: content
            width: timelinePlaceholder.width
            height: pianoKeysPlaceholder.height
        }
        Rectangle {
            id: vbarPlaceholder
            width: vbar.width
            height: pianoKeysPlaceholder.height
            color: Colors.controlBackground
        }
        Rectangle {
            id: bottomLeft
            width: pianoKeysPlaceholder.width
            height: hbarPlaceholder.height
            color: Colors.controlBackground
        }
        Rectangle {
            id: hbarPlaceholder
            width: timelinePlaceholder.width
            height: vbarPlaceholder.width
            color: Colors.controlBackground
        }
        Button {
            id: barOptions
            width: vbarPlaceholder.width
            height: hbarPlaceholder.height
            border.width: 0
            text: "\u25bc"
        }
    }
}
