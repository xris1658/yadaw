import QtQuick

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

    canClose: true

    Component.onCompleted: {
        EventReceiver.mainWindow = mainWindow;
        Global.qtVersion = "6.5.1";
        Global.qtCopyrightYear = "2023";
        mainWindowReady();
    }
}
