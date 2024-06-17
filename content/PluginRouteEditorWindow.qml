import QtQuick.Window

Window {
    id: pluginRouteEditorWindow
    color: Colors.background
    width: 500
    height: 300
    property alias inputRouteList: pluginRouteEditor.inputRouteListModel
    property alias outputRouteList: pluginRouteEditor.outputRouteListModel
    property Window audioIOSelectorWindow
    property int padding: 10
    PluginRouteEditor {
        id: pluginRouteEditor
        width: pluginRouteEditorWindow.width - pluginRouteEditorWindow.padding * 2
        height: pluginRouteEditorWindow.height - pluginRouteEditorWindow.padding * 2
        x: pluginRouteEditorWindow.padding
        y: pluginRouteEditorWindow.padding
        audioIOSelectorWindow: pluginRouteEditorWindow.audioIOSelectorWindow
    }
}