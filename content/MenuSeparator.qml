import QtQuick
import QtQuick.Templates as T

T.MenuSeparator {
    id: root
    implicitHeight: 5
    background: Rectangle {
        color: Colors.controlBackground
    }
    Rectangle {
        color: Colors.controlBorder
        width: parent.width
        height: 1
        anchors.centerIn: parent
    }
}
