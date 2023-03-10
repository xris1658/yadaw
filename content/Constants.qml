pragma Singleton

import QtQuick

QtObject {
    readonly property string okText:                    qsTr("OK")
    readonly property string okTextWithMnemonic:        qsTr("&OK")
    readonly property string cancelText:                qsTr("Cancel")
    readonly property string cancelTextWithMnemonic:    qsTr("&Cancel")
    readonly property string yesText:                   qsTr("Yes")
    readonly property string yesTextWithMnemonic:       qsTr("&Yes")
    readonly property string noText:                    qsTr("No")
    readonly property string noTextWithMnemonic:        qsTr("&No")
    readonly property string yesToAll:                  qsTr("Yes to All")
    readonly property string yesToAllWithMnemonic:      qsTr("Yes to &All")
    readonly property string noToAll:                   qsTr("No to All")
    readonly property string noToAllWithMnemonic:       qsTr("N&o to All")
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

    readonly property ListModel technicalSupport: ListModel {
        ListElement {
            name: "ASIO"
            homepage: "https://www.steinberg.net/developers/"
            description: qsTr("Low latency audio driver")
        }
        ListElement {
            name: "FFmpeg"
            homepage: "https://www.ffmpeg.org/"
            description: qsTr("Library for manipulating multimedia files")
        }
        ListElement {
            name: "Avcpp"
            homepage: "https://github.com/h4tr3d/avcpp"
            description: qsTr("C++ wrapper of FFmpeg")
        }
        ListElement {
            name: "Inno Setup"
            homepage: "https://jrsoftware.org/isinfo.php"
            description: qsTr("Installer framework")
        }
        ListElement {
            name: "LAME"
            homepage: "https://lame.sourceforge.io/"
            description: qsTr("MP3 audio encoder")
        }
        ListElement {
            name: "Qt"
            homepage: "https://www.qt.io/"
            description: qsTr("Cross-platform application development toolkit")
        }
        ListElement {
            name: "The SoX Resampler library"
            homepage: "https://sourceforge.net/p/soxr/wiki/Home/"
            description: qsTr("Audio resampler")
        }
        ListElement {
            name: "spdlog"
            homepage: "https://github.com/gabime/spdlog"
            description: qsTr("Logger")
        }
        ListElement {
            name: "{fmt}"
            homepage: "https://fmt.dev"
            description: qsTr("Library for text formatting, dependency of spdlog")
        }
        ListElement {
            name: "SQLite"
            homepage: "https://www.sqlite.org/index.html"
            description: qsTr("Light-weight, efficient database engine")
        }
        ListElement {
            name: "sqlite-modern-cpp"
            homepage: "https://github.com/aminroosta/sqlite_modern_cpp"
            description: qsTr("Modern C++ wrapper of SQLite")
        }
        ListElement {
            name: "VST"
            homepage: "https://www.steinberg.net/developers/"
            description: qsTr("Widely used audio plugin format")
        }
        ListElement {
            name: "CLAP"
            homepage: "https://cleveraudio.org"
            description: qsTr("Free audio plugin format")
        }
        ListElement {
            name: "yaml-cpp"
            homepage: "https://github.com/jbeder/yaml-cpp/"
            description: qsTr("YAML parser and emitter")
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
