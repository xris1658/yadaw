import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts

import YADAW.Models

Rectangle {
    id: root
    color: Colors.background
    property var deviceListModel: ListModel {}
    property alias model: busList.model
    readonly property alias editingText: impl.editingText

    QtObject {
        id: impl
        // Create a property that is read-only from outside
        property bool editingText: false
    }

    Column {
        id: busListColumn
        Item {
            id: busListHeader
            width: busList.width
            height: addBusButton.height + addBusButton.topPadding + addBusButton.bottomPadding
            Label {
                height: addBusButton.height + addBusButton.topPadding + addBusButton.bottomPadding
                verticalAlignment: Label.AlignVCenter
                text: busList.count.toString() + (busList.count <= 1? qsTr(" audio bus"): qsTr(" audio buses"))
                leftPadding: addBusButton.rightPadding
            }
            Button {
                id: addBusButton
                anchors.right: parent.right
                anchors.rightMargin: rightPadding
                anchors.top: parent.top
                anchors.topMargin: topPadding
                text: qsTr("&Add Bus...")
                QC.Popup {
                    property int position: 0
                    id: popup
                    x: (parent.width - width) / 2
                    y: parent.height + parent.bottomPadding
                    width: popupRow.width + popupRow.spacing * 2
                    height: popupRow.height + popupRow.spacing * 2
                    background: Rectangle {
                        color: Colors.background
                        border.width: 1
                        border.color: Colors.controlBorder
                    }
                    Row {
                        id: popupRow
                        anchors.centerIn: parent
                        spacing: 3
                        Label {
                            text: qsTr("Channel Config") + ":"
                            height: channelConfigComboBox.height
                            verticalAlignment: Label.AlignVCenter
                        }
                        ComboBox {
                            id: channelConfigComboBox
                            model: Constants.channelConfigProperties
                            textRole: "name"
                            valueRole: "type"
                        }
                        Button {
                            id: okAddBusButton
                            text: Constants.okTextWithMnemonic
                            onClicked: {
                                popup.close();
                                root.model.insert(popup.position, channelConfigComboBox.currentValue);
                            }
                        }
                    }
                    onOpened: {
                        channelConfigComboBox.forceActiveFocus();
                    }
                }
                onClicked: {
                    popup.position = busList.count;
                    popup.open();
                }
            }
        }
        ListView {
            id: busList
            width: root.width / 2
            height: root.height - busListHeader.height
            clip: true
            model: null
            boundsBehavior: ListView.StopAtBounds
            ScrollBar.vertical: ScrollBar {
                id: busListScrollBar
                visible: busList.height < busList.contentHeight
            }
            delegate: ItemDelegate {
                id: audioBusButton
                property int busIndex: index
                width: busList.width - (busListScrollBar.visible? busListScrollBar.width: 0)
                text: abcm_name == ""? qsTr("Bus") + " " + (index + 1): abcm_name
                z: 2
                highlighted: stackLayout.currentIndex === audioBusButton.busIndex
                TextField {
                    id: audioBusNameTextField
                    visible: false
                    width: audioBusButton.width
                    height: parent.height
                    onAccepted: {
                        visible = false;
                        abcm_name = text;
                    }
                    Keys.onEscapePressed:  {
                        audioBusNameTextField.visible = false;
                    }
                    onVisibleChanged: {
                        impl.editingText = visible;
                    }
                }
                Menu {
                    id: audioBusOptions
                    title: qsTr("Audio Bus Options")
                    MenuItem {
                        text: qsTr("&Insert Audio Bus Above")
                        onTriggered: {
                            popup.position = audioBusButton.busIndex;
                            popup.open();
                        }
                    }
                    MenuItem {
                        text: qsTr("Rena&me")
                        onTriggered: {
                            audioBusButton.renameAudioBus();
                        }
                    }
                    MenuItem {
                        text: qsTr("&Delete")
                        onTriggered: {
                            root.model.remove(audioBusButton.busIndex);
                            if(stackLayout.currentIndex == audioBusButton.busIndex) {
                                stackLayout.currentIndex = -1;
                            }
                        }
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    onClicked: (mouse) => {
                        audioBusOptions.parent = audioBusButton;
                        audioBusOptions.x = mouse.x;
                        audioBusOptions.y = mouse.y;
                        audioBusOptions.open();
                    }
                }
                function renameAudioBus() {
                    audioBusNameTextField.visible = true;
                    audioBusNameTextField.text = audioBusButton.text;
                    audioBusNameTextField.forceActiveFocus();
                    audioBusNameTextField.selectAll();
                }
                onClicked: {
                    stackLayout.currentIndex = audioBusButton.busIndex;
                }
                ListView.onAdd: {
                    stackLayout.currentIndex = audioBusButton.busIndex;
                    audioBusButton.renameAudioBus();
                }
            }
        }
    }
    Rectangle {
        id: separator
        width: 1
        height: root.height
        anchors.left: busListColumn.right
        color: Colors.controlBorder
    }
    StackLayout {
        id: stackLayout
        anchors.right: parent.right
        width: root.width - busList.width - separator.width
        height: root.height
        Repeater {
            model: busList.model
            ListView {
                id: channelList
                width: stackLayout.width
                model: abcm_channel_list
                clip: true
                property int busIndex: index
                boundsBehavior: ListView.StopAtBounds
                ScrollBar.vertical: ScrollBar {
                    id: channelListScrollBar
                    visible: channelList.height < channelList.contentHeight
                }
                delegate: Row {
                    id: channelRow
                    property int indexInBus: channelList.busIndex
                    Label {
                        id: indexIndicator
                        text: index + 1
                        width: channelSelector.width
                        height: channelSelector.height
                        verticalAlignment: Label.AlignVCenter
                        horizontalAlignment: Label.AlignLeft
                        leftPadding: (height - contentHeight) / 2
                    }
                    ComboBox {
                        id: deviceSelector
                        width: channelList.width - (channelListScrollBar.visible? channelListScrollBar.width: 0) - channelSelector.width - indexIndicator.width
                        model: deviceListModel
                        property int channelCount: model.data(model.index(currentIndex, 0), IAudioDeviceListModel.ChannelCount)
                        textRole: "adlm_name"
                        valueRole: "adlm_id"
                        enabledRole: "adlm_enabled"
                        displayText: currentIndex == -1? qsTr("Dummy Device"):
                            currentText
                        currentIndex: abclm_device_index
                        hideDisabledItem: true
                        onCurrentIndexChanged: {
                            abclm_device_index = currentIndex;
                        }
                    }
                    ComboBox {
                        id: channelSelector
                        width: height * 1.5
                        enabled: !(count == 0)
                        indicator.width: 0
                        indicator.visible: false
                        model: Functions.arrayFromOne(deviceSelector.channelCount)
                        displayText: enabled? model[currentIndex]: qsTr("N/A")
                        currentIndex: abclm_channel_index
                        onCurrentIndexChanged: {
                            abclm_channel_index = currentIndex;
                        }
                    }
                }
            }
        }
    }
}
