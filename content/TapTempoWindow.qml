import QtQml
import QtQuick
import QtQuick.Window

Window {
    id: root
    flags: Qt.Dialog
    title: qsTr("Tap Tempo")
    modality: Qt.WindowModal
    color: Colors.background

    width: contents.width + contents.spacing * 2
    height: contents.height + contents.spacing * 2

    Column {
        id: contents
        anchors.centerIn: parent
        spacing: 5
        Button {
            id: tapButton
            width: 160
            height: 80
        }
        Label {
            width: tapButton.width
            font.bold: true
            font.family: "Fira Sans Condensed"
            font.pointSize: Qt.application.font.pointSize * 2
            text: "128.000"
            horizontalAlignment: Label.AlignHCenter
        }
        Item {
            width: tapButton.width
            height: buttons.height
            Row {
                id: buttons
                anchors.centerIn: parent
                spacing: 5
                Button {
                    text: qsTr("&Round")
                }
                Button {
                    text: qsTr("&Apply")
                }
            }
        }
    }
    Component.onCompleted: {
        tapButton.forceActiveFocus();
    }
}
