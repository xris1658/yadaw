import QtQuick

SplitView {
    id: root

    property Window mainWindow: null

    property alias audioInputList: addTrackWindow.audioInputList
    property alias audioOutputList: addTrackWindow.audioOutputList
    property alias midiInputList: addTrackWindow.midiInputList
    property alias midiOutputList: addTrackWindow.midiOutputList
    property alias instrumentList: addTrackWindow.instrumentList
    property alias audioEffectList: addTrackWindow.audioEffectList

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
        ListView {
            id: trackHeaderListView
            anchors.top: topLeft.bottom
            anchors.bottom: bottomLeft.top
            footer: MouseArea {
                property int minimumHeight: 60
                width: arrangementHeader.width
                height: Math.max(trackHeaderListView.height - trackHeaderListView.contentHeight + minimumHeight, minimumHeight)
                acceptedButtons: Qt.RightButton
                Menu {
                    id: trackHeaderBlankOptions
                    title: qsTr("Track Header Blank Area Options")
                    Menu {
                        id: appendTrackMenu
                        title: qsTr("&Append Track")
                        function openAddTrackWindow(trackType: int) {
                            addTrackWindow.transientParent = root.mainWindow;
                            addTrackWindow.trackType = trackType;
                            addTrackWindow.showNormal();
                        }
                        MenuItem {
                            text: qsTr("&Audio") + "..."
                            onTriggered: {
                                appendTrackMenu.openAddTrackWindow(AddTrackWindow.TrackType.Audio);
                            }
                        }
                        MenuItem {
                            text: qsTr("&Instrument") + "..."
                            onTriggered: {
                                appendTrackMenu.openAddTrackWindow(AddTrackWindow.TrackType.Instrument);
                            }
                        }
                        MenuItem {
                            text: qsTr("&MIDI") + "..."
                            onTriggered: {
                                appendTrackMenu.openAddTrackWindow(AddTrackWindow.TrackType.MIDI);
                            }
                        }
                        MenuItem {
                            text: qsTr("Audio &Effect") + "..."
                            onTriggered: {
                                appendTrackMenu.openAddTrackWindow(AddTrackWindow.TrackType.AudioEffect);
                            }
                        }
                    }
                }
                onClicked: {
                    trackHeaderBlankOptions.x = mouseX;
                    trackHeaderBlankOptions.y = mouseY;
                    trackHeaderBlankOptions.open();
                }
            }
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
    AddTrackWindow {
        id: addTrackWindow
        modality: Qt.WindowModal
    }
}
