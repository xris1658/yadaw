import QtQuick
import QtQuick.Layouts

GridLayout {
    id: root
    columns: 3
    rows: 3
    rowSpacing: 0
    columnSpacing: 0
    flow: GridLayout.LeftToRight
    Item {
        id: upperLeft
        width: pianoKeysPlaceholder.width
        height: timelinePlaceholder.height
    }
    Item {
        id: timelinePlaceholder
        Layout.fillWidth: true
        height: timeline.height
        Timeline {
            id: timeline
            width: parent.width // FIXME
        }
    }
    Button {
        id: timelineOptionsPlaceholder
        Layout.fillWidth: false
        Layout.preferredWidth: vbarPlaceholder.width
        Layout.preferredHeight: timelinePlaceholder.height
        text: "\u25bc"
        border.width: 0
    }
    SplitView {
        id: pianoKeysPlaceholder
        orientation: Qt.Vertical
        width: pianoKeys.width
        Layout.fillHeight: true
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
            SplitView.fillHeight: true
            spacing: 1
            reuseItems: true
            boundsBehavior: ListView.StopAtBounds
            ScrollBar.vertical: vbar
            Component.onCompleted: {
                vbar.position = 1 - 60/*C5*/ / 128;
            }
        }
        Rectangle {
            id: expressionSelectorPlaceholder
            width: parent.width
            SplitView.preferredHeight: 128
            SplitView.minimumHeight: expressionSelector.height
            SplitView.fillHeight: false
            color: Colors.controlBackground
            ComboBox {
                id: expressionSelector
                anchors.bottom: parent.bottom
                width: parent.width
            }
        }
    }
    Item {
        id: contentAndExpression
        Layout.fillWidth: true
        Layout.fillHeight: true
        Flickable {
            id: contentPlaceholder
            width: parent.width
            height: pianoKeys.height
            clip: true
            ScrollBar.horizontal: hbar
            Item {
                id: content
                width: parent.width // FIXME
                height: pianoKeys.contentHeight
                y: -pianoKeys.contentY - pianoKeys.contentHeight
                Repeater {
                    model: 127
                    Rectangle {
                        width: content.width
                        height: 1
                        color: Colors.secondaryBorder
                        x: 0
                        y: (pianoKeys.keyHeight + height) * (index + 1) - 1
                    }
                }
            }
        }
        Rectangle {
            width: parent.width
            height: 2
            color: Colors.border
            anchors.top: contentPlaceholder.bottom
        }
        Flickable {
            id: expressionFlickable
            width: parent.width
            height: expressionSelectorPlaceholder.height
            anchors.bottom: parent.bottom
        }
    }
    Item {
        id: vbarPlaceholder
        Layout.preferredWidth: vbar.width
        Layout.fillHeight: true
        ScrollBar {
            id: vbar
            height: pianoKeys.height
        }
        Rectangle {
            width: parent.width
            height: 2
            color: Colors.border
            anchors.top: vbar.bottom
        }
        Rectangle {
            width: parent.width
            height: expressionSelectorPlaceholder.height
            color: Colors.controlBackground
            anchors.bottom: parent.bottom
        }
    }
    Rectangle {
        id: bottomLeft
        width: pianoKeysPlaceholder.width
        Layout.preferredHeight: hbar.height
        color: Colors.controlBackground
    }
    Rectangle {
        id: hbarPlaceholder
        Layout.fillWidth: true
        Layout.preferredHeight: hbar.height
        color: Colors.controlBackground
        ScrollBar {
            id: hbar
            orientation: Qt.Horizontal
            width: parent.width
            visible: size < 1.0
        }
    }
    Button {
        id: barOptions
        Layout.fillWidth: false
        Layout.preferredWidth: vbarPlaceholder.width
        Layout.preferredHeight: hbar.height
        border.width: 0
        text: "\u25bc"
    }
}
