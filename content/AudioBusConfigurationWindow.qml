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
    width: column.width + padding * 2
    height: column.height + padding * 2
    Column {
        id: column
        anchors.centerIn: parent
        Row {
            id: tabButtons
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
            width: stackLayout.width + border.width * 2
            height: stackLayout.height + border.width * 2
            border.color: Colors.controlBorder
            StackLayout {
                id: stackLayout
                anchors.centerIn: parent
                currentIndex: inputButton.checked? 0: 1
                AudioBusConfiguration {
                    id: inputConfig
                    width: minimumWidth
                    height: minimumHeight
                }
                AudioBusConfiguration {
                    id: outputConfig
                    width: minimumWidth
                    height: minimumHeight
                }
            }
        }
    }
}
