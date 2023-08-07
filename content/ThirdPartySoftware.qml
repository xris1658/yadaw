import QtQml
import QtQml.Models
import QtQuick
import QtQuick.Window

Window {
    id: root
    flags: Qt.Dialog
    title: qsTr("Third party software used by YADAW")
    modality: Qt.WindowModal
    color: Colors.background
    width: column.width + 20
    height: minimumHeight + 50
    minimumHeight: listView.contentHeight + noASIOSupportText.height + buttonArea.height + 20 + column.spacing * 2 + listRect.border.width * 2
    Column {
        id: column
        anchors.centerIn: parent
        spacing: 5
        Rectangle {
            id: listRect
            color: "transparent"
            border.color: Colors.controlBorder
            width: 570
            height: root.height - 20 - noASIOSupportText.height - buttonArea.height - 10
            clip: true
            ListView {
                id: listView
                anchors.fill: parent
                anchors.margins: parent.border.width
                model: ListModel {
                    ListElement {
                        name: "Qt"
                        website: "qt.io"
                        description: qsTr("Cross-platform application development kit")
                        license: "Commercial / LGPL v3"
                    }
                    ListElement {
                        name: "VST"
                        website: "https://www.steinberg.net/developers/"
                        description: qsTr("Audio Plugin API")
                        license: "Commercial / GPL v3"
                    }
                    ListElement {
                        name: "CLAP"
                        website: "https://github.com/free-audio/clap"
                        description: qsTr("Audio Plugin API")
                        license: "MIT"
                    }
                    ListElement {
                        name: "FFmpeg"
                        website: "http://ffmpeg.org/"
                        description: qsTr("Multimedia codec, resampler, muxer and more")
                        license: "GPL v2 / GPL v3 / LGPL v3"
                    }
                    ListElement {
                        name: "Avcpp"
                        website: "https://github.com/h4tr3d/avcpp"
                        description: qsTr("C++ wraper of FFmpeg")
                        license: "BSD-3-Clause / LGPL v2.1"
                    }
                    ListElement {
                        name: "SQLite"
                        website: "https://sqlite.org/index.html"
                        description: qsTr("Light-weight and fast SQL database engine")
                        license: "Public-domain"
                    }
                    ListElement {
                        name: "sqlite-modern-cpp"
                        website: "https://github.com/aminroosta/sqlite_modern_cpp"
                        description: qsTr("Modern C++ wrapper of SQLite")
                        license: "MIT"
                    }
                    ListElement {
                        name: "spdlog"
                        website: "https://github.com/gabime/spdlog"
                        description: qsTr("Logger")
                        license: "MIT"
                    }
                    ListElement {
                        name: "{fmt}"
                        website: "https://github.com/fmtlib/fmt"
                        description: qsTr("Format library which spdlog depends on")
                        license: "MIT"
                    }
                    ListElement {
                        name: "yaml-cpp"
                        website: "https://github.com/jbeder/yaml-cpp"
                        description: qsTr("YAML parser and emitter")
                        license: "MIT"
                    }
                    ListElement {
                        name: "Fira Sans"
                        website: "https://github.com/mozilla/Fira"
                        description: qsTr("A low-resolution friendly sans-serif typeface")
                        license: "SIL Open Font License"
                    }
                    ListElement {
                        name: qsTr("WenQuanYi Micro Hei")
                        website: "http://wenq.org/wqy2/index.cgi?MicroHei"
                        description: qsTr("Typeface used with Simplified Chinese localization")
                        license: "GPL v3"
                    }
                    ListElement {
                        name: "ADE"
                        website: "https://github.com/opencv/ade"
                        description: qsTr("Graph construction, manipulation, and processing")
                        license: "Apache-2.0"
                    }
                }
                delegate: Item {
                    width: listView.width
                    height: row.height + 4
                    Row {
                        id: row
                        anchors.centerIn: parent
                        Label {
                            id: label
                            leftPadding: 3
                            width: 135
                            text: "<a href=\"%1\">%2</a>".arg(website).arg(name)
                            ToolTip {
                                id: toolTip
                                text: qsTr("<p><b>Click to visit the following link: </b></p>%1").arg(website)
                            }
                            onLinkActivated: (link) => {
                                Qt.openUrlExternally(link);
                            }
                            onLinkHovered: (link) => {
                                if(link) {
                                    toolTip.open();
                                }
                                else {
                                    toolTip.close();
                                }
                            }
                        }
                        Label {
                            width: 285
                            text: description
                        }
                        Label {
                            width: 150
                            text: license
                        }
                    }
                }
            }
        }
        Label {
            id: noASIOSupportText
            wrapMode: Text.WordWrap
            text: qsTr("Due to incompatibility with several open-source licenses, ASIO support is not included.")
            width: listRect.width
        }
        Item {
            id: buttonArea
            width: listRect.width
            height: buttons.height
            Row {
                anchors.right: parent.right
                id: buttons
                Button {
                    id: buttonClose
                    text: qsTr("&Close")
                    onClicked: {
                        root.close();
                    }
                }
            }
        }
    }
    Component.onCompleted: {
        buttonClose.forceActiveFocus();
    }
}
