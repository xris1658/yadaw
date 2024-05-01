import QtQml
import QtQml.Models
import QtQuick
import QtQuick.Layouts

import YADAW.Entities

Item {
    id: root

    property int firstColumnWidth
    property int secondColumnWidth

    property int currentBackend: -1

    onCurrentBackendChanged: {
        audioEngineSelector.currentIndex = currentBackend;
    }

    property alias audioInputDeviceList: audioBusConfigurationWindow.inputDeviceListModel
    property alias audioOutputDeviceList: audioBusConfigurationWindow.outputDeviceListModel

    property alias audioGraphInputDeviceList: audioGraphSettings.inputDeviceList
    property alias audioGraphOutputDeviceList: audioGraphSettings.outputDeviceList
    property alias audioGraphOutputDeviceIndex: audioGraphSettings.outputDeviceIndex

    property alias alsaInputDeviceList: alsaSettings.inputDeviceList
    property alias alsaOutputDeviceList: alsaSettings.outputDeviceList

    property alias audioInputBusConfigurationModel: audioBusConfigurationWindow.inputConfigModel
    property alias audioOutputBusConfigurationModel: audioBusConfigurationWindow.outputConfigModel

    QtObject {
        id: impl
        readonly property AudioBackendSupport audioBackendSupport: AudioBackendSupport {}
    }

    MessageDialog {
        id: messageDialog
        visible: false
        icon: MessageDialog.Warning
        message: qsTr("The audio backend will be switched from <b>%1</b> to <b>%2</b>, so the audio capture and playback will be interrupted.\nProceed?")
            .arg(audioEngineSelector.model.get(currentBackend).name).arg(audioEngineSelector.currentText)
        title: "YADAW"
        buttonModel: ListModel {
            ListElement {
                buttonRole: DialogButtons.Yes
                buttonText: qsTr("&Switch")
            }
            ListElement {
                buttonRole: DialogButtons.No
                buttonText: qsTr("&Keep")
            }
        }
        onClicked: (buttonRole) => {
            if(buttonRole === DialogButtons.Yes) {
                currentBackend = audioEngineSelector.currentIndex;
            }
            else {
                audioEngineSelector.currentIndex = currentBackend;
            }
        }
    }

    Grid {
        id: grid
        columns: 2
        columnSpacing: 10
        rowSpacing: 5
        verticalItemAlignment: Grid.AlignVCenter
        horizontalItemAlignment: Grid.AlignHCenter
        Label {
            width: firstColumnWidth
            text: qsTr("Audio Engine")
            color: Colors.secondaryContent
        }
        Item {
            width: secondColumnWidth
            height: 5
        }
        Label {
            width: firstColumnWidth
            text: qsTr("Audio Backend")
            horizontalAlignment: Label.AlignRight
        }
        ComboBox {
            id: audioEngineSelector
            width: secondColumnWidth
            model: ListModel { dynamicRoles: true }
            textRole: "name"
            valueRole: "value"
            enabledRole: "enabled"
            onActivated: (index) => {
                if(currentBackend !== audioEngineSelector.currentIndex) {
                    messageDialog.show();
                }
            }
        }
        Item {
            visible: audioEngineSelector.currentValue !== AudioBackendSupport.Off
            width: firstColumnWidth
            height: configureAudioBusButton.height
        }
        Item {
            visible: audioEngineSelector.currentValue !== AudioBackendSupport.Off
            width: secondColumnWidth
            height: configureAudioBusButton.height
            Button {
                id: configureAudioBusButton
                anchors.right: parent.right
                text: qsTr("&Configure Audio Bus...")
                onClicked: {
                    audioBusConfigurationWindow.show();
                }
            }
        }
    }
    StackLayout {
        anchors.top: grid.bottom
        anchors.topMargin: grid.rowSpacing
        currentIndex: audioEngineSelector.currentIndex
        Item { /*Off*/ }
        AudioGraphSettings {
            id: audioGraphSettings
            firstColumnWidth: root.firstColumnWidth
            secondColumnWidth: root.secondColumnWidth
        }
        ALSASettings {
            id: alsaSettings
            firstColumnWidth: root.firstColumnWidth
            secondColumnWidth: root.secondColumnWidth
        }
        WASAPIExclusiveSettings {
            id: wasapiExclusiveSettings
            firstColumnWidth: root.firstColumnWidth
            secondColumnWidth: root.secondColumnWidth
        }
    }
    AudioBusConfigurationWindow {
        id: audioBusConfigurationWindow
    }
    Component.onCompleted: {
        audioEngineSelector.model.append(
            {
                "name": qsTr("Off"),
                "value": AudioBackendSupport.Off,
                "enabled": impl.audioBackendSupport.isBackendSupported(AudioBackendSupport.Off)
            }
        );
        audioEngineSelector.model.append(
            {
                "name": "AudioGraph",
                "value": AudioBackendSupport.AudioGraph,
                "enabled": impl.audioBackendSupport.isBackendSupported(AudioBackendSupport.AudioGraph)
            }
        );
        audioEngineSelector.model.append(
            {
                "name": "ALSA",
                "value": AudioBackendSupport.ALSA,
                "enabled": impl.audioBackendSupport.isBackendSupported(AudioBackendSupport.ALSA)
            }
        );
        audioEngineSelector.model.append(
            {
                "name": qsTr("WASAPI Exclusive"),
                "value": AudioBackendSupport.WASAPIExclusive,
                "enabled": impl.audioBackendSupport.isBackendSupported(AudioBackendSupport.WASAPIExclusive)
            }
        );
        audioEngineSelector.currentIndex = 0;
    }
}
