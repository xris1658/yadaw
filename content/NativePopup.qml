import QtQuick

Window {
    id: root
    flags: Qt.Tool | Qt.FramelessWindowHint
    visible: false
    signal mousePressedOutside()
}