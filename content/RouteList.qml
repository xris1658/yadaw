import QtQml
import QtQuick
import QtQuick.Layouts

import YADAW.Models

Item {
    id: root
    height: 300

    property bool isInput: true
    property alias model: list.model
    property IAudioDeviceIOGroupListModel ioListModel: null
    property Window audioIOSelectorWindow: null
    property Window targetListWindow: null

    QtObject {
        id: impl
        property bool usingAudioIOSelector: false
        property ComboBoxButton usingAudioIOSelectorButton: null
        property int index: -1
    }
    Connections {
        id: connectToAudioIOSelector
        target: impl.usingAudioIOSelector? audioIOSelectorWindow: null
        function onAccepted() {
            impl.usingAudioIOSelectorButton.checked = false;
            impl.usingAudioIOSelector = false;
            impl.usingAudioIOSelectorButton = null;
            if(isInput) {
                model.setData(
                    model.index(impl.index, 0),
                    audioIOSelectorWindow.audioIOSelector.currentPosition,
                    IAuxIOTargetListModel.Target
                );
            }
            // TODO
        }
        function onResetted() {
            impl.usingAudioIOSelectorButton.checked = false;
            impl.usingAudioIOSelector = false;
            impl.usingAudioIOSelectorButton = null;
            if(isInput) {
                model.setData(
                    model.index(impl.index, 0),
                    null,
                    IAuxIOTargetListModel.Target
                );
            }
        }
        function onCancelled() {
            impl.usingAudioIOSelectorButton.checked = false;
            impl.usingAudioIOSelector = false;
            impl.usingAudioIOSelectorButton = null;
        }
    }
    ListView {
        id: list
        anchors.fill: parent
        spacing: 5
        topMargin: 5
        bottomMargin: 5
        clip: true
        delegate: Row {
            spacing: 3
            Label {
                text: (aiotlm_channel_index + 1) + ". " + ioListModel.data(
                    ioListModel.index(aiotlm_channel_index, 0),
                    IAudioDeviceIOGroupListModel.Name
                )
                verticalAlignment: Qt.AlignVCenter
                width: list.width - comboBoxButton.width - parent.spacing
                height: comboBoxButton.height
            }
            ComboBoxButton {
                id: comboBoxButton
                width: 100
                text: isInput? (aiotlm_target? aiotlm_target.completeName: undefined):
                    aiotlm_target.rowCount() > 1? qsTr("%1 targets").arg(aiotlm_target.rowCount()):
                    aiotlm_target.rowCount() == 1? qsTr("%1 target").arg(aiotlm_target.rowCount()): // TODO: show complete name of the only destination
                    undefined
                onCheckedChanged: {
                    if(checked) {
                        impl.index = index;
                        if(root.isInput) {
                            locatePopupWindow(audioIOSelectorWindow, comboBoxButton.height, 0);
                            audioIOSelectorWindow.audioIOSelector.showAudioHardwareInput = true;
                            audioIOSelectorWindow.audioIOSelector.showAudioHardwareOutput = false;
                            audioIOSelectorWindow.audioIOSelector.showAudioGroupChannel = false;
                            audioIOSelectorWindow.audioIOSelector.showAudioEffectChannel = false;
                            audioIOSelectorWindow.audioIOSelector.showPluginAuxIn = false;
                            audioIOSelectorWindow.audioIOSelector.showPluginAuxOut = true;
                            audioIOSelectorWindow.audioIOSelector.currentIndex = 0;
                            audioIOSelectorWindow.audioIOSelector.audioChannelConfig = ioListModel.data(
                                ioListModel.index(aiotlm_channel_index, 0),
                                IAudioDeviceIOGroupListModel.ChannelConfig
                            );
                            audioIOSelectorWindow.audioIOSelector.showResetButton = aiotlm_target? true: false;
                            audioIOSelectorWindow.showNormal();
                            impl.usingAudioIOSelector = true;
                            impl.usingAudioIOSelectorButton = comboBoxButton;
                        }
                        else { // TODO
                            locatePopupWindow(targetListWindow, comboBoxButton.height, 0);
                            targetListWindow.targetList.audioIOSelectorWindow = root.audioIOSelectorWindow;
                            targetListWindow.showNormal();
                        }
                    }
                }
            }
        }
    }
}