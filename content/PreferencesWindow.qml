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

    property alias systemFontRendering: generalSettings.systemFontRendering
    property alias pluginDirectoryListModel: pluginSettings.directoryListModel

    signal startPluginScan()
    signal pluginScanComplete()
    onPluginScanComplete: {
        pluginSettings.scanPluginComplete();
    }

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
            SplitView.minimumWidth: contentWidth
            SplitView.maximumWidth: contentWidth
            model: ["General", "Audio Hardware", "Plugins"]
            boundsBehavior: ListView.StopAtBounds
            delegate: ItemDelegate {
                id: itemDelegate
                text: modelData
                highlighted: listView.currentIndex == index
                Component.onCompleted: {
                    listView.contentWidth = Math.max(listView.contentWidth, itemDelegate.implicitWidth);
                    if(index === listView.model.length - 1) {
                        for(let i = 0; i < listView.model.length - 1; ++i) {
                            listView.itemAtIndex(i).width = listView.contentWidth;
                        }
                        width = listView.contentWidth;
                    }
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
                    id: generalSettings
                    firstColumnWidth: root.firstColumnWidth
                    secondColumnWidth: root.secondColumnWidth
                }
                AudioHardwareSettings {
                    firstColumnWidth: root.firstColumnWidth
                    secondColumnWidth: root.secondColumnWidth
                }
                PluginSettings {
                id: pluginSettings
                    firstColumnWidth: root.firstColumnWidth
                    secondColumnWidth: root.secondColumnWidth
                    onStartPluginScan: {
                        root.startPluginScan();
                    }
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
