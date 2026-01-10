import QtQml
import QtQuick
import QtQuick.Layouts

import YADAW.Models

Item {
    id: root

    property alias model: list.model
    property IAudioDeviceIOGroupListModel inputListModel: null
    property Window audioIOSelectorWindow: null

    QtObject {
        id: impl
        property bool usingPopup: false
        property ComboBoxButton usingPopupButton: null
        property int index: -1
    }
    Connections {
        id: connectToAudioIOSelector
        target: impl.usingPopup? audioIOSelectorWindow: null
        function onAccepted() {
            impl.usingPopupButton.checked = false;
            impl.usingPopup = false;
            impl.usingPopupButton = null;
            model.setData(
                model.index(impl.index, 0),
                audioIOSelectorWindow.audioIOSelector.currentPosition,
                IAuxIOTargetListModel.Target
            );
        }
        function onResetted() {
            impl.usingPopupButton.checked = false;
            impl.usingPopup = false;
            impl.usingPopupButton = null;
            model.setData(
                model.index(impl.index, 0),
                null,
                IAuxIOTargetListModel.Target
            );
        }
        function onCancelled() {
            impl.usingPopupButton.checked = false;
            impl.usingPopup = false;
            impl.usingPopupButton = null;
        }
    }
    ListView {
        id: list
        anchors.fill: parent
        spacing: 5
        property int margin: 5
        topMargin: margin
        bottomMargin: margin
        leftMargin: margin
        rightMargin: margin
        clip: true
        delegate: Row {
            spacing: 3
            Label {
                text: (aiotlm_channel_index + 1) + ". " + inputListModel.data(
                    inputListModel.index(aiotlm_channel_index, 0),
                    IAudioDeviceIOGroupListModel.Name
                )
                verticalAlignment: Qt.AlignVCenter
                width: list.width - comboBoxButton.width - parent.spacing - list.margin * 2
                height: comboBoxButton.height
            }
            ComboBoxButton {
                id: comboBoxButton
                width: 100
                text: aiotlm_target? aiotlm_target.completeName: undefined
                onCheckedChanged: {
                    if(checked) {
                        impl.index = index;
                        locatePopupWindow(audioIOSelectorWindow, comboBoxButton.height, 0);
                        audioIOSelectorWindow.audioIOSelector.showAudioHardwareInput = true;
                        audioIOSelectorWindow.audioIOSelector.showAudioHardwareOutput = false;
                        audioIOSelectorWindow.audioIOSelector.showAudioChannel = false;
                        audioIOSelectorWindow.audioIOSelector.showAudioGroupChannel = false;
                        audioIOSelectorWindow.audioIOSelector.showAudioEffectChannel = false;
                        audioIOSelectorWindow.audioIOSelector.showPluginAuxIn = false;
                        audioIOSelectorWindow.audioIOSelector.showPluginAuxOut = true;
                        audioIOSelectorWindow.audioIOSelector.currentIndex = 0;
                        audioIOSelectorWindow.audioIOSelector.audioChannelConfig = inputListModel.data(
                            inputListModel.index(aiotlm_channel_index, 0),
                            IAudioDeviceIOGroupListModel.ChannelConfig
                        );
                        audioIOSelectorWindow.audioIOSelector.showResetButton = aiotlm_target? true: false;
                        audioIOSelectorWindow.showNormal();
                        impl.usingPopup = true;
                        impl.usingPopupButton = comboBoxButton;
                    }
                }
            }
        }
    }
}