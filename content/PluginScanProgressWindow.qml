import QtQml
import QtQuick
import QtQuick.Window

Window {
    id: root

    property bool scanningDirectories: false
    property string currentFileName
    property int totalFileCount
    property int currentFileIndex

    flags: Qt.Dialog
    title: qsTr("Scanning Plugins...")
    modality: Qt.WindowModal
    color: Colors.background

    width: content.width + 20
    height: content.height + 20

    Column {
        id: content
        anchors.centerIn: parent
        spacing: 5
        Label {
            width: progressBar.width
            text: root.scanningDirectories?
                qsTr("Scanning library files..."):
                qsTr("Scanning file: %1").arg(root.currentFileName)
        }
        ProgressBar {
            id: progressBar
            indeterminate: root.scanningDirectories
            width: 300
            from: 0
            to: root.totalFileCount
            value: root.currentFileIndex
        }
        Label {
            width: progressBar.width
            visible: !root.scanningDirectories
            text: qsTr("%1 of %2").arg(root.currentFileIndex + 1).arg(totalFileCount)
            horizontalAlignment: Label.AlignHCenter
        }
    }
    onClosing: (accepted) => {
        hide();
        accepted = false;
    }
}