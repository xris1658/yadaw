import QtQuick
import QtQuick.Dialogs

Item {
    id: root

    property int firstColumnWidth
    property int secondColumnWidth
    property bool scanning: false

    property alias directoryListModel: pluginPathListView.model

    signal startPluginScan()
    signal scanPluginComplete()
    onScanPluginComplete: {
        scanning = false;
    }

    Grid {
        id: grid
        columns: 2
        columnSpacing: 10
        rowSpacing: 5
        verticalItemAlignment: Grid.AlignVCenter
        horizontalItemAlignment: Grid.AlignHCenter
        Label {
            id: label
            width: firstColumnWidth
            text: qsTr("Directories")
            color: Colors.secondaryContent
        }
        Item {
            width: secondColumnWidth
            height: 5
        }
        Item {
            width: firstColumnWidth
            height: pluginPathListViewBackground.height
        }
        Rectangle {
            id: pluginPathListViewBackground
            width: secondColumnWidth
            height: 100
            color: Colors.controlBackground
            border.color: Colors.controlBorder
            clip: true
            ListView {
                id: pluginPathListView
                anchors.fill: parent
                anchors.margins: parent.border.width
                clip: true
                boundsBehavior: ListView.StopAtBounds
                ScrollBar.vertical: ScrollBar {
                    visible: pluginPathListView.contentHeight > pluginPathListView.height
                }
                delegate: ItemDelegate {
                    text: pdlm_path
                    width: pluginPathListView.width
                    highlighted: pluginPathListView.currentIndex === index
                    onClicked: {
                        pluginPathListView.currentIndex = index;
                    }
                }
            }
        }
        Item {
            width: firstColumnWidth
            height: buttons.height
        }
        Item {
            id: buttons
            width: secondColumnWidth
            height: buttonAddPath.height
            Button {
                id: buttonAddPath
                anchors.left: parent.left
                text: qsTr("&Add...")
                enabled: (!root.scanning)
                FolderDialog {
                    id: folderDialog
                    onAccepted: {
                        directoryListModel.append(folderDialog.selectedFolder);
                    }
                }
                onClicked: {
                    folderDialog.open();
                }
            }
            Button {
                id: buttonRemovePath
                anchors.left: buttonAddPath.right
                anchors.leftMargin: leftPadding
                text: qsTr("&Remove")
                enabled: (!root.scanning) && pluginPathListView.currentIndex !== -1
                onClicked: {
                    let currentIndex = pluginPathListView.currentIndex;
                    if(pluginPathListView.currentIndex !== -1) {
                        directoryListModel.remove(currentIndex);
                        pluginPathListView.currentIndex = -1;
                    }
                }
            }
            Button {
                id: buttonScan
                anchors.right: parent.right
                text: qsTr("&Scan plugins")
                enabled: (!root.scanning) && (pluginPathListView.count !== 0)
                onClicked: {
                    root.scanning = true;
                    root.startPluginScan();
                }
            }
        }
        Label {
            width: firstColumnWidth
            height: buttonAddPath.height
            text: qsTr("Advanced")
            color: Colors.secondaryContent
        }
        Item {
            width: secondColumnWidth
            height: 5
        }
        Label {
            width: firstColumnWidth
            text: qsTr("Scan shortcuts")
            horizontalAlignment: Label.AlignRight
        }
        Item {
            width: secondColumnWidth
            height: scanShortcutSwitch.height
            Switch {
                id: scanShortcutSwitch
                checked: false
            }
        }
        Label {
            width: firstColumnWidth
            text: qsTr("Scan on startup")
            horizontalAlignment: Label.AlignRight
        }
        Item {
            width: secondColumnWidth
            height: scanOnStartupSwitch.height
            Switch {
                id: scanOnStartupSwitch
                checked: false
            }
        }
    }
    onScanningChanged: {
        if(scanning) {
            EventReceiver.mainWindow.pluginScanProgressWindow.visible = true;
            EventReceiver.mainWindow.pluginScanProgressWindow.showNormal();
        }
        else {
            EventReceiver.mainWindow.pluginScanProgressWindow.visible = false;
            EventReceiver.mainWindow.pluginScanProgressWindow.close();
        }
    }
}
