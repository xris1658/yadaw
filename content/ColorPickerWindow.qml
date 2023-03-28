import QtQml
import QtQuick
import QtQuick.Window

Window {
    id: root
    flags: Qt.Dialog
    color: Colors.background
    width: colorPicker.width + 2 * padding
    height: column.height + width - colorPicker.width

    property int padding: 20
    property color originalColor
    property alias currentColor: colorPicker.color

    signal accepted()

    Column {
        id: column
        anchors.centerIn: parent
        spacing: 10
        ColorPicker {
            id: colorPicker
            radius: 128
            spacing: column.spacing
        }
        Item {
            width: colorPicker.radius * 2
            height: okButton.height
            Row {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                Label {
                    text: qsTr("Old")
                    height: originalColorRectangle.height
                    verticalAlignment: Label.AlignVCenter
                    rightPadding: 5
                }
                Rectangle {
                    id: originalColorRectangle
                    width: okButton.width
                    height: width
                    color: root.originalColor
                }
                Rectangle {
                    id: currentColorRectangle
                    width: okButton.width
                    height: width
                    color: root.currentColor
                }
                Label {
                    text: qsTr("New")
                    height: currentColorRectangle.height
                    verticalAlignment: Label.AlignVCenter
                    leftPadding: 5
                }
            }

            Row {
                anchors.right: parent.right
                spacing: 5
                Button {
                    id: okButton
                    text: qsTr("&OK")
                    onClicked: {
                        root.close();
                        root.accepted();
                    }
                }
                Button {
                    id: cancelButton
                    text: qsTr("&Cancel")
                }
            }
        }
    }
}
