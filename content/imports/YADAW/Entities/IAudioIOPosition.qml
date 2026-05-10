import QtQuick

QtObject {
    enum Type
    {
        Invalid,
        AudioHardwareIOChannel,
        BusAndFXChannelInput,
        AudioChannelInput,
        RegularChannelOut,
        PluginAuxIO,
        Send
    }
    property int type
    property string name
    property string completeName
}