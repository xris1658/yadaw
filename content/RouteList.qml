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

    QtObject {
        id: impl
        property bool usingAudioIOSelector: false
        property ComboBoxButton usingAudioIOSelectorButton: null
    }
    Connections {
        id: connectToAudioIOSelector
        target: impl.usingAudioIOSelector? audioIOSelectorWindow: null
        function onAccepted() {
            impl.usingAudioIOSelectorButton.checked = false;
            impl.usingAudioIOSelector = false;
            impl.usingAudioIOSelectorButton = null;
        }
        function onResetted() {
            impl.usingAudioIOSelectorButton.checked = false;
            impl.usingAudioIOSelector = false;
            impl.usingAudioIOSelectorButton = null;
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
                onCheckedChanged: {
                    if(checked) {
                        locatePopupWindow(audioIOSelectorWindow, comboBoxButton.height, 0);
                        if(root.isInput) {
                            audioIOSelectorWindow.audioIOSelector.showAudioHardwareInput = true;
                            audioIOSelectorWindow.audioIOSelector.showAudioHardwareOutput = false;
                            audioIOSelectorWindow.audioIOSelector.showAudioGroupChannel = false;
                            audioIOSelectorWindow.audioIOSelector.showAudioEffectChannel = false;
                            audioIOSelectorWindow.audioIOSelector.showPluginAuxIn = false;
                            audioIOSelectorWindow.audioIOSelector.showPluginAuxOut = true;
                        }
                        else {
                            audioIOSelectorWindow.audioIOSelector.showAudioHardwareInput = false;
                            audioIOSelectorWindow.audioIOSelector.showAudioHardwareOutput = true;
                            audioIOSelectorWindow.audioIOSelector.showAudioGroupChannel = true;
                            audioIOSelectorWindow.audioIOSelector.showAudioEffectChannel = true;
                            audioIOSelectorWindow.audioIOSelector.showPluginAuxIn = true;
                            audioIOSelectorWindow.audioIOSelector.showPluginAuxOut = false;
                        }
                        audioIOSelectorWindow.audioIOSelector.currentIndex = 0;
                        audioIOSelectorWindow.audioIOSelector.audioChannelConfig = ioListModel.data(
                            ioListModel.index(aiotlm_channel_index, 0),
                            IAudioDeviceIOGroupListModel.ChannelConfig
                        );
                        if(root.isInput) { // TODO
                            audioIOSelectorWindow.audioIOSelector.showResetButton = aiotlm_target? true: false;
                        }
                        audioIOSelectorWindow.showNormal();
                        impl.usingAudioIOSelector = true;
                        impl.usingAudioIOSelectorButton = comboBoxButton;
                    }
                }
            }
        }
    }
}