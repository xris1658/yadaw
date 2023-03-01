import QtQuick

Rectangle {
    id: root
    color: Colors.background
    border.color: Colors.controlBorder

    readonly property int preferredWidth: searchTextBox.height * 15

    clip: true

    TextField {
        id: searchTextBox
        anchors.top: parent.top
        width: parent.width
        height: implicitHeight + 1 // align bottom width the arrangement view
        placeholderText: "<i>" + qsTr("Search (Ctrl+F)") + "</i>"
        Button {
            id: clearButton
            visible: parent.length !== 0
            anchors.right: parent.right
            anchors.rightMargin: 1
            anchors.verticalCenter: parent.verticalCenter
            width: height
            height: parent.height - anchors.rightMargin * 2
            text: "\u232b"
            padding: 1
            background: Rectangle {
                anchors.fill: parent
                color: (!clearButton.enabled)? "transparent":
                    clearButton.down? Colors.pressedControlBackground:
                    clearButton.hovered? Colors.mouseOverControlBackground:
                    "transparent"
            }
            onClicked: {
                parent.clear();
            }
        }
    }
    SplitView {
        anchors.top: searchTextBox.bottom
        anchors.topMargin: -1
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        orientation: Qt.Horizontal
        handle: Item {
            implicitWidth: 5
            implicitHeight: 5
            Rectangle {
                width: 1
                height: parent.height
                anchors.centerIn: parent
                color: Colors.controlBorder
            }
        }
        Rectangle {
            SplitView.preferredWidth: 50 // FIXME
            color: "transparent"
            clip: true
            Column {
                topPadding: 5
                bottomPadding: 5
                width: parent.width
                Label {
                    text: qsTr("Locations")
                    color: Colors.secondaryContent
                    font.pointSize: Qt.application.font.pointSize * 0.9
                    width: parent.width
                    height: contentHeight + topPadding + bottomPadding
                    leftPadding: 5
                    rightPadding: leftPadding
                    topPadding: 3
                    bottomPadding: 0
                    elide: Text.ElideRight
                }
                ListView {
                    model: ["Assets", "My Samples"]
                    width: parent.width
                    height: contentHeight
                    delegate: ItemDelegate {
                        id: control
                        width: parent.width - 3 * 2
                        x: root.border.width + 3
                        text: modelData
                        rightPadding: 2
                        topPadding: 2
                        bottomPadding: 2
                        background: Rectangle {
                            width: control.width
                            height: control.height
                            anchors.topMargin: root.topInset
                            anchors.bottomMargin: root.bottomInset
                            anchors.leftMargin: root.leftInset
                            anchors.rightMargin: root.rightInset
                            color: (!control.enabled)? Colors.background:
                                control.highlighted? Colors.highlightControlBackground:
                                control.hovered? Colors.mouseOverControlBackground: Colors.background
                        }
                    }
                    footer: ItemDelegate {
                        id: addLocation
                        width: parent.width - 3 * 2
                        x: root.border.width + 3
                        text: "Add..."
                        rightPadding: 2
                        background: Rectangle {
                            width: addLocation.width
                            height: addLocation.height
                            anchors.topMargin: root.topInset
                            anchors.bottomMargin: root.bottomInset
                            anchors.leftMargin: root.leftInset
                            anchors.rightMargin: root.rightInset
                            color: (!addLocation.enabled)? Colors.background:
                                addLocation.highlighted? Colors.highlightControlBackground:
                                addLocation.hovered? Colors.mouseOverControlBackground: Colors.background
                        }
                    }
                }
                Label {
                    text: qsTr("Categories")
                    color: Colors.secondaryContent
                    font.pointSize: Qt.application.font.pointSize * 0.9
                    width: parent.width
                    height: contentHeight + topPadding + bottomPadding
                    leftPadding: 5
                    rightPadding: leftPadding
                    topPadding: 3
                    bottomPadding: 0
                    elide: Text.ElideRight
                }
                ListView {
                    model: ["Plugins", "MIDI Effects", "Instruments", "Audio Effects"]
                    width: parent.width
                    height: contentHeight
                    delegate: ItemDelegate {
                        id: control
                        width: parent.width - 3 * 2
                        x: root.border.width + 3
                        text: modelData
                        rightPadding: 2
                        topPadding: 2
                        bottomPadding: 2
                        background: Rectangle {
                            width: control.width
                            height: control.height
                            anchors.topMargin: root.topInset
                            anchors.bottomMargin: root.bottomInset
                            anchors.leftMargin: root.leftInset
                            anchors.rightMargin: root.rightInset
                            color: (!control.enabled)? Colors.background:
                                control.highlighted? Colors.highlightControlBackground:
                                control.hovered? Colors.mouseOverControlBackground: Colors.background
                        }
                    }
                }
            }
        }
        Rectangle {
            color: "transparent"
        }
    }
}
