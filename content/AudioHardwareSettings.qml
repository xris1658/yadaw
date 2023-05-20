import QtQml
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    property int firstColumnWidth
    property int secondColumnWidth

    property alias audioGraphInputDeviceList: audioGraphSettings.inputDeviceList
    property alias audioGraphOutputDeviceList: audioGraphSettings.outputDeviceList
    property alias audioGraphOutputDeviceIndex: audioGraphSettings.outputDeviceIndex

    property alias alsaInputDeviceList: alsaSettings.inputDeviceList
    property alias alsaOutputDeviceList: alsaSettings.outputDeviceList

    property alias audioInputBusConfigurationModel: audioBusConfigurationWindow.inputConfigModel
    property alias audioOutputBusConfigurationModel: audioBusConfigurationWindow.outputConfigModel

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
            model: [qsTr("Dummy"), "AudioGraph", "ALSA"]
        }
        Item {
            width: firstColumnWidth
            height: configureAudioBusButton.height
        }
        Item {
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
        Item {}
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
        inputDeviceListModel: audioGraphInputDeviceList
        outputDeviceListModel: audioGraphOutputDeviceList
    }
}
