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

    property PluginSelector pluginSelector: null

    width: 120
    height: 400

    QtObject {
        id: impl
        readonly property int padding: 3
        readonly property int borderWidth: 1
    }
    ColumnLayout {
        width: root.width
        height: root.height
        spacing: 0
        Rectangle {
            id: ioPlaceholder
            color: "transparent"
            width: root.width
            Layout.preferredHeight: ioComboBoxColumn.height + impl.padding * 2
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
            Layout.preferredHeight: impl.borderWidth
            color: Colors.border
            visible: ioPlaceholder.visible
        }
        Rectangle {
            id: instrumentPlaceholder
            color: "transparent"
            width: root.width
            Layout.preferredHeight: instrumentButton.height + impl.padding * 2
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
            Layout.preferredHeight: impl.borderWidth
            color: Colors.border
            visible: instrumentPlaceholder.visible
        }
        Rectangle {
            id: insertPlaceholder
            color: "transparent"
            width: root.width
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 1
            visible: root.showInsertSlot
            clip: true
            Rectangle {
                id: insertHeader
                anchors.top: parent.top
                width: parent.width
                height: insertLabel.height + impl.padding
                color: Colors.border
                Label {
                    id: insertLabel
                    text: "INSERTS"
                    font.pointSize: Qt.application.font.pointSize * 0.8
                    anchors.centerIn: parent
                }
                Button {
                    id: bypassedButton
                    anchors.right: parent.right
                    width: height
                    height: parent.height
                    z: 2
                    property int contentDiameter: parent.height / 2
                    leftInset: (width - contentDiameter) / 2
                    rightInset: leftInset
                    topInset: (height - contentDiameter) / 2
                    bottomInset: topInset
                    background: Rectangle {
                        anchors.centerIn: parent
                        width: parent.contentDiameter
                        height: width
                        radius: width / 2
                        color: Colors.background
                        border.color: Colors.secondaryBackground
                    }
                }
            }
            ListView {
                id: insertList
                anchors.top: insertHeader.bottom
                anchors.topMargin: impl.padding
                anchors.horizontalCenter: parent.horizontalCenter
                width: insertPlaceholder.width - impl.padding * 2
                spacing: impl.padding
                delegate: MixerInsertSlot {
                    width: insertList.width
                }
                footer: Button {
                    width: insertList.width
                    onClicked: {
                        pluginSelector.parent = this;
                        pluginSelector.x = 0;
                        pluginSelector.y = height + impl.padding;
                        pluginSelector.open();
                    }
                }
            }
        }
        Rectangle {
            width: root.width
            Layout.preferredHeight: impl.borderWidth
            color: Colors.border
            visible: insertPlaceholder.visible
        }
        Rectangle {
            id: sendPlaceholder
            color: "transparent"
            width: root.width
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 1
            visible: root.showSendSlot
            clip: true
            Rectangle {
                id: sendHeader
                anchors.top: parent.top
                width: parent.width
                height: sendLabel.height + impl.padding
                color: Colors.border
                Label {
                    id: sendLabel
                    text: "SENDS"
                    font.pointSize: Qt.application.font.pointSize * 0.8
                    anchors.centerIn: parent
                }
                Button {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    width: height
                    height: sendLabel.height
                    anchors.rightMargin: (parent.height - height) / 2
                    radius: height / 2
                }
            }
            ListView {
                id: sendList
                anchors.top: sendHeader.bottom
                anchors.topMargin: impl.padding
                anchors.horizontalCenter: parent.horizontalCenter
                width: sendPlaceholder.width - impl.padding * 2
                spacing: impl.padding
                delegate: Button {
                    width: sendList.width
                }
                footer: Button {
                    width: sendList.width
                }
            }

            Column {
                anchors.top: sendHeader.bottom
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
            Layout.preferredHeight: impl.borderWidth
            color: Colors.border
            visible: sendPlaceholder.visible
        }
        Rectangle {
            id: controlButtonPlaceholder
            color: "transparent"
            width: root.width
            Layout.preferredHeight: muteButton.height + panningButton.height + impl.padding * 3
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
                Item {
                    id: panningButton
                    width: root.width - impl.padding * 2
                    height: panningText.contentHeight
                    Item {
                        anchors.left: parent.left
                        width: parent.width / 2
                        height: parent.height
                        Row {
                            anchors.centerIn: parent
                            spacing: 5
                            Dial {
                                id: panningDial
                                diameter: panningText.contentHeight
                                from: -100
                                to: 100
                            }
                            Label {
                                id: panningText
                                width: panningTextMax.contentWidth
                                horizontalAlignment: Label.AlignHCenter
                                text: Math.floor(panningDial.value)
                                Label {
                                    id: panningTextMax
                                    text: "-100"
                                    visible: false
                                }
                            }
                        }
                    }
                    Item {
                        anchors.right: parent.right
                        width: parent.width / 2
                        height: parent.height
                        Button {
                            anchors.centerIn: parent
                            height: panningButton.height
                            text: "MONO"
                            checkable: true
                        }
                    }
                }

            }
        }
        Rectangle {
            width: root.width
            Layout.preferredHeight: impl.borderWidth
            color: Colors.border
        }
        Rectangle {
            id: faderAndMeterPlaceholder
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 2
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
            Layout.preferredHeight: nameLabel.height + impl.padding * 2
            Layout.fillHeight: false
            color: Colors.controlBackground
            Label {
                id: nameLabel
                anchors.centerIn: parent
                width: parent.width - impl.padding * 2
                horizontalAlignment: Label.AlignHCenter
                text: "Channel"
            }
        }
    }
}
