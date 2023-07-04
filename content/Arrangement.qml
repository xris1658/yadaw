import QtQuick

SplitView {
    id: root

    handle: Item {
        implicitWidth: 2
        implicitHeight: 2
        Rectangle {
            width: 2
            height: parent.height
            anchors.right: parent.right
            color: Colors.controlBorder
        }
    }
    Item {
        id: arrangementHeader
        SplitView.minimumWidth: 100
        Rectangle {
            id: topLeft
            color: Colors.controlBackground
            width: parent.width
            height: timeline.height
        }
        Rectangle {
            id: bottomLeft
            anchors.bottom: parent.bottom
            color: Colors.controlBackground
            width: parent.width
            height: hbar.height
        }
    }
    Item {
        id: arrangementContent
        Item {
            id: scrollBarAndOptions
            anchors.right: parent.right
            width: vbar.width
            height: parent.height
            Column {
                Item {
                    width: scrollBarAndOptions.width
                    height: timeline.height
                    Button {
                        anchors.fill: parent
                        text: "\u25bc"
                        border.width: 0
                        Menu {
                            id: timelineOptions
                            x: parent.width - width
                            y: parent.height
                            title: qsTr("Timeline Options")
                            MenuItem {
                                text: qsTr("&Beat")
                                checkable: true
                                showIndicatorAsRadio: true
                            }
                            MenuItem {
                                text: qsTr("&Second")
                                checkable: true
                                showIndicatorAsRadio: true
                            }
                            MenuItem {
                                text: qsTr("S&ample")
                                checkable: true
                                showIndicatorAsRadio: true
                            }
                            MenuSeparator {}
                            MenuItem {
                                text: qsTr("B&eat linear")
                                checkable: true
                                showIndicatorAsRadio: true
                            }
                            MenuItem {
                                text: qsTr("T&ime linear")
                                checkable: true
                                showIndicatorAsRadio: true
                            }
                        }
                        onClicked: {
                            timelineOptions.open();
                        }
                    }
                }
                ScrollBar {
                    id: vbar
                    height: scrollBarAndOptions.height - timeline.height - hbar.height
                    orientation: Qt.Vertical
                }
                Item {
                    width: scrollBarAndOptions.width
                    height: hbar.height
                    Button {
                        anchors.fill: parent
                        text: "\u25bc"
                        border.width: 0
                        Menu {
                            id: contentAreaZoomOptions
                            x: parent.width - width
                            y: parent.height
                            title: qsTr("Content Area Zoom Options")
                            MenuItem {
                                text: qsTr("Fit &width")
                            }
                            MenuItem {
                                text: qsTr("Fit &height")
                            }
                        }
                        onClicked: {
                            contentAreaZoomOptions.open();
                        }
                    }
                }
            }
        }
        Item {
            id: timelineAndContentArea
            width: arrangementContent.width - scrollBarAndOptions.width
            height: arrangementContent.height
            Item {
                id: timelineArea
                width: parent.width
                height: timeline.height
                Timeline {
                    id: timeline
                    width: timelineArea.width
                }
            }
            ScrollBar {
                id: hbar
                anchors.bottom: parent.bottom
                orientation: Qt.Horizontal
                width: timelineAndContentArea.width
            }
            Item {
                id: contentArea
                width: parent.width
                anchors.top: timelineArea.bottom
                anchors.bottom: hbar.top
            }
        }
    }
}
