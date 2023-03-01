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
                        leftPadding: height
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
                        FolderIcon {
                            readonly property double effectiveWidth: originalWidth * scale
                            readonly property double effectiveHeight: originalHeight * scale
                            x: y
                            y: (parent.height - effectiveHeight) / 2
                            transformOrigin: Item.TopLeft
                            scale: (parent.height - parent.topPadding - parent.bottomPadding) * 0.8 / originalHeight
                            path.strokeColor: "transparent"
                            path.fillColor: Colors.secondaryContent
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
                    model: ListModel {
                        ListElement {
                            name: "Plugins"
                            iconSource: "PluginIcon.qml"
                        }
                        ListElement {
                            name: "MIDI Effects"
                            iconSource: "MIDIEffectIcon.qml"
                        }
                        ListElement {
                            name: "Instruments"
                            iconSource: "PianoKeysIcon.qml"
                        }
                        ListElement {
                            name: "Audio Effects"
                            iconSource: "Audio.qml"
                        }
                    }

                    // model: ["Plugins", "MIDI Effects", "Instruments", "Audio Effects", "Audio Files", "MIDI Files"]
                    width: parent.width
                    height: contentHeight
                    delegate: ItemDelegate {
                        id: pluginItemDelegate
                        width: parent.width - 3 * 2
                        height: implicitHeight
                        x: root.border.width + 3
                        text: name
                        leftPadding: height
                        rightPadding: 2
                        topPadding: 2
                        bottomPadding: 2
                        background: Rectangle {
                            width: pluginItemDelegate.width
                            height: pluginItemDelegate.height
                            anchors.topMargin: root.topInset
                            anchors.bottomMargin: root.bottomInset
                            anchors.leftMargin: root.leftInset
                            anchors.rightMargin: root.rightInset
                            color: (!pluginItemDelegate.enabled)? Colors.background:
                                pluginItemDelegate.highlighted? Colors.highlightControlBackground:
                                pluginItemDelegate.hovered? Colors.mouseOverControlBackground: Colors.background
                        }
                        // Only the icon is smoothed; the text is not affected.
                        Item {
                            id: enableLayerForIcons
                            width: height
                            height: parent.height
                            layer.enabled: true
                            layer.smooth: true
                            layer.samples: 4
                            Loader {
                                id: loader
                                onLoaded: {
                                    item.transformOrigin = Item.TopLeft;
                                    item.scale = enableLayerForIcons.height * 0.625 / item.originalHeight;
                                    item.x = (enableLayerForIcons.height - item.originalHeight * item.scale) / 2;
                                    item.y = item.x;
                                    item.path.strokeColor = "transparent";
                                    item.path.fillColor = Colors.secondaryContent;
                                }
                            }
                        }
                        Component.onCompleted: {
                            loader.source = iconSource;
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
