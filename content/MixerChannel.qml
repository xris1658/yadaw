import QtQml
import QtQuick
import QtQuick.Dialogs
import QtQuick.Layouts

Rectangle {
    id: root
    color: Colors.background
    property alias channelColor: infoPlaceholder.color
    property bool inputAvailable: true
    property bool outputAvailable: true
    property bool hasInstrument: true
    property alias inputModel: inputButton.model
    property alias outputModel: outputButton.model
    property alias name: nameLabel.text
    property bool showIO: true
    property bool showInstrumentSlot: true
    property bool showInsertSlot: true
    property bool showSendSlot: true
    property bool showFader: true

    width: 120
    height: 400

    QtObject {
        id: impl
        readonly property int padding: 3
        readonly property int borderWidth: 1
    }
    Column {
        Rectangle {
            id: ioPlaceholder
            color: "transparent"
            width: root.width
            height: ioComboBoxColumn.height + impl.padding * 2
            visible: root.showIO
            Column {
                id: ioComboBoxColumn
                spacing: impl.padding
                anchors.centerIn: parent
                ComboBox {
                    id: inputButton
                    opacity: root.inputAvailable? 1: 0
                    width: ioPlaceholder.width - impl.padding * 2
                    valueRole: "abcm_name"
                    textRole: "abcm_name"
                    displayText: count == 0? qsTr("No Bus Available"): currentIndex == -1? qsTr("Select Bus..."): currentText == ""? qsTr("Bus") + " " + (currentIndex + 1): currentText
                    indicator.visible: count != 0
                    enabled: count != 0
                }
                ComboBox {
                    id: outputButton
                    opacity: root.outputAvailable? 1: 0
                    width: ioPlaceholder.width - impl.padding * 2
                    valueRole: "abcm_name"
                    textRole: "abcm_name"
                    displayText: count == 0? qsTr("No Bus Available"): currentIndex == -1? qsTr("Select Bus..."): currentText == ""? qsTr("Bus") + " " + (currentIndex + 1): currentText
                    indicator.visible: count != 0
                    enabled: count != 0
                }
            }
        }
        Rectangle {
            width: root.width
            height: impl.borderWidth
            color: Colors.border
            visible: ioPlaceholder.visible
        }
        Rectangle {
            id: instrumentPlaceholder
            color: "transparent"
            width: root.width
            height: instrumentButton.height + impl.padding * 2
            visible: root.showInstrumentSlot
            opacity: root.hasInstrument? 1: 0
            Button {
                id: instrumentButton
                visible: root.hasInstrument
                anchors.centerIn: parent
                width: parent.width - impl.padding * 2
            }
        }
        Rectangle {
            width: root.width
            height: impl.borderWidth
            color: Colors.border
            visible: instrumentPlaceholder.visible
        }
        Rectangle {
            id: insertPlaceholder
            color: "transparent"
            width: root.width
            height: 60
            visible: root.showInsertSlot
            clip: true
            Column {
                anchors.top: parent.top
                anchors.topMargin: impl.padding
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: impl.padding
                Repeater {
                    model: 1
                    Button {
                        width: insertPlaceholder.width - impl.padding * 2
                    }
                }
            }
        }
        Rectangle {
            width: root.width
            height: impl.borderWidth
            color: Colors.border
            visible: insertPlaceholder.visible
        }
        Rectangle {
            id: sendPlaceholder
            color: "transparent"
            width: root.width
            height: 60
            visible: root.showSendSlot
            clip: true
            Column {
                anchors.top: parent.top
                anchors.topMargin: impl.padding
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: impl.padding
                Repeater {
                    model: 1
                    Button {
                        width: sendPlaceholder.width - impl.padding * 2
                    }
                }
            }
        }
        Rectangle {
            width: root.width
            height: impl.borderWidth
            color: Colors.border
            visible: sendPlaceholder.visible
        }
        Rectangle {
            id: controlButtonPlaceholder
            color: "transparent"
            width: root.width
            height: muteButton.height + panningButton.height + impl.padding * 3
            Column {
                anchors.centerIn: parent
                spacing: impl.padding
                Row {
                    spacing: impl.padding
                    Button {
                        id: muteButton
                        text: "M"
                        width: (controlButtonPlaceholder.width - impl.padding * 5) / 4
                        topPadding: 1
                        bottomPadding: 1
                    }
                    Button {
                        id: soloButton
                        text: "S"
                        width: (controlButtonPlaceholder.width - impl.padding * 5) / 4
                        topPadding: 1
                        bottomPadding: 1
                    }
                    Button {
                        id: invertPolarityButton
                        text: "∅"
                        width: (controlButtonPlaceholder.width - impl.padding * 5) / 4
                        topPadding: 1
                        bottomPadding: 1
                    }
                    Button {
                        id: armRecordingButton
                        text: "R"
                        width: (controlButtonPlaceholder.width - impl.padding * 5) / 4
                        topPadding: 1
                        bottomPadding: 1
                    }
                }
                Button {
                    id: panningButton
                    width: root.width - impl.padding * 2
                }
            }
        }
        Rectangle {
            width: root.width
            height: impl.borderWidth
            color: Colors.border
        }
        Rectangle {
            id: faderAndMeterPlaceholder
            height: root.height - (ioPlaceholder.visible? ioPlaceholder.height + impl.borderWidth: 0)
                    - (instrumentPlaceholder.visible? instrumentPlaceholder.height + impl.borderWidth: 0)
                    - (insertPlaceholder.visible? insertPlaceholder.height + impl.borderWidth: 0)
                    - (sendPlaceholder.visible? sendPlaceholder.height + impl.borderWidth: 0)
                    - controlButtonPlaceholder.height - infoPlaceholder.height
            visible: root.showFader
            color: "transparent"
            width: root.width
            clip: true
            Grid {
                rows: 2
                columns: 2
                rowSpacing: impl.padding
                Item {
                    id: faderPlaceholder
                    width: faderAndMeterPlaceholder.width / 2
                    height: faderAndMeterPlaceholder.height - volumeLabel.height - parent.rowSpacing
                    Slider {
                        orientation: Qt.Vertical
                        height: parent.height - impl.padding
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
                Item {
                    id: meterPlaceholder
                    width: faderAndMeterPlaceholder.width / 2
                    height: faderAndMeterPlaceholder.height - volumeLabel.height - parent.rowSpacing
                    Rectangle {
                        color: "#003300"
                        width: 20
                        height: parent.height - impl.padding
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
                Label {
                    id: volumeLabel
                    text: "-6.00"
                    width: faderAndMeterPlaceholder.width / 2
                    topPadding: 0
                    bottomPadding: impl.padding
                    horizontalAlignment: Label.AlignHCenter
                    verticalAlignment: Label.AlignVCenter
                }
                Label {
                    id: levelLabel
                    text: "-6.00"
                    width: faderAndMeterPlaceholder.width / 2
                    topPadding: 0
                    bottomPadding: impl.padding
                    horizontalAlignment: Label.AlignHCenter
                    verticalAlignment: Label.AlignVCenter
                }
            }
        }
        Rectangle {
            id: infoPlaceholder
            width: root.width
            height: nameLabel.height + impl.padding * 4
            Label {
                id: nameLabel
                anchors.centerIn: parent
                width: parent.width - impl.padding * 2
                horizontalAlignment: Label.AlignHCenter
            }
        }
    }
}
