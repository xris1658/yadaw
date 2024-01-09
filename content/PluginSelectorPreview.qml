import QtQuick

import YADAW.Models

Window {
    id: root
    width: pluginSelector.width
    height: pluginSelector.height
    visible: true
    PluginSelector {
        id: pluginSelector
        visible: true
        pluginListModel: IPluginListModel {
            property string filterString
            Component.onCompleted: {
                for(let i = 0; i < 12; ++i) {
                    append(
                        {
                            "plm_name": "Plugin Name",
                            "plm_vendor": "Plugin Vendor",
                            "plm_version": "1.0.0.0",
                            "plm_format": IPluginListModel.Vestifal,
                            "plm_type": IPluginListModel.Instrument
                        }
                    );
                    append(
                        {
                            "plm_name": "Plugin Name",
                            "plm_vendor": "Plugin Vendor",
                            "plm_version": "1.0.0.0",
                            "plm_format": IPluginListModel.VST3,
                            "plm_type": IPluginListModel.AudioEffect
                        }
                    );
                    append(
                        {
                            "plm_name": "Plugin Name",
                            "plm_vendor": "Plugin Vendor",
                            "plm_version": "1.0.0.0",
                            "plm_format": IPluginListModel.CLAP,
                            "plm_type": IPluginListModel.MIDIEffect
                        }
                    );
                }
                for(let i = 0; i < 36; ++i) {
                    let text = Math.floor(Math.random() * 10).toString();
                    setProperty(i, "plm_name", text);
                }
            }
        }
        categoryListModel: ["EQ / Filter", "Dynamics", "Gain / Panning", "Phaser / Flanger", "Overdrive / Distortion"]
        onClosed: {
            root.close();
        }
    }
}
