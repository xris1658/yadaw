import QtQuick
import QtQuick.Controls as QC

Window {
    width: root.width
    height: root.height
    QC.Popup {
        id: root

        QtObject {
            id: impl
            property int contentWidth: 360
        }

        visible: true

        topInset: 0
        bottomInset: 0
        leftInset: 0
        rightInset: 0
        padding: 10

        background: Rectangle {
            anchors.fill: parent
            anchors.topMargin: root.topInset
            anchors.bottomMargin: root.bottomInset
            anchors.leftMargin: root.leftInset
            anchors.rightMargin: root.rightInset
            color: Colors.background
            border.color: Colors.border
        }

        contentItem: Column {
            spacing: 5
            TextField {
                placeholderText: "<i>" + qsTr("Search (Ctrl+F)") + "</i>"
                width: impl.contentWidth
            }
            Rectangle {
                width: impl.contentWidth
                height: 300
                color: "transparent"
                border.color: Colors.border
                SplitView {
                    anchors.fill: parent
                    anchors.margins: parent.border.width
                    orientation: Qt.Horizontal
                    handle: Item {
                        implicitWidth: 5
                        implicitHeight: 5
                        Rectangle {
                            width: 1
                            height: parent.height
                            anchors.right: parent.right
                            color: Colors.controlBorder
                        }
                    }
                    Item {
                        SplitView.preferredWidth: 100
                        Column {
                            topPadding: 5
                            bottomPadding: 5
                            width: parent.width
                            Label {
                                text: qsTr("Format")
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
                                id: formatList
                                model: ListModel {
                                    ListElement {
                                        name: "Vestifal"
                                        iconSource: "VestifalIcon.qml"
                                    }
                                    ListElement {
                                        name: "VST3"
                                        iconSource: "VST3Icon.qml"
                                    }
                                    ListElement {
                                        name: "CLAP"
                                        iconSource: "CLAPIcon.qml"
                                    }
                                }
                                width: parent.width
                                height: contentHeight
                                delegate: ItemDelegate {
                                    id: pluginItemDelegate
                                    width: parent.width - 3 * 2
                                    height: implicitHeight
                                    text: name
                                    leftPadding: height
                                    rightPadding: 2
                                    topPadding: 2
                                    bottomPadding: 2
                                    highlighted: formatList.currentIndex === index
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
                                                item.parent = enableLayerForIcons;
                                                item.scale = 16 / item.originalHeight;
                                                item.anchors.centerIn = enableLayerForIcons;
                                                item.path.strokeColor = "transparent";
                                                item.path.fillColor = Colors.secondaryContent;
                                            }
                                        }
                                    }
                                    Component.onCompleted: {
                                        loader.source = iconSource;
                                    }
                                    onClicked: {
                                        formatList.currentIndex = index;
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
                                id: categoryList
                                model: ["EQ / Filter", "Dynamics", "Gain / Panning", "Phaser / Flanger", "Overdrive / Distortion"]
                                width: parent.width
                                height: contentHeight
                                delegate: ItemDelegate {
                                    id: categoryItemDelegate
                                    width: parent.width - 3 * 2
                                    height: implicitHeight
                                    text: modelData
                                    leftPadding: height
                                    rightPadding: 2
                                    topPadding: 2
                                    bottomPadding: 2
                                    background: Rectangle {
                                        width: categoryItemDelegate.width
                                        height: categoryItemDelegate.height
                                        anchors.topMargin: root.topInset
                                        anchors.bottomMargin: root.bottomInset
                                        anchors.leftMargin: root.leftInset
                                        anchors.rightMargin: root.rightInset
                                        color: (!categoryItemDelegate.enabled)? Colors.background:
                                            categoryItemDelegate.highlighted? Colors.highlightControlBackground:
                                            categoryItemDelegate.hovered? Colors.mouseOverControlBackground: Colors.background
                                    }
                                    // Only the icon is smoothed; the text is not affected.
                                    Item {
                                        id: enableLayerForIcons2
                                        width: height
                                        height: parent.height
                                        layer.enabled: true
                                        layer.smooth: true
                                        layer.samples: 4
                                        TagIcon {
                                            scale: 16 / originalHeight
                                            anchors.centerIn: parent
                                            path.strokeColor: "transparent"
                                            path.fillColor: Colors.secondaryContent;
                                            path.fillRule: ShapePath.OddEvenFill
                                        }
                                    }
                                    onClicked: {
                                        formatList.currentIndex = index;
                                    }
                                }
                            }
                        }
                    }
                    Item {
                        id: rightContainer
                        Column {
                            Rectangle {
                                id: header
                                color: Colors.controlBackground
                                width: rightContainer.width
                                height: 20
                            }
                            Item {
                                width: rightContainer.width
                                height: rightContainer.height - header.height
                                ListView {
                                    id: listView
                                    anchors.fill: parent
                                    anchors.rightMargin: vbar.width
                                    anchors.bottomMargin: hbar.height
                                    ScrollBar.vertical: vbar
                                    ScrollBar.horizontal: hbar
                                }
                                ScrollBar {
                                    id: vbar
                                    anchors.right: parent.right
                                    height: listView.height
                                    orientation: Qt.Vertical
                                }
                                ScrollBar {
                                    id: hbar
                                    anchors.bottom: parent.bottom
                                    width: listView.width
                                    orientation: Qt.Horizontal
                                }
                            }
                        }
                    }
                }
            }
            Item {
                width: impl.contentWidth
                height: okButton.height
                Row {
                    anchors.right: parent.right
                    spacing: 5
                    Button {
                        id: okButton
                        text: Constants.okTextWithMnemonic
                    }
                    Button {
                        id: cancelButton
                        text: Constants.cancelTextWithMnemonic
                    }
                }
            }
        }
    }
}
