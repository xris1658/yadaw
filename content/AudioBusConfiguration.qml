import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts

Rectangle {
    id: root
    color: Colors.background
    property int minimumWidth: 400
    property int minimumHeight: 300
    property var deviceListModel: ListModel {}
    property alias model: busList.model

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
                        TextField {
                            id: channelCountTextField
                            width: height * 5
                            placeholderText: qsTr("Channel count")
                            onAccepted: {
                                okAddBusButton.clicked();
                            }
                        }
                        Button {
                            id: okAddBusButton
                            text: Constants.okTextWithMnemonic
                            onClicked: {
                                let value = Functions.parseStringAsInt(channelCountTextField.text);
                                if(!isNaN(value) && value > 0) {
                                    root.model.append(value);
                                    popup.close();
                                }
                                else {
                                    channelCountTextField.selectAll();
                                    channelCountTextField.forceActiveFocus();
                                }
                            }
                        }
                    }
                    onOpened: {
                        channelCountTextField.clear();
                        channelCountTextField.forceActiveFocus();
                    }
                }
                onClicked: {
                    popup.open();
                }
            }
        }
        ListView {
            id: busList
            width: root.width / 2
            height: root.height - busListHeader.height
            clip: true
            boundsBehavior: ListView.StopAtBounds
            ScrollBar.vertical: ScrollBar {
                id: busListScrollBar
                visible: busList.height < busList.contentHeight
            }
            delegate: Row {
                id: audioBusRow
                property int busIndex: index
                 function renameAudioBus() {
                    audioBusNameTextField.visible = true;
                    audioBusNameTextField.text = audioBusButton.text;
                    audioBusNameTextField.forceActiveFocus();
                    audioBusNameTextField.selectAll();
                }
                ItemDelegate {
                    id: audioBusButton
                    width: busList.width - (busListScrollBar.visible? busListScrollBar.width: 0) - audioBusRenameButton.width - audioBusRemoveButton.width
                    text: abcm_name == ""? qsTr("Bus") + " " + (index + 1): abcm_name
                    z: 2
                    highlighted: stackLayout.currentIndex === audioBusRow.busIndex
                    TextField {
                        id: audioBusNameTextField
                        visible: false
                        width: audioBusRow.width
                        height: parent.height
                        onAccepted: {
                            visible = false;
                            abcm_name = text;
                        }
                        Keys.onEscapePressed:  {
                            audioBusNameTextField.visible = false;
                        }
                    }
                    onClicked: {
                        stackLayout.currentIndex = audioBusRow.busIndex
                    }
                }
                ItemDelegate {
                    id: audioBusRenameButton
                    width: height
                    z: 1
                    RenameIcon {
                        anchors.centerIn: parent
                        path.fillColor: Colors.secondaryContent
                        path.strokeColor: "transparent"
                    }
                    onClicked: {
                        audioBusRow.renameAudioBus();
                    }
                }
                ItemDelegate {
                    id: audioBusRemoveButton
                    width: height
                    z: 1
                    RemoveIcon {
                        anchors.centerIn: parent
                        scale: parent.height / originalHeight
                        path.fillColor: Colors.secondaryContent
                        path.strokeColor: "transparent"
                    }
                    onClicked: {
                        root.model.remove(index);
                        if(stackLayout.currentIndex == index) {
                            stackLayout.currentIndex = -1;
                        }
                    }
                }
                ListView.onAdd: {
                    stackLayout.currentIndex = index;
                    renameAudioBus();
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
        width: root.minimumWidth - busList.width - separator.width
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
                        property int channelCount: model.data(model.index(currentIndex, 0), Qt.UserRole + 3)
                        model: deviceListModel
                        textRole: "agdlm_name"
                        valueRole: "agdlm_id"
                        enabledRole: "agdlm_enabled"
                        currentIndex: abclm_device_index
                        hideDisabledItem: true
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
