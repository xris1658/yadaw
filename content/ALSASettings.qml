import QtQml
import QtQuick

Rectangle {
    id: root

    property int firstColumnWidth
    property int secondColumnWidth

    property alias inputDeviceList: inputDeviceListView.model
    property alias outputDeviceList: outputDeviceListView.model
    property alias sampleRate: sampleRateComboBox.currentValue

    property int bufferSize
    property int latency

    Grid {
        columns: 2
        columnSpacing: 10
        rowSpacing: 5
        verticalItemAlignment: Grid.AlignVCenter
        horizontalItemAlignment: Grid.AlignHCenter
        Label {
            width: firstColumnWidth
            text: qsTr("I/O")
            color: Colors.secondaryContent
        }
        Item {
            width: secondColumnWidth
            height: 5
        }
        Label {
            width: firstColumnWidth
            text: qsTr("Sample Rate")
            horizontalAlignment: Label.AlignRight
        }
        Item {
            width: secondColumnWidth
            height: sampleRateComboBox.height
            ComboBox {
                id: sampleRateComboBox
                width: parent.width - hertzLabel.width
                model: [44100, 48000, 88200, 96000, 176400, 192000]
            }
            Label {
                id: hertzLabel
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                leftPadding: 5
                text: "Hz"
            }
        }
        Item {
            width: firstColumnWidth
            height: inputDeviceListViewBackground.height
            Label {
                width: firstColumnWidth
                text: qsTr("Input Device")
                horizontalAlignment: Label.AlignRight
                topPadding: 3
            }
        }
        Rectangle {
            id: inputDeviceListViewBackground
            width: secondColumnWidth
            height: Math.max(noInputDeviceFoundLabel.height,
                Math.min(inputDeviceListView.contentHeight + border.width * 2, noInputDeviceFoundLabel.height * 5 + border.width * 2))
            color: noInputDeviceFoundLabel.visible? Colors.background: Colors.controlBackground
            border.color: noInputDeviceFoundLabel.visible? Colors.disabledControlBorder: Colors.controlBorder
            clip: true
            Label {
                id: noInputDeviceFoundLabel
                padding: 3
                visible: inputDeviceListView.count === 0
                anchors.centerIn: parent
                text: qsTr("No input device found")
            }
            ListView {
                id: inputDeviceListView
                anchors.fill: parent
                anchors.margins: parent.border.width
                clip: true
                boundsBehavior: ListView.StopAtBounds
                ScrollBar.vertical: ScrollBar {
                    id: inputDeviceListScrollBar
                    visible: policy !== ScrollBar.AlwaysOff && inputDeviceListView.contentHeight > inputDeviceListView.height
                    background: Item {}
                }
                delegate: CheckBox {
                    padding: 2
                    text: adlm_name
                    width: inputDeviceListView.width - (inputDeviceListScrollBar.visible? inputDeviceListScrollBar.width: 0)
                    checked: adlm_enabled
                    onCheckedChanged: {
                        adlm_enabled = checked;
                    }
                }
            }
        }
        Label {
            width: firstColumnWidth
            text: qsTr("Output Device")
            horizontalAlignment: Label.AlignRight
        }
        Rectangle {
            id: outputDeviceListViewBackground
            width: secondColumnWidth
            height: Math.max(noOutputDeviceFoundLabel.height,
                Math.min(outputDeviceListView.contentHeight + border.width * 2, noOutputDeviceFoundLabel.height * 5 + border.width * 2))
            color: noOutputDeviceFoundLabel.visible? Colors.background: Colors.controlBackground
            border.color: noOutputDeviceFoundLabel.visible? Colors.disabledControlBorder: Colors.controlBorder
            clip: true
            Label {
                id: noOutputDeviceFoundLabel
                padding: 3
                visible: outputDeviceListView.count === 0
                anchors.centerIn: parent
                text: qsTr("No output device found")
            }
            ListView {
                id: outputDeviceListView
                anchors.fill: parent
                anchors.margins: parent.border.width
                clip: true
                boundsBehavior: ListView.StopAtBounds
                ScrollBar.vertical: ScrollBar {
                    id: outputDeviceListScrollBar
                    visible: policy !== ScrollBar.AlwaysOff && outputDeviceListView.contentHeight > outputDeviceListView.height
                    background: Item {}
                }
                delegate: CheckBox {
                    padding: 2
                    text: adlm_name
                    width: outputDeviceListView.width - (outputDeviceListScrollBar.visible? outputDeviceListScrollBar.width: 0)
                    checked: adlm_enabled
                    onCheckedChanged: {
                        adlm_enabled = checked;
                    }
                }
            }
        }
        Label {
            width: firstColumnWidth
            text: qsTr("Status")
            color: Colors.secondaryContent
        }
        Item {
            width: secondColumnWidth
            height: 5
        }
        Label {
            width: firstColumnWidth
            text: qsTr("Buffer Size") + ": "
            horizontalAlignment: Label.AlignRight
        }
        Label {
            width: secondColumnWidth
            text: qsTr("%1 sample(s) (%2 ms)").arg(root.bufferSize).arg(root.bufferSize * 1000 / parseFloat(sampleRateComboBox.currentValue))
        }
        Label {
            width: firstColumnWidth
            text: qsTr("Latency") + ": "
            horizontalAlignment: Label.AlignRight
        }
        Label {
            width: secondColumnWidth
            text: qsTr("%1 sample(s) (%2 ms)").arg(root.latency).arg(root.latency * 1000 / parseFloat(sampleRateComboBox.currentValue))
        }
    }
}
