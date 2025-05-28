pragma Singleton

import QtCore
import QtQuick

import YADAW.Entities

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
    readonly property ChannelConfig channelConfig: ChannelConfig {}

    readonly property ListModel channelConfigProperties: ListModel {
        ListElement {
            type: ChannelConfig.Mono
            name: qsTr("Mono")
        }
        ListElement {
            type: ChannelConfig.Stereo
            name: qsTr("Stereo")
        }
        ListElement {
            type: ChannelConfig.LRC
            name: qsTr("LRC")
        }
        ListElement {
            type: ChannelConfig.Quad
            name: qsTr("Quad")
        }
        ListElement {
            type: ChannelConfig.C51
            name: qsTr("5.1")
        }
        ListElement {
            type: ChannelConfig.C61
            name: qsTr("6.1")
        }
        ListElement {
            type: ChannelConfig.C71
            name: qsTr("7.1")
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

    readonly property ListModel volumeFaderScaleModel: ListModel {
        id: volumeFaderScaleList
        ListElement {
            position: 0
            value: -144
        }
        ListElement {
            position: 0.0625
            value: -72
        }
        ListElement {
            position: 0.125
            value: -48
        }
        ListElement {
            position: 0.203125
            value: -36
        }
        ListElement {
            position: 0.28125
            value: -24
        }
        ListElement {
            position: 0.375
            value: -18
        }
        ListElement {
            position: 0.5
            value: -12
        }
        ListElement {
            position: 0.640625
            value: -6
        }
        ListElement {
            position: 0.796875
            value: 0
        }
        ListElement {
            position: 1
            value: 6
        }
    }

    enum NoteName { Sharp, Flat }
    readonly property int midiValueRange: 128
    readonly property int maxVelocity: midiValueRange - 1
    readonly property int minVelocity: 0
    property int titleTextPointSize: 8
    property string mnemonicRegexInEnglish: "&"
    property string mnemonicRegexReplaceWithInEnglish: ""
    property string mnemonicRegex: qsTr("&", "`mnemonicRegex`: Replace this pattern of the mnemonic part of item text, e.g. \"&\" in \"&File\"")
    property string mnemonicRegexReplaceWith: qsTr("", "Replace the mnemonic part matched with `mnemonicRegex` with this")
    property bool isMnemonicSupported: SystemInformation.productType != "macos"
}
