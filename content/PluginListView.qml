import QtQuick
import QtQuick.Layouts

ListView {
    id: root
    enum PluginFormat {
        Unknown,
        VST3,
        CLAP,
        Vestifal
    }
    enum PluginType {
        Unknown,
        MIDIEffect,
        Instrument,
        AudioEffect
    }
    reuseItems: true
    boundsBehavior: ListView.StopAtBounds
    ScrollBar.vertical: ScrollBar {
        visible: root.contentHeight > root.height
    }
    delegate: ItemDelegate {
        id: itemDelegate
        width: root.width
        height: label.contentHeight + label.topPadding + label.bottomPadding
        Row {
            spacing: 2
            Row {
                StackLayout {
                    id: formatIcons
                    currentIndex: plm_format
                    Item {
                        layer.enabled: true
                        layer.smooth: true
                        layer.textureSize: Qt.size(width * 2, height * 2)
                        width: height
                        height: itemDelegate.height
                        PluginIcon {
                            anchors.centerIn: parent
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                            path.strokeColor: "transparent"
                        }
                    }
                    Item {
                        layer.enabled: true
                        layer.smooth: true
                        layer.textureSize: Qt.size(width * 2, height * 2)
                        width: height
                        height: itemDelegate.height
                        VST3Icon {
                            anchors.centerIn: parent
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                            path.strokeColor: "transparent"
                        }
                    }
                    Item {
                        layer.enabled: true
                        layer.smooth: true
                        layer.samples: 4
                        width: height
                        height: itemDelegate.height
                        CLAPIcon {
                            anchors.centerIn: parent
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                            path.strokeColor: "transparent"
                        }
                    }
                    Item {
                        layer.enabled: true
                        layer.smooth: true
                        layer.samples: 4
                        width: height
                        height: itemDelegate.height
                        VestifalIcon {
                            anchors.centerIn: parent
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                            path.strokeColor: "transparent"
                        }
                    }
                }
                StackLayout {
                    id: typeIcons
                    currentIndex: plm_type
                    Item {
                        layer.enabled: true
                        layer.smooth: true
                        layer.samples: 4
                        width: height
                        height: itemDelegate.height
                        PluginIcon {
                            anchors.centerIn: parent
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                            path.strokeColor: "transparent"
                        }
                    }
                    Item {
                        layer.enabled: true
                        layer.smooth: true
                        layer.samples: 4
                        width: height
                        height: itemDelegate.height
                        MIDIEffectIcon {
                            anchors.centerIn: parent
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                            path.strokeColor: "transparent"
                        }
                    }
                    Item {
                        layer.enabled: true
                        layer.smooth: true
                        layer.samples: 4
                        width: height
                        height: itemDelegate.height
                        PianoKeysIcon {
                            anchors.centerIn: parent
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                            path.strokeColor: "transparent"
                        }
                    }
                    Item {
                        layer.enabled: true
                        layer.smooth: true
                        layer.samples: 4
                        width: height
                        height: itemDelegate.height
                        AudioIcon {
                            anchors.centerIn: parent
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                            path.strokeColor: "transparent"
                        }
                    }
                }
            }
            Label {
                id: label
                topPadding: 2
                bottomPadding: 2
                text: plm_name
            }
        }
    }
}
