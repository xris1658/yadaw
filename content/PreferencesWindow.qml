import QtQuick
import QtQuick.Layouts
import QtQuick.Window

Window {
    id: root
    title: qsTr("Preferences")
    flags: Qt.Dialog
    modality: Qt.NonModal

    property int firstColumnWidth: 125
    property int secondColumnWidth: 275

    width: 540
    height: 500
    minimumWidth: 540
    SplitView {
        id: content
        anchors.fill: parent
        orientation: Qt.Horizontal
        z: 2
        ListView {
            id: listView
            property int minWidth: 0
            SplitView.minimumWidth: minWidth
            SplitView.maximumWidth: root.width - firstColumnWidth - secondColumnWidth - 35
            model: ["General", "Audio Hardware", "Plugins"]
            boundsBehavior: ListView.StopAtBounds
            delegate: ItemDelegate {
                id: itemDelegate
                text: modelData
                width: listView.width
                highlighted: listView.currentIndex == index
                Component.onCompleted: {
                    listView.minWidth = Math.max(listView.minWidth, itemDelegate.implicitWidth);
                }
                onClicked: {
                    listView.currentIndex = index;
                }
            }
        }
        Item {
            StackLayout {
                anchors.fill: parent
                anchors.margins: 10
                currentIndex: listView.currentIndex
                GeneralSettings {
                    firstColumnWidth: root.firstColumnWidth
                    secondColumnWidth: root.secondColumnWidth
                }
                AudioHardwareSettings {
                    firstColumnWidth: root.firstColumnWidth
                    secondColumnWidth: root.secondColumnWidth
                }
                PluginSettings {
                    firstColumnWidth: root.firstColumnWidth
                    secondColumnWidth: root.secondColumnWidth
                }
            }
        }
        Keys.onEscapePressed: {
            close();
        }
    }
    Rectangle {
        width: listView.width
        height: listView.height
        z: 1
        color: Colors.controlBackground
    }
    Component.onCompleted: {
        content.forceActiveFocus();
    }
    onClosing: {
        EventSender.darkModeSupportWindow = root;
        EventSender.removeWindowForDarkModeSupport();
    }
}
