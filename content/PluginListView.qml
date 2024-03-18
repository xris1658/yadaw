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
                Item {
                    width: height
                    height: itemDelegate.height
                    StackLayout {
                        id: formatIcons
                        anchors.centerIn: parent
                        currentIndex: plm_format
                        layer.enabled: true
                        layer.smooth: true
                        layer.textureSize: Qt.size(width * 2, height * 2)
                        PluginIcon {
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                            path.strokeColor: "transparent"
                        }
                        VST3Icon {
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                            path.strokeColor: "transparent"
                        }
                        CLAPIcon {
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                            path.strokeColor: "transparent"
                        }
                        VestifalIcon {
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                            path.strokeColor: "transparent"
                        }
                    }
                }
                Item {
                    width: height
                    height: itemDelegate.height
                    StackLayout {
                        id: typeIcons
                        anchors.centerIn: parent
                        currentIndex: plm_type
                        layer.enabled: true
                        layer.smooth: true
                        layer.textureSize: Qt.size(width * 2, height * 2)
                        PluginIcon {
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                            path.strokeColor: "transparent"
                        }
                        MIDIEffectIcon {
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                            path.strokeColor: "transparent"
                        }
                        PianoKeysIcon {
                            scale: 16 / originalHeight
                            path.fillColor: Colors.secondaryContent
                            path.strokeColor: "transparent"
                        }
                        AudioIcon {
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
