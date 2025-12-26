import QtQml
import QtQuick
import QtQuick.Layouts

import YADAW.Models

Item {
    id: root

    property alias model: list.model
    property IAudioDeviceIOGroupListModel outputListModel: null
    property Window audioIOSelectorWindow: null

    QtObject {
        id: impl
        property bool usingPopup: false
        property ComboBoxButton usingPopupButton: null
        property int index: -1
        property bool adding: false
    }
    Connections {
        id: connectToAudioIOSelector
        target: impl.usingPopup? audioIOSelectorWindow: null
        function onAccepted() {
            if(impl.usingPopupButton) {
                impl.usingPopupButton.checked = false;
            }
            impl.usingPopup = false;
            impl.usingPopupButton = null;
            let target = model.data(
                model.index(list.currentIndex, 0),
                IAuxIOTargetListModel.Target
            );
            if(impl.adding) {
                target.append(
                    audioIOSelectorWindow.audioIOSelector.currentPosition
                );
            }
        }
        function onResetted() {
            if(impl.usingPopupButton) {
                impl.usingPopupButton.checked = false;
            }
            impl.usingPopup = false;
            impl.usingPopupButton = null;
            model.setData(
                model.index(impl.index, 0),
                null,
                IAuxIOTargetListModel.Target
            );
        }
        function onCancelled() {
            if(impl.usingPopupButton) {
                impl.usingPopupButton.checked = false;
            }
            impl.usingPopup = false;
            impl.usingPopupButton = null;
        }
    }
    SplitView {
        anchors.fill: parent
        handle: Item {
            implicitWidth: 2
            implicitHeight: 2
            Rectangle {
                width: 2
                height: parent.height
                anchors.right: parent.right
                color: Colors.controlBorder
            }
        }
        ListView {
            id: list
            SplitView.preferredWidth: 125
            height: root.height
            model: outputListModel
            clip: true
            delegate: ItemDelegate {
                text: (aiotlm_channel_index + 1) + ". " + outputListModel.data(
                    outputListModel.index(aiotlm_channel_index, 0),
                    IAudioDeviceIOGroupListModel.Name
                )
                width: list.width
                highlighted: list.currentIndex === index
                property var model: aiotlm_target
                onClicked: {
                    list.currentIndex = index;
                }
            }
        }
        Item {
            id: rightPanePlaceholder
            height: root.height
            Item {
                id: buttonsPlaceholder
                property int spacing: 5
                height: addButton.height + spacing * 2 + splitter.height
                width: parent.width
                Row {
                    anchors.left: parent.left
                    anchors.leftMargin: parent.spacing
                    anchors.top: parent.top
                    anchors.topMargin: parent.spacing
                    spacing: parent.spacing
                    Button {
                        id: addButton
                        text: qsTr("&Add...")
                        enabled: list.currentIndex != -1
                        onClicked: {
                            locatePopupWindow(audioIOSelectorWindow, addButton.height, 0);
                            audioIOSelectorWindow.audioIOSelector.showAudioHardwareInput = false;
                            audioIOSelectorWindow.audioIOSelector.showAudioHardwareOutput = true;
                            audioIOSelectorWindow.audioIOSelector.showAudioGroupChannel = true;
                            audioIOSelectorWindow.audioIOSelector.showAudioEffectChannel = true;
                            audioIOSelectorWindow.audioIOSelector.showPluginAuxIn = true;
                            audioIOSelectorWindow.audioIOSelector.showPluginAuxOut = false;
                            audioIOSelectorWindow.audioIOSelector.currentIndex = 0;
                            audioIOSelectorWindow.audioIOSelector.audioChannelConfig = outputListModel.data(
                                outputListModel.index(list.currentIndex, 0),
                                IAudioDeviceIOGroupListModel.ChannelConfig
                            );
                            audioIOSelectorWindow.audioIOSelector.showResetButton = false;
                            audioIOSelectorWindow.showNormal();
                            impl.usingPopup = true;
                            impl.adding = true;
                        }
                    }
                    Button {
                        id: locateButton
                        text: qsTr("&Locate")
                        enabled: list.currentIndex != -1
                    }
                    Button {
                        id: disconnectButton
                        text: qsTr("&Disconnect")
                        enabled: list.currentIndex != -1 && destList.currentIndex != -1
                        onClicked: {
                            let target = model.data(
                                model.index(list.currentIndex, 0),
                                IAuxIOTargetListModel.Target
                            );
                            target.remove(destList.index);
                        }
                    }
                }
                Button {
                    anchors.right: parent.right
                    anchors.rightMargin: parent.spacing
                    anchors.top: parent.top
                    anchors.topMargin: parent.spacing
                    id: disconnectAllButton
                    text: qsTr("Disconnect All")
                    enabled: destList.count != 0
                }
                Label {
                    anchors.right: disconnectAllButton.left
                    anchors.rightMargin: parent.spacing
                    anchors.verticalCenter: disconnectAllButton.verticalCenter
                    text: destList.count == 1? destList.count + qsTr(" target"):
                          destList.count > 1? destList.count + qsTr(" targets"):
                          qsTr("No target")
                }
                Rectangle {
                    id: splitter
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: Colors.border
                }
            }
            ListView {
                id: destList
                anchors.fill: parent
                anchors.topMargin: buttonsPlaceholder.height
                clip: true
                model: list.currentItem? list.currentItem.model: null
                delegate: ItemDelegate {
                    width: parent.width
                    text: aodlm_destination.completeName
                    highlighted: destList.currentIndex === index
                    onClicked: {
                        destList.currentIndex = index;
                    }
                }
            }
        }
    }
}