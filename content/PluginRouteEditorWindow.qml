import QtQuick.Window

Window {
    id: root
    color: Colors.background
    width: 500
    height: 300
    property alias inputRouteList: pluginRouteEditor.inputRouteListModel
    property alias outputRouteList: pluginRouteEditor.outputRouteListModel
    property int padding: 10
    PluginRouteEditor {
        id: pluginRouteEditor
        width: root.width - root.padding * 2
        height: root.height - root.padding * 2
        x: root.padding
        y: root.padding
    }
}