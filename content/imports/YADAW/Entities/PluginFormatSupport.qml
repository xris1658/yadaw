import QtQml

QtObject {
    enum PluginFormat {
        VST3,
        CLAP,
        Vestifal
    }
    function isPluginFormatSupported(pluginFormat) {
        return true;
    }
}