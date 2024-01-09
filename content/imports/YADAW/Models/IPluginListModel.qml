import QtQml.Models

ListModel {
    enum Role {
        Id,
        Path,
        Uid,
        Name,
        Vendor,
        Version,
        Format,
        Type,
        RoleCount
    }
    enum PluginFormat
    {
        UnknownFormat,
        VST3,
        CLAP,
        Vestifal
    }
    enum PluginType
    {
        UnknownType,
        MIDIEffect,
        Instrument,
        AudioEffect
    }
    dynamicRoles: true
    function getSortIndexOfRole(role: int) {
        return -1;
    }
}
