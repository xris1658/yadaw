import QtQml.Models
import QtQuick
import QtQuick.Layouts
import QtQuick.Window

Window {
    id: root

    property int padding: 10
    property alias inputConfigModel: inputConfig.model
    property alias outputConfigModel: outputConfig.model
    property alias inputDeviceListModel: inputConfig.deviceListModel
    property alias outputDeviceListModel: outputConfig.deviceListModel

    flags: Qt.Dialog
    title: qsTr("Configure Audio Bus")
    modality: Qt.WindowModal
    color: Colors.background
    width: 500
    height: 300
    Column {
        id: column
        anchors.centerIn: parent
        Row {
            id: tabButtons
            width: root.width - root.padding * 2
            TabButton {
                id: inputButton
                text: qsTr("Input")
                checked: true
            }
            TabButton {
                id: outputButton
                text: qsTr("Output")
            }
        }
        Rectangle {
            id: tabContent
            width: tabButtons.width
            height: root.height - tabButtons.height - root.padding * 2
            color: "transparent"
            border.color: Colors.border
            StackLayout {
                id: stackLayout
                anchors.fill: parent
                anchors.margins: parent.border.width
                currentIndex: inputButton.checked? 0: 1
                AudioBusConfiguration {
                    id: inputConfig
                    width: stackLayout.width
                    height: stackLayout.height
                }
                AudioBusConfiguration {
                    id: outputConfig
                    width: stackLayout.width
                    height: stackLayout.height
                }
            }
        }
    }
}
