import QtQuick
import QtQuick.Layouts

ListView {
    id: root
    enum PluginFormat {
        Unknown,
        VST3,
        CLAP
    }
    enum PluginType {
        Unknown,
        MIDIEffect,
        Instrument,
        AudioEffect
    }
    boundsBehavior: ListView.StopAtBounds
    ScrollBar.vertical: ScrollBar {
        visible: root.contentHeight > root.height
    }
    delegate: ItemDelegate {
        id: itemDelegate
        property string name: plm_name
        property int format: plm_format
        property int type: plm_type
        width: root.width
        height: label.contentHeight + label.topPadding + label.bottomPadding
        Row {
            spacing: 2
            Row {
                StackLayout {
                    id: formatIcons
                    currentIndex: itemDelegate.format
                    Item {
                        layer.enabled: true
                        layer.samples: 4
                        width: height
                        height: itemDelegate.height
                        PluginIcon {
                            anchors.centerIn: parent
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                        }
                    }
                    Item {
                        layer.enabled: true
                        layer.samples: 4
                        width: height
                        height: itemDelegate.height
                        VST3Icon {
                            anchors.centerIn: parent
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                        }
                    }
                    Item {
                        layer.enabled: true
                        layer.samples: 4
                        width: height
                        height: itemDelegate.height
                        CLAPIcon {
                            anchors.centerIn: parent
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                        }
                    }
                }
                StackLayout {
                    id: typeIcons
                    currentIndex: itemDelegate.type
                    Item {
                        layer.enabled: true
                        layer.samples: 4
                        width: height
                        height: itemDelegate.height
                        PluginIcon {
                            anchors.centerIn: parent
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                        }
                    }
                    Item {
                        layer.enabled: true
                        layer.samples: 4
                        width: height
                        height: itemDelegate.height
                        MIDIEffectIcon {
                            anchors.centerIn: parent
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                        }
                    }
                    Item {
                        layer.enabled: true
                        layer.samples: 4
                        width: height
                        height: itemDelegate.height
                        PianoKeysIcon {
                            anchors.centerIn: parent
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                        }
                    }
                    Item {
                        layer.enabled: true
                        layer.samples: 4
                        width: height
                        height: itemDelegate.height
                        AudioIcon {
                            anchors.centerIn: parent
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                        }
                    }
                }
            }
            Label {
                id: label
                topPadding: 2
                bottomPadding: 2
                text: itemDelegate.name
            }
        }
    }
}
