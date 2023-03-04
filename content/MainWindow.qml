import QtQml
import QtQuick
import QtQuick.Shapes

ApplicationWindow {
    id: root
    width: 1280
    height: 720
    visible: true
    title: "YADAW"
    flags: Qt.Window

    property bool canClose: false

    property alias assetDirectoryListModel: assets.directoryListModel
    property alias pluginDirectoryListModel: preferencesWindow.pluginDirectoryListModel

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
                        EventSender.darkModeSupportWindow = aboutWindow;
                        EventSender.addWindowForDarkModeSupport();
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
                        EventSender.darkModeSupportWindow = aboutWindow;
                        EventSender.addWindowForDarkModeSupport();
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
                    EventSender.darkModeSupportWindow = preferencesWindow;
                    EventSender.addWindowForDarkModeSupport();
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
                checked: loopButton.checked
                onTriggered: {
                    loopButton.checked = checked;
                }
            }
            Action {
                text: qsTr("Jump to Loop In Point")
                shortcut: "I"
            }
            Action {
                text: qsTr("Jump to Loop Out Point")
                shortcut: "O"
            }
            Action {
                text: qsTr("Set Loop In Point Here")
                shortcut: "Alt+I"
            }
            Action {
                text: qsTr("Set Loop Out Point Here")
                shortcut: "Alt+O"
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
                checked: followButton.checked
                onTriggered: {
                    followButton.checked = checked;
                }
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
                checked: metronomeButton.checked
                onTriggered: {
                    metronomeButton.checked = checked;
                }
            }
            Action {
                id: actionEnableKeyboard
                text: qsTr("Map Keyboard")
                checkable: true
                checked: mapKeyboardIcon.checked
                onTriggered: {
                    mapKeyboardIcon.checked = checked;
                }
            }
            Action {
                id: actionCountdown
                text: qsTr("Record Countdown")
                checkable: true
                checked: countdownButton.checked
                onTriggered: {
                    countdownButton.checked = checked;
                }
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
    header: Rectangle {
        height: bigClock.height + 20
        color: Colors.topBarBackground
        Row {
            anchors.top: bigClockBackground.top
            anchors.right: bigClockBackground.left
            anchors.margins: 10
            spacing: 5
            Button {
                id: playButton
                width: height * 1.5
                height: loopButton.height
                checkable: true
                border.width: 0
                radius: 5
                layer.enabled: true
                layer.smooth: true
                layer.samples: 4
                PlayIcon {
                    id: playIcon
                    anchors.centerIn: parent
                    scale: playButton.height / originalHeight * 0.5
                    path.strokeColor: "transparent"
                    path.fillColor: parent.checked? Colors.playingIcon: Colors.content
                    path.capStyle: ShapePath.RoundCap
                }
            }
            Button {
                id: stopButton
                width: height
                height: loopButton.height
                border.width: 0
                radius: 5
                Rectangle {
                    anchors.centerIn: parent
                    width: loopIcon.width * loopIcon.scale * 0.9
                    height: width
                    color: Colors.content
                }
            }
            Button {
                id: recordButton
                width: height
                height: loopButton.height
                checkable: true
                border.width: 0
                radius: 5
                Rectangle {
                    anchors.centerIn: parent
                    width: loopIcon.width * loopIcon.scale
                    height: width
                    color: parent.checked? Colors.recordingIcon: Colors.recordIcon
                    radius: width / 2
                }
            }
            Button {
                id: loopButton
                width: height
                height: Qt.application.font.pixelSize * 3
                checkable: true
                border.width: 0
                radius: 5
                layer.enabled: true
                layer.smooth: true
                layer.samples: 4
                LoopIcon {
                    id: loopIcon
                    anchors.centerIn: parent
                    scale: loopButton.height / actualHeight * 0.6
                    path.strokeColor: "transparent"
                    path.fillColor: parent.checked? Colors.content: Colors.disabledContent
                    path.joinStyle: ShapePath.MiterJoin
                }
            }
        }
        Rectangle {
            id: bigClockBackground
            anchors.centerIn: parent
            width: bigClock.width + 20
            height: bigClock.height + radius * 2
            radius: 5
            color: Colors.bigClockBackground
            BigClock {
                id: bigClock
                anchors.centerIn: parent
                cpuUsagePercentage: 0
            }
        }
        Row {
            anchors.top: bigClockBackground.top
            anchors.left: bigClockBackground.right
            anchors.margins: 10
            spacing: 5
            Button {
                id: metronomeButton
                width: height
                height: loopButton.height
                checkable: true
                border.width: 0
                radius: 5
                layer.enabled: true
                layer.smooth: true
                layer.samples: 4
                MetronomeIcon {
                    anchors.centerIn: parent
                    scale: 20 / originalHeight
                    path.fillColor: parent.checked? Colors.content: Colors.disabledContent
                    path.strokeColor: "transparent"
                }
            }
            Button {
                id: followButton
                width: height
                height: loopButton.height
                checkable: true
                border.width: 0
                radius: 5
                layer.enabled: true
                layer.smooth: true
                layer.samples: 4
                FollowIcon {
                    anchors.centerIn: parent
                    scale: loopIcon.width * loopIcon.scale / originalHeight
                    path.fillColor: parent.checked? Colors.content: Colors.disabledContent
                    path.strokeColor: "transparent"
                }
            }
            Button {
                id: mapKeyboardIcon
                width: height
                height: loopButton.height
                checkable: true
                border.width: 0
                radius: 5
                PianoKeysIcon {
                    anchors.centerIn: parent
                    scale: loopIcon.width * loopIcon.scale / originalHeight
                    path.fillColor: parent.checked? Colors.content: Colors.disabledContent
                    path.strokeColor: "transparent"
                }
            }
            Button {
                id: countdownButton
                width: height
                height: loopButton.height
                checkable: true
                border.width: 0
                radius: 5
                Label {
                    anchors.centerIn: parent
                    text: "3 2 1"
                    font.family: "Fira Sans Condensed"
                    font.bold: true
                    color: parent.checked? Colors.content: Colors.disabledContent
                }
            }
        }
    }
    footer: Label {
        leftPadding: 5
        rightPadding: leftPadding
        topPadding: 3
        bottomPadding: topPadding
        id: statusText
        height: contentHeight + topPadding + bottomPadding
        elide: Text.ElideRight
        text: "Ready"
        background: Rectangle {
            color: Colors.controlBackground
        }
    }
    SplitView {
        id: contents
        anchors.fill: parent
        anchors.margins: 5
        orientation: Qt.Horizontal
        SplitView {
            id: assetDetailSplitView
            SplitView.minimumWidth: assetTabButton.height
            SplitView.maximumWidth: Math.max(SplitView.minimumWidth, contents.width * 0.4)
            SplitView.preferredWidth: assets.preferredWidth + assetTabButton.height
            orientation: Qt.Vertical
            handle: Item {
                implicitWidth: 5
                implicitHeight: 5
            }
            Row {
                id: assetRow
                SplitView.minimumHeight: assetTabButton.width
                SplitView.preferredHeight: Math.max(SplitView.minimumHeight, contents.height * 0.7)
                Item {
                    width: assetTabButton.height
                    height: parent.height
                    TabButton {
                        id: assetTabButton
                        text: qsTr("Assets")
                        x: -width
                        y: 0
                        transformOrigin: Item.TopRight
                        rotation: -90
                    }
                }
                Assets {
                    id: assets
                    width: parent.width - assetTabButton.height
                    height: parent.height
                }
            }
            Row {
                id: detailRow
                SplitView.minimumHeight: detailTabButton.width
                Item {
                    width: detailTabButton.height
                    height: parent.height
                    TabButton {
                        id: detailTabButton
                        text: qsTr("Detail")
                        x: height
                        y: parent.height - height
                        transformOrigin: Item.BottomLeft
                        rotation: -90
                    }
                }
                Detail {
                    width: parent.width - detailTabButton.height
                    height: parent.height
                }
            }
        }
        SplitView {
            orientation: Qt.Vertical
            Column {
                SplitView.minimumHeight: arrangementTabButton.height
                SplitView.preferredHeight: contents.height * 0.6
                TabButton {
                    id: arrangementTabButton
                    text: qsTr("Arrangement")
                }
                Rectangle {
                    width: parent.width
                    height: parent.height - arrangementTabButton.height
                    color: "transparent"
                    border.color: Colors.controlBorder
                    Item {
                        anchors.fill: parent
                        anchors.margins: parent.border.width
                    }
                }
            }
            Column {
                SplitView.minimumHeight: editorAndMixerTabButtonRow.height
                Rectangle {
                    width: parent.width
                    height: parent.height - editorAndMixerTabButtonRow.height
                    color: "transparent"
                    border.color: Colors.controlBorder
                    Item {
                        anchors.fill: parent
                        anchors.margins: parent.border.width
                    }
                }
                Row {
                    id: editorAndMixerTabButtonRow
                    width: parent.width
                    TabButton {
                        width: implicitWidth
                        text: qsTr("Editor")
                    }
                    TabButton {
                        width: implicitWidth
                        text: qsTr("Mixer")
                    }
                }
            }
        }
    }
    PreferencesWindow {
        id: preferencesWindow
        color: root.color
    }
}
