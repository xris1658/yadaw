import QtQuick

Window {
    id: root
    height: bar.height
    color: Colors.background
    PluginWindowTopBar {
        id: bar
        width: parent.width
    }
}