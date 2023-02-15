import QtQml
import QtQuick

ApplicationWindow {
    id: root
    width: 1280
    height: 720
    visible: true
    title: "YADAW"
    flags: Qt.Window

    property bool canClose: false

    Component.onCompleted: {
        EventReceiver.mainWindow = root;
    }

    onClosing: (close) => {
        if(!canClose) {
            close.accepted = false;
            EventSender.mainWindowClosing();
        }
        if(canClose) {
            close.accepted = true;
        }
    }

    function closingAccepted() {
        canClose = true;
    }

    menuBar: MenuBar {
        Menu {
            title: "&YADAW"
            Action {
                text: qsTr("&About YADAW...")
                onTriggered: {
                    var component = Qt.createComponent("AboutWindow.qml");
                    if(component.status == Component.Ready) {
                        var aboutWindow = component.createObject(null);
                        aboutWindow.palette = root.palette;
                        aboutWindow.showNormal();
                    }
                }
            }
            Action {
                text: qsTr("About &Qt...")
                onTriggered: {
                    var component = Qt.createComponent("AboutQtWindow.qml");
                    if(component.status == Component.Ready) {
                        var aboutWindow = component.createObject(null);
                        aboutWindow.palette = root.palette;
                        aboutWindow.showNormal();
                    }
                }
            }
            MenuSeparator {}
            Action {
                text: qsTr("&Keyboard Shortcuts...")
            }
            Action {
                text: qsTr("Check for &Updates...")
            }
            MenuSeparator {}
            Action {
                text: qsTr("&Preferences...")
                shortcut: "Ctrl+,"
                onTriggered: {
                    preferencesWindow.showNormal();
                }
            }
            MenuSeparator {}
            Action {
                text: qsTr("E&xit")
                shortcut: "Ctrl+Q"
                onTriggered: {
                    root.close();
                }
            }
        }
        Menu {
            title: qsTr("&File")

            Action {
                text: qsTr("&New")
                shortcut: "Ctrl+N"
            }
            Action {
                text: qsTr("&Open")
                shortcut: "Ctrl+O"
            }
            Menu {
                title: qsTr("Recent files")

                Action {
                    text: qsTr("Empty list")
                    enabled: false
                }
                MenuSeparator {}
                Action {
                    text: qsTr("Clear list")
                }
            }
            MenuSeparator {}
            Action {
                text: qsTr("&Save")
                shortcut: "Ctrl+S"
            }
            Action {
                id: actionSaveAs
                text: qsTr("Save &As...")
                shortcut: "Ctrl+Shift+S"
            }
            MenuSeparator {}
            Menu {
                title: qsTr("&Export")

                Action {
                    text: qsTr("&Mixdown...")
                    shortcut: "Ctrl+Shift+R"
                }
                Action {
                    text: qsTr("Multiple &Tracks...")
                }
                MenuSeparator {}
                Action {
                    text: qsTr("&Packed Project...")
                }
                Action {
                    text: qsTr("Music&XML...")
                }
                MenuSeparator {}
                Action {
                    text: qsTr("&Score...")
                }
            }
            MenuSeparator {}
            Action {
                text: qsTr("&Project Properties...")
                shortcut: "Alt+3"
            }
            MenuSeparator {}
            Action {
                text: qsTr("&Revert to Last Save...")
                shortcut: "F12"
            }
        }
        Menu {
            title: qsTr("&Edit")

            Action {
                text: Constants.undoTextWithMnemonic
                shortcut: "Ctrl+Z"
                enabled: false
            }
            Action {
                text: Constants.redoTextWithMnemonic
                shortcut: "Ctrl+Y"
                enabled: false
            }
            Action {
                text: qsTr("&History...")
                shortcut: "Ctrl+H"
                enabled: false
            }

            MenuSeparator {}
            Action {
                text: Constants.cutTextWithMnemonic
                shortcut: "Ctrl+X"
            }
            Action {
                text: Constants.copyTextWithMnemonic
                shortcut: "Ctrl+C"
            }
            Action {
                text: Constants.pasteTextWithMnemonic
                shortcut: "Ctrl+V"
            }
            Action {
                text: Constants.deleteTextWithMnemonic
                shortcut: "Delete"
            }
            MenuSeparator {}
            Action {
                text: Constants.selectAllTextWithMnemonic
                shortcut: "Ctrl+A"
            }
            MenuSeparator {}
            Action {
                text: qsTr("Jump to Start")
                shortcut: "Home"
            }
            Action {
                text: qsTr("Jump to End")
                shortcut: "End"
            }
            MenuSeparator {}
            Action {
                id: actionLoop
                text: qsTr("Loop")
                shortcut: "Ctrl+L"
                checkable: true
                checked: false
            }
            Action {
                text: qsTr("Set Loop In Point")
                shortcut: "I"
            }
            Action {
                text: qsTr("Set Loop Out Point")
                shortcut: "O"
            }
        }
        Menu {
            title: qsTr("&View")

            Action {
                id: actionFullScreen
                property bool previouslyMaximized: false
                text: qsTr("&Fullscreen")
                shortcut: "F11"
                checkable: true
                onTriggered: {
                    if(!checked) {
                        if(previouslyMaximized) {
                            root.showMaximized();
                        }
                        else {
                            root.show();
                        }
                    }
                    else {
                        previouslyMaximized = (root.visibility === 4);
                        root.showFullScreen();
                    }
                }
            }
            MenuSeparator {}
            Action {
                id: actionAssetVisible
                text: qsTr("&Assets")
                shortcut: "F8"
                checkable: true
                checked: true
            }
            Action {
                id: actionMidiEditorVisible
                text: qsTr("&Clip Editor")
                shortcut: "F7"
                checkable: true
                checked: true
            }
            Action {
                id: actionMixerVisible
                text: qsTr("&Mixer")
                shortcut: "F9"
                checkable: true
                checked: true
            }
            MenuSeparator {}
            Action {
                id: actionFollow
                text: qsTr("&Follow")
                shortcut: "Ctrl+Shift+F"
                checkable: true
                checked: false
            }
            Action {
                id: actionShowKeyScale
                text: qsTr("Show &Key Scale")
                checkable: true
            }
            MenuSeparator {}
            Action {
                id: actionSnap
                text: qsTr("&Snap")
                checkable: true
                checked: false
            }
            MenuSeparator {}
            Menu {
                id: menuShowMasterTrack
                title: qsTr("Master Track")
            }
            Menu {
                title: qsTr("Arrangement Snap Unit")
            }
            Menu {
                title: qsTr("Editor Snap Unit")
                MenuSeparator {}
                MenuItem {
                    text: qsTr("Triplets")
                    checkable: true
                }
            }
        }
        Menu {
            title: qsTr("&Tools")

            Action {
                id: menuItemMetronome
                text: qsTr("Metronome")
                shortcut: "Ctrl+M"
                checkable: true
                checked: false
            }
            Action {
                id: actionEnableKeyboard
                text: qsTr("Map Keyboard")
                checkable: true
                checked: false
            }
            MenuSeparator {}
            Action {
                text: qsTr("Tap Tempo...")
            }
        }
        Menu {
            title: qsTr("&Help")

            Action {
                text: qsTr("&Manual")
                shortcut: "F1"
            }
            Action {
                text: qsTr("&Quick Start")
            }
            MenuSeparator {}
            Action {
                text: qsTr("&Register Product")
            }
            Action {
                text: qsTr("User Support")
            }
        }
    }
    PreferencesWindow {
        id: preferencesWindow
        color: root.color
    }
}
