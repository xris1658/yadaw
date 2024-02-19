import QtQuick

SplitView {
    id: root

    property alias showAudioInputs: showAudioInputsItem.checked
    property alias showAudioOutputs: showAudioOutputsItem.checked

    property alias trackList: trackHeaderListView.model

    signal insertTrack(position: int, type: int) // AddTrackWindow.TrackType
    function appendTrack(type: int) { // AddTrackWindow.TrackType
        insertTrack(trackHeaderListView.count, type);
    }

    enum TimelineUnit {
        Beat,
        Second,
        Sample
    }

    enum TimelineLinear {
        BeatLinear,
        TimeLinear
    }

    property int timelineUnit: Arrangement.TimelineUnit.Beat
    property int timelineLinear: Arrangement.TimelineLinear.BeatLinear

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
            anchors.left: parent.left
            anchors.right: parent.right
            ScrollBar.vertical: ScrollBar {
                id: vbar
                parent: vbarPlaceholder
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                visible: size !== 1.0
            }
            delegate: Column {
                TrackHeader {
                    width: trackHeaderListView.width
                    height: 60
                    clip: true
                    name: mclm_name
                    trackColor: mclm_color
                    color: Colors.controlBackground
                }
                Rectangle {
                    width: trackHeaderListView.width
                    height: 1
                    color: Colors.border
                }
            }
            readonly property int footerHeight: 60
            footer: Item {
                width: arrangementHeader.width
                height: 60
            }
            MouseArea {
                id: trackHeaderListBlankArea
                anchors.top: trackHeaderListView.contentItem.bottom
                anchors.topMargin: -1 * trackHeaderListView.footerHeight
                height: Math.max(trackHeaderListView.footerHeight,
                    trackHeaderListView.height - (trackHeaderListView.contentHeight - trackHeaderListView.contentY) + trackHeaderListView.footerHeight
                )
                anchors.left: parent.left
                anchors.right: parent.right
                acceptedButtons: Qt.RightButton
                Menu {
                    id: trackHeaderBlankOptions
                    title: qsTr("Track Header Blank Area Options")
                    Menu {
                        id: appendTrackMenu
                        title: qsTr("&Append Track")
                        MenuItem {
                            text: qsTr("&Audio") + "..."
                            onTriggered: {
                                root.appendTrack(AddTrackWindow.TrackType.Audio);
                            }
                        }
                        MenuItem {
                            text: qsTr("&Instrument") + "..."
                            onTriggered: {
                                root.appendTrack(AddTrackWindow.TrackType.Instrument);
                            }
                        }
                        MenuItem {
                            text: qsTr("&MIDI") + "..."
                            onTriggered: {
                                root.appendTrack(AddTrackWindow.TrackType.MIDI);
                            }
                        }
                        MenuItem {
                            text: qsTr("Audio &Effect") + "..."
                            onTriggered: {
                                root.appendTrack(AddTrackWindow.TrackType.AudioEffect);
                            }
                        }
                    }
                    MenuItem {
                        id: showAudioInputsItem
                        text: qsTr("Show Audio &Inputs")
                        checkable: true
                        checked: false
                    }
                    MenuItem {
                        id: showAudioOutputsItem
                        text: qsTr("Show Audio &Outputs")
                        checkable: true
                        checked: false
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
        SplitView.minimumWidth: vbarPlaceholder.width
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
                                showIndicatorAsRadio: true
                                checked: root.timelineUnit === Arrangement.TimelineUnit.Beat
                                onTriggered: {
                                    root.timelineUnit = Arrangement.TimelineUnit.Beat;
                                }
                            }
                            MenuItem {
                                text: qsTr("&Second")
                                showIndicatorAsRadio: true
                                checked: root.timelineUnit === Arrangement.TimelineUnit.Second
                                onTriggered: {
                                    root.timelineUnit = Arrangement.TimelineUnit.Second;
                                }
                            }
                            MenuItem {
                                text: qsTr("S&ample")
                                showIndicatorAsRadio: true
                                checked: root.timelineUnit === Arrangement.TimelineUnit.Sample
                                onTriggered: {
                                    root.timelineUnit = Arrangement.TimelineUnit.Sample;
                                }
                            }
                            MenuSeparator {}
                            MenuItem {
                                text: qsTr("B&eat linear")
                                showIndicatorAsRadio: true
                                checked: root.timelineLinear === Arrangement.TimelineLinear.BeatLinear
                                onTriggered: {
                                    root.timelineLinear = Arrangement.TimelineLinear.BeatLinear;
                                }
                            }
                            MenuItem {
                                text: qsTr("T&ime linear")
                                showIndicatorAsRadio: true
                                checked: root.timelineLinear === Arrangement.TimelineLinear.TimeLinear
                                onTriggered: {
                                    root.timelineLinear = Arrangement.TimelineLinear.TimeLinear;
                                }
                            }
                        }
                        onClicked: {
                            timelineOptions.open();
                        }
                    }
                }
                Rectangle {
                    id: vbarPlaceholder
                    width: vbar.width
                    height: scrollBarAndOptions.height - timeline.height - hbar.height
                    color: Colors.controlBackground
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
