pragma Singleton

import QtQuick

QtObject {
    readonly property string okText:                    qsTr("OK")
    readonly property string okTextWithMnemonic:        qsTr("&OK")
    readonly property string cancelText:                qsTr("Cancel")
    readonly property string cancelTextWithMnemonic:    qsTr("&Cancel")
    readonly property string applyText:                 qsTr("Apply")
    readonly property string applyTextWithMnemonic:     qsTr("&Apply")
    readonly property string closeText:                 qsTr("Close")
    readonly property string closeTextWithMnemonic:     qsTr("&Close")
    readonly property string yesText:                   qsTr("Yes")
    readonly property string yesTextWithMnemonic:       qsTr("&Yes")
    readonly property string noText:                    qsTr("No")
    readonly property string noTextWithMnemonic:        qsTr("&No")
    readonly property string yesToAllText:              qsTr("Yes to All")
    readonly property string yesToAllTextWithMnemonic:  qsTr("Yes to &All")
    readonly property string noToAllText:               qsTr("No to All")
    readonly property string noToAllTextWithMnemonic:   qsTr("N&o to All")
    readonly property string abortText:                 qsTr("Abort")
    readonly property string abortTextWithMnemonic:     qsTr("&Abort")
    readonly property string retryText:                 qsTr("Retry")
    readonly property string retryTextWithMnemonic:     qsTr("&Retry")
    readonly property string ignoreText:                qsTr("Ignore")
    readonly property string ignoreTextWithMnemonic:    qsTr("&Ignore")
    readonly property string undoText:                  qsTr("Undo")
    readonly property string undoTextWithMnemonic:      qsTr("&Undo")
    readonly property string redoText:                  qsTr("Redo")
    readonly property string redoTextWithMnemonic:      qsTr("&Redo")
    readonly property string cutText:                   qsTr("Cut")
    readonly property string cutTextWithMnemonic:       qsTr("Cu&t")
    readonly property string copyText:                  qsTr("Copy")
    readonly property string copyTextWithMnemonic:      qsTr("&Copy")
    readonly property string pasteText:                 qsTr("Paste")
    readonly property string pasteTextWithMnemonic:     qsTr("&Paste")
    readonly property string deleteText:                qsTr("Delete")
    readonly property string deleteTextWithMnemonic:    qsTr("&Delete")
    readonly property string selectAllText:             qsTr("Select All")
    readonly property string selectAllTextWithMnemonic: qsTr("Select &All")
    readonly property string duplicateText:             qsTr("Duplicate")
    readonly property string duplicateTextWithMnemonic: qsTr("&Duplicate")
    readonly property string renameText:                qsTr("Rename")
    readonly property string renameTextWithMnemonic:    qsTr("Rena&me")

    enum ChannelConfig {
        Invalid,
        Mono,
        Stereo,
        LRC,
        Quad,
        C51,
        C61,
        C71
    }

    readonly property ListModel channelConfigProperties: ListModel {
        ListElement {
            type: Constants.ChannelConfig.Mono
            name: qsTr("Mono")
            channelCount: 1
        }
        ListElement {
            type: Constants.ChannelConfig.Stereo
            name: qsTr("Stereo")
            channelCount: 2
        }
        ListElement {
            type: Constants.ChannelConfig.LRC
            name: qsTr("LRC")
            channelCount: 3
        }
        ListElement {
            type: Constants.ChannelConfig.Quad
            name: qsTr("Quad")
            channelCount: 4
        }
        ListElement {
            type: Constants.ChannelConfig.C51
            name: qsTr("5.1")
            channelCount: 6
        }
        ListElement {
            type: Constants.ChannelConfig.C61
            name: qsTr("6.1")
            channelCount: 7
        }
        ListElement {
            type: Constants.ChannelConfig.C71
            name: qsTr("7.1")
            channelCount: 8
        }
    }

    readonly property ListModel dynamicKeyName: ListModel {
        ListElement {
            flat: "C"
            sharp: "C"
        }
        ListElement {
            flat: "D\u266d"
            sharp: "C\u266f"
        }
        ListElement {
            flat: "D"
            sharp: "D"
        }
        ListElement {
            flat: "E\u266d"
            sharp: "D\u266f"
        }
        ListElement {
            flat: "E"
            sharp: "E"
        }
        ListElement {
            flat: "F"
            sharp: "F"
        }
        ListElement {
            flat: "G\u266d"
            sharp: "F\u266f"
        }
        ListElement {
            flat: "G"
            sharp: "G"
        }
        ListElement {
            flat: "A\u266d"
            sharp: "G\u266f"
        }
        ListElement {
            flat: "A"
            sharp: "A"
        }
        ListElement {
            flat: "B\u266d"
            sharp: "A\u266f"
        }
        ListElement {
            flat: "B"
            sharp: "B"
        }
    }

    readonly property ListModel noteName: ListModel {
        ListElement {
            name: "\u266f"
            asciiName: "#"
        }
        ListElement {
            name: "\u266d"
            asciiName: "b"
        }
    }

    enum NoteName { Sharp, Flat }
    readonly property int midiValueRange: 128
    readonly property int maxVelocity: midiValueRange - 1
    readonly property int minVelocity: 0
    property int titleTextPointSize: 8
}
