import QtQml
import QtQml.Models
import QtQuick
import QtQuick.Layouts

import YADAW.Entities

Item {
    id: root

    property int firstColumnWidth
    property int secondColumnWidth

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
        audioEngineSelector.currentIndex = 0;
    }
}
