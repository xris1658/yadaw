import QtQuick

Window {
    id: root
    flags: Qt.Tool | Qt.FramelessWindowHint | Qt.WindowDoesNotAcceptFocus
    visible: false
    signal mousePressedOutside()
}