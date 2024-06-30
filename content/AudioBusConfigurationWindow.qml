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
        TabBar {
            id: tabButtons
            TabButton {
                id: inputButton
                width: implicitWidth
                text: qsTr("Input")
                checked: true
            }
            TabButton {
                id: outputButton
                width: implicitWidth
                text: qsTr("Output")
            }
        }
        Rectangle {
            id: tabContent
            width: root.width - root.padding * 2
            height: root.height - tabButtons.height - root.padding * 2
            color: "transparent"
            border.color: Colors.border
            StackLayout {
                id: stackLayout
                anchors.fill: parent
                anchors.margins: parent.border.width
                currentIndex: tabButtons.currentIndex
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
    Shortcut {
        context: Qt.WindowShortcut
        sequence: "Ctrl+Tab"
        onActivated: {
            let index = tabButtons.currentIndex === tabButtons.count - 1?
                0: tabButtons.currentIndex + 1;
            tabButtons.currentIndex = index;
        }
    }
    Shortcut {
        context: Qt.WindowShortcut
        sequence: "Ctrl+Shift+Tab"
        onActivated: {
            let index = tabButtons.currentIndex === 0?
                tabButtons.count - 1: tabButtons.currentIndex - 1;
            tabButtons.currentIndex = index;
        }
    }
    Shortcut {
        context: Qt.WindowShortcut
        sequence: "Esc"
        enabled: !(inputConfig.editingText || outputConfig.editingText)
        onActivated: {
            root.close();
        }
    }
}
