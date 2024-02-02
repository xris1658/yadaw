import QtQuick
import QtQml.Models

import YADAW.Models

MainWindow {
    id: mainWindow
    assetDirectoryListModel: ListModel {
        ListElement {
            adlm_id: 1
            adlm_path: "/path/to/dir/1"
            adlm_name: "Asset Folder 1"
            adlm_default_name: "Dir 1"
        }
        ListElement {
            adlm_id: 1
            adlm_path: "/path/to/dir/2"
            adlm_name: "Asset Folder 2"
            adlm_default_name: "Dir 2"
        }
    }

    translationModel: ListModel {
        ListElement {
            lm_name: "English"
        }
        ListElement {
            lm_name: "Simplified Chinese"
        }
        ListElement {
            lm_name: "Loooooooooooooooooooooooooooooooooooooooooooooooong text"
        }
    }
    currentTranslationIndex: 0
    systemFontRendering: false
    audioGraphInputDeviceList: ListModel {
        ListElement {
            adlm_id: 0
            adlm_name: "Microphone"
            adlm_enabled: true
            adlm_channel_count: 2
        }
        ListElement {
            adlm_id: 1
            adlm_name: "Aux In"
            adlm_enabled: false
            adlm_channel_count: 2
        }
    }
    alsaInputDeviceList: audioGraphInputDeviceList
    audioGraphOutputDeviceList: ListModel {
        ListElement {
            adlm_id: 0
            adlm_name: "Speaker"
            adlm_enabled: true
            adlm_channel_count: 2
        }
    }
    audioGraphOutputDeviceIndex: 0
    alsaOutputDeviceList: audioGraphOutputDeviceList
    audioInputBusConfigurationModel: ListModel {
        ListElement {
            abcm_name: "Microphone"
            abcm_channel_list: [
                ListElement {
                    abclm_device_index: 0
                    abclm_channel_index: 0
                },
                ListElement {
                    abclm_device_index: 0
                    abclm_channel_index: 1
                }
            ]
        }
        ListElement {
            abcm_name: "Aux In"
            abcm_channel_list: [
                ListElement {
                    abclm_device_index: 1
                    abclm_channel_index: 0
                },
                ListElement {
                    abclm_device_index: 1
                    abclm_channel_index: 1
                }
            ]
        }
        ListElement {
            abcm_name: "Aux In"
            abcm_channel_list: [
                ListElement {
                    abclm_device_index: 1
                    abclm_channel_index: 0
                },
                ListElement {
                    abclm_device_index: 1
                    abclm_channel_index: 1
                }
            ]
        }
        ListElement {
            abcm_name: "Aux In"
            abcm_channel_list: [
                ListElement {
                    abclm_device_index: 1
                    abclm_channel_index: 0
                },
                ListElement {
                    abclm_device_index: 1
                    abclm_channel_index: 1
                }
            ]
        }
        ListElement {
            abcm_name: "Aux In"
            abcm_channel_list: [
                ListElement {
                    abclm_device_index: 1
                    abclm_channel_index: 0
                },
                ListElement {
                    abclm_device_index: 1
                    abclm_channel_index: 1
                }
            ]
        }
        ListElement {
            abcm_name: "Aux In"
            abcm_channel_list: [
                ListElement {
                    abclm_device_index: 1
                    abclm_channel_index: 0
                },
                ListElement {
                    abclm_device_index: 1
                    abclm_channel_index: 1
                }
            ]
        }
        ListElement {
            abcm_name: "Aux In"
            abcm_channel_list: [
                ListElement {
                    abclm_device_index: 1
                    abclm_channel_index: 0
                },
                ListElement {
                    abclm_device_index: 1
                    abclm_channel_index: 1
                }
            ]
        }
    }
    audioOutputBusConfigurationModel: ListModel {
        ListElement {
            abcm_name: "Speaker"
            abcm_channel_list: [
                ListElement {
                    abclm_device_index: 0
                    abclm_channel_index: 0
                },
                ListElement {
                    abclm_device_index: 0
                    abclm_channel_index: 1
                }
            ]
        }
        ListElement {
            abcm_name: "Speaker"
            abcm_channel_list: [
                ListElement {
                    abclm_device_index: 0
                    abclm_channel_index: 0
                },
                ListElement {
                    abclm_device_index: 0
                    abclm_channel_index: 1
                }
            ]
        }
        ListElement {
            abcm_name: "Speaker"
            abcm_channel_list: [
                ListElement {
                    abclm_device_index: 0
                    abclm_channel_index: 0
                },
                ListElement {
                    abclm_device_index: 0
                    abclm_channel_index: 1
                }
            ]
        }
        ListElement {
            abcm_name: "Speaker"
            abcm_channel_list: [
                ListElement {
                    abclm_device_index: 0
                    abclm_channel_index: 0
                },
                ListElement {
                    abclm_device_index: 0
                    abclm_channel_index: 1
                }
            ]
        }
        ListElement {
            abcm_name: "Speaker"
            abcm_channel_list: [
                ListElement {
                    abclm_device_index: 0
                    abclm_channel_index: 0
                },
                ListElement {
                    abclm_device_index: 0
                    abclm_channel_index: 1
                }
            ]
        }
        ListElement {
            abcm_name: "Speaker"
            abcm_channel_list: [
                ListElement {
                    abclm_device_index: 0
                    abclm_channel_index: 0
                },
                ListElement {
                    abclm_device_index: 0
                    abclm_channel_index: 1
                }
            ]
        }
    }

    mixerChannelModel: ListModel {
        ListElement {
            mclm_name: "Track 1"
            mclm_type: 0
            mclm_color: "#000000"
            mclm_height: 50
            mclm_mute: false
            mclm_solo: false
            mclm_invert_polarity: false
            mclm_arm_recording: false
            mclm_volume: 1.0
            mclm_panning: 0.0
            mclm_mono: false
        }
    }

    canClose: true

    Component.onCompleted: {
        EventReceiver.mainWindow = mainWindow;
        Global.qtVersion = "6.5.1";
        Global.qtCopyrightYear = "2023";
        mainWindowReady();
    }

    pluginListModel: IPluginListModel {
        Component.onCompleted: {
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
    }
}
