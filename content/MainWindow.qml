import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes

import YADAW.Models

ApplicationWindow {
    id: root
    width: 1280
    height: 720
    visible: false
    title: "YADAW"
    flags: Qt.Window

    property bool canClose: false
    property bool opened: false

    property alias assetDirectoryListModel: assets.directoryListModel

    property alias translationModel: preferencesWindow.translationModel
    property alias currentTranslationIndex: preferencesWindow.currentTranslationIndex
    property alias systemFontRendering: preferencesWindow.systemFontRendering
    property alias systemFontRenderingWhileDebugging: preferencesWindow.systemFontRenderingWhileDebugging
    property alias currentAudioBackend: preferencesWindow.currentAudioBackend
    property alias audioInputBusConfigurationModel: preferencesWindow.audioInputBusConfigurationModel
    property alias audioOutputBusConfigurationModel: preferencesWindow.audioOutputBusConfigurationModel
    property alias audioInputDeviceList: preferencesWindow.audioInputDeviceList
    property alias audioOutputDeviceList: preferencesWindow.audioOutputDeviceList
    property alias audioGraphInputDeviceList: preferencesWindow.audioGraphInputDeviceList
    property alias audioGraphOutputDeviceList: preferencesWindow.audioGraphOutputDeviceList
    property alias audioGraphOutputDeviceIndex: preferencesWindow.audioGraphOutputDeviceIndex
    property alias audioGraphBufferSize: preferencesWindow.audioGraphBufferSize
    property alias audioGraphLatency: preferencesWindow.audioGraphLatency
    property alias alsaInputDeviceList: preferencesWindow.alsaInputDeviceList
    property alias alsaOutputDeviceList: preferencesWindow.alsaOutputDeviceList

    property alias audioHardwareInputPositionModel: audioIOSelector.audioHardwareInputPositionModel
    property alias audioHardwareOutputPositionModel: audioIOSelector.audioHardwareOutputPositionModel
    property alias audioGroupChannelModel: audioIOSelector.audioGroupChannelModel
    property alias audioEffectChannelModel: audioIOSelector.audioEffectChannelModel
    property alias pluginAuxInModel: audioIOSelector.pluginAuxInModel
    property alias pluginAuxOutModel: audioIOSelector.pluginAuxOutModel

    property alias pluginListModel: assets.pluginListModel
    property alias pluginDirectoryListModel: preferencesWindow.pluginDirectoryListModel

    property alias mixerAudioInputChannelModel: mixer.inputModel
    property alias mixerAudioOutputChannelModel: mixer.outputModel
    property alias mixerChannelModel: mixer.channelsModel

    property alias cpuUsagePercentage: bigClock.cpuUsagePercentage

    property var pluginWindow: null
    property var genericPluginEditor: null

    property bool debugMode: false

    property PluginScanProgressWindow pluginScanProgressWindow: pluginScanProgressWindow
    property INativePopupEventFilterModel nativePopupEventFilterModel: null
    property INativePopupEventFilterModel quickMenuBarEventFilterModel: null
    onNativePopupEventFilterModelChanged: {
        Global.nativePopupEventFilterModel = nativePopupEventFilterModel;
    }
    onQuickMenuBarEventFilterModelChanged: {
        Global.quickMenuBarEventFilterModel = quickMenuBarEventFilterModel;
    }

    property bool previouslyMaximized: false

    onVisibilityChanged: {
        if(root.visibility != Window.FullScreen) {
            previouslyMaximized = root.visibility == Window.Maximized;
        }
        actionFullScreen.checked = root.visibility == Window.FullScreen;
    }

    KeyEventForwarder {
        id: keyEventForwarder
    }
    property KeyEventForwarder keyEventForwarder: keyEventForwarder
    MessageDialog {
        id: restartMessageDialog
        message: qsTr("The settings are saved. Please restart the application to apply the new settings.")
        icon: MessageDialog.Icon.Info
        title: "YADAW"
    }

    onActiveChanged: {
        if(!active) {
            nativePopupEventFilterModel.clear();
        }
    }

    onCurrentTranslationIndexChanged: {
        if(opened) {
            EventSender.setTranslationIndex(currentTranslationIndex);
            restartMessageDialog.focusButton(0);
            restartMessageDialog.showNormal();
        }
    }

    onCurrentAudioBackendChanged: {
        EventSender.currentAudioBackendChanged();
    }

    onAudioGraphOutputDeviceIndexChanged: {
        EventSender.audioGraphOutputDeviceIndexChanged(audioGraphOutputDeviceIndex);
    }

    signal pluginScanComplete()
    onPluginScanComplete: {
        preferencesWindow.pluginScanComplete();
    }

    property NativePopup nativePopupToShow: null
    signal mainWindowReady()
    onMainWindowReady: {
        opened = true;
        visible = true;
        // Qt 6.7.0+ workaround
        if(Global.qtMajorVersion == 6 && Global.qtMinorVersion >= 7) {
            pluginSelectorWindow.showMinimized();
            pluginSelectorWindow.hide();
            audioIOSelectorWindow.showMinimized();
            audioIOSelectorWindow.hide();
        }
    }

    Component.onCompleted: {
        EventReceiver.mainWindow = root;
    }

    onClosing: (close) => {
        menuBar.closeAllMenus();
        if(!canClose) {
            preferencesWindow.close();
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

    background: Rectangle {
        color: root.color
    }

    menuBar: MenuBar {
        Menu {
            title: "&YADAW"
            mnemonicRegex: Constants.mnemonicRegexInEnglish
            mnemonicRegexReplaceWith: Constants.mnemonicRegexReplaceWithInEnglish
            Action {
                text: qsTr("&About YADAW")
                onTriggered: {
                    let component = Qt.createComponent("AboutWindow.qml");
                    if(component.status == Component.Ready) {
                        let aboutWindow = component.createObject(root);
                        aboutWindow.showNormal();
                    }
                }
            }
            Action {
                text: qsTr("About &Qt")
                onTriggered: {
                    let component = Qt.createComponent("AboutQtWindow.qml");
                    if(component.status == Component.Ready) {
                        let aboutWindow = component.createObject(root);
                        aboutWindow.showNormal();
                    }
                }
            }
            MenuSeparator {}
            Action {
                text: qsTr("&Keyboard Shortcuts...")
            }
            Action {
                text: qsTr("Check for &Updates")
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
                text: qsTr("&Revert to Last Save")
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
                text: qsTr("&History")
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
                text: qsTr("Jump to Loop In")
                shortcut: "I"
            }
            Action {
                text: qsTr("Jump to Loop Out")
                shortcut: "O"
            }
            Action {
                text: qsTr("Set Loop In Here")
                shortcut: "Alt+I"
            }
            Action {
                text: qsTr("Set Loop Out Here")
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
                    EventSender.toggleMainWindowFullscreen();
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
                text: qsTr("Fo&llow")
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
            Action {
                id: actionShowAudioInputTrack
                text: qsTr("Show Audio &Inputs")
                checkable: true
                checked: arrangement.showAudioInputs
                onCheckedChanged: {
                    arrangement.showAudioInputs = checked;
                }
            }
            Action {
                id: actionShowAudioOutputTrack
                text: qsTr("Show Audio &Outputs")
                checkable: true
                checked: arrangement.showAudioOutputs
                onCheckedChanged: {
                    arrangement.showAudioOutputs = checked;
                }
            }
            Action {
                id: actionShowTempoAutomation
                text: qsTr("Show &Tempo Automation")
                checkable: true
                checked: false
            }
            Menu {
                title: qsTr("Arrangement Snap Unit")
                MenuSeparator {}
                MenuItem {
                    text: qsTr("Triplets")
                    checkable: true
                }
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
                text: qsTr("&Metronome")
                shortcut: "Ctrl+M"
                checkable: true
                checked: metronomeButton.checked
                onTriggered: {
                    metronomeButton.checked = checked;
                }
            }
            Action {
                id: actionEnableKeyboard
                text: qsTr("Map &Keyboard")
                checkable: true
                checked: mapKeyboardIcon.checked
                onTriggered: {
                    mapKeyboardIcon.checked = checked;
                }
            }
            Action {
                id: actionCountdown
                text: qsTr("&Record Countdown")
                checkable: true
                checked: countdownButton.checked
                onTriggered: {
                    countdownButton.checked = checked;
                }
            }

            MenuSeparator {}
            Action {
                text: qsTr("&Tap Tempo")
                onTriggered: {
                    tapTempoWindow.show();
                }
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
                    path.fillColor: parent.contentItem.color
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
                    path.fillColor: parent.contentItem.color
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
                    path.fillColor: parent.contentItem.color
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
                layer.enabled: true
                layer.smooth: true
                layer.samples: 4
                PianoKeysIcon {
                    anchors.centerIn: parent
                    scale: loopIcon.width * loopIcon.scale / originalHeight
                    path.fillColor: parent.contentItem.color
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
                    color: parent.contentItem.color
                }
            }
            GridLayout {
                id: showPaneButtons
                width: height
                height: loopButton.height
                rows: 2
                columns: 2
                rowSpacing: 0
                columnSpacing: 0
                Button {
                    id: showAssetsButton
                    Layout.preferredWidth: showPaneButtons.width / 2
                    Layout.preferredHeight: showPaneButtons.height / 2
                    border.width: 0
                    checkable: true
                    checked: true
                    FolderIcon {
                        anchors.centerIn: parent
                        scale: 16 / originalHeight
                        path.fillColor: parent.contentItem.color
                    }
                }
                Button {
                    id: showDetailButton
                    Layout.preferredWidth: showPaneButtons.width / 2
                    Layout.preferredHeight: showPaneButtons.height / 2
                    border.width: 0
                    enabled: showAssetsButton.checked
                    checkable: true
                    checked: true
                    InfoIcon {
                        anchors.centerIn: parent
                        scale: 16 / originalHeight
                        path.fillColor: parent.contentItem.color
                    }
                }
                Button {
                    id: showEditorButton
                    Layout.preferredWidth: showPaneButtons.width / 2
                    Layout.preferredHeight: showPaneButtons.height / 2
                    border.width: 0
                    checkable: true
                    checked: false
                    PianoKeysIcon {
                        anchors.centerIn: parent
                        scale: 16 / originalHeight
                        path.fillColor: parent.contentItem.color
                    }
                    onCheckedChanged: {
                        if(checked) {
                            showMixerButton.checked = false;
                            editorAndMixerStack.currentIndex = 0;
                        }
                    }
                }
                Button {
                    id: showMixerButton
                    Layout.preferredWidth: showPaneButtons.width / 2
                    Layout.preferredHeight: showPaneButtons.height / 2
                    border.width: 0
                    checkable: true
                    checked: true
                    Row {
                        anchors.centerIn: parent
                        Repeater {
                            model: 2
                            Item {
                                width: 16 * faderIcon.contentWidth / faderIcon.originalWidth + 2
                                height: showMixerButton.height
                                FaderIcon {
                                    id: faderIcon
                                    anchors.centerIn: parent
                                    scale: 16 / originalHeight
                                    path.fillColor: showMixerButton.contentItem.color
                                }
                            }
                        }
                    }
                    onCheckedChanged: {
                        if(checked) {
                            showEditorButton.checked = false;
                            editorAndMixerStack.currentIndex = 1;
                        }
                    }
                }
            }
        }
    }
    footer: Label {
        id: statusText
        leftPadding: 5
        rightPadding: leftPadding
        topPadding: 3
        bottomPadding: topPadding
        height: contentHeight + topPadding + bottomPadding
        elide: Text.ElideRight
        text: qsTr("Ready")
        color: root.debugMode? Colors.debugModeStatusBarForeground: Colors.content
        background: Rectangle {
            color: root.debugMode? Colors.debugModeStatusBarBackground: Colors.controlBackground
        }
    }
    SplitView {
        id: contents
        anchors.fill: parent
        anchors.margins: 5
        anchors.topMargin: 0
        anchors.bottomMargin: 0
        orientation: Qt.Horizontal
        SplitView {
            id: assetDetailSplitView
            SplitView.maximumWidth: Math.max(SplitView.minimumWidth, contents.width * 0.4)
            SplitView.preferredWidth: assets.preferredWidth
            orientation: Qt.Vertical
            visible: showAssetsButton.checked
            Item {
                id: assetsPlaceholder
                width: parent.width
                SplitView.preferredHeight: Math.max(SplitView.minimumHeight, contents.height * 0.7)
                Assets {
                    id: assets
                    anchors.fill: parent
                    anchors.topMargin: 5
                    anchors.bottomMargin: detailPlaceholder.visible? 0: 5
                    pluginAuxInModel: root.pluginAuxInModel
                    pluginAuxOutModel: root.pluginAuxOutModel
                }
            }
            Item {
                id: detailPlaceholder
                width: parent.width
                SplitView.minimumHeight: 100 + detail.anchors.bottomMargin
                visible: showDetailButton.checked
                Detail {
                    id: detail
                    anchors.fill: parent
                    anchors.bottomMargin: 5
                }
            }
        }
        SplitView {
            id: arrangementAndEditorMixerSplitView
            orientation: Qt.Vertical
            onResizingChanged: {
                if(!resizing) {
                    arrangementPlaceholder.ratio = arrangementPlaceholder.height / height;
                }
            }
            Item {
                id: arrangementPlaceholder
                property double ratio: 0.6
                width: parent.width
                SplitView.preferredHeight: contents.height * ratio
                Rectangle {
                    anchors.fill: parent
                    anchors.topMargin: 5
                    anchors.bottomMargin: editorAndMixerPlaceholder.visible? 0: 5
                    color: "transparent"
                    border.color: Colors.controlBorder
                    Arrangement {
                        id: arrangement
                        anchors.fill: parent
                        anchors.margins: parent.border.width
                        clip: true
                        onInsertTrack: (position, type) => {
                            addTrackWindow.openWindow(position, type);
                        }
                        trackList: root.mixerChannelModel
                    }
                }
            }
            Item {
                id: editorAndMixerPlaceholder
                SplitView.preferredHeight: root.height * (1 - arrangementPlaceholder.ratio)
                visible: showEditorButton.checked || showMixerButton.checked
                Rectangle {
                    color: "transparent"
                    border.color: Colors.controlBorder
                    anchors.fill: parent
                    anchors.bottomMargin: 5
                    StackLayout {
                        id: editorAndMixerStack
                        anchors.fill: parent
                        anchors.margins: parent.border.width
                        currentIndex: showEditorButton.checked? 0: 1
                        MIDIEditor {
                            id: editor
                        }
                        Mixer {
                            id: mixer
                            audioIOSelectorWindow: audioIOSelectorWindow
                            pluginSelectorWindow: pluginSelectorWindow
                            pluginRouteEditorWindow: pluginRouteEditorWindow
                            onInsertTrack: (position, type) => {
                                addTrackWindow.openWindow(position, type);
                            }
                            onChannelsModelChanged: {
                                assets.mixerChannelListModel = mixerChannelModel;
                            }
                        }
                    }
                }
            }
        }
    }
    PreferencesWindow {
        id: preferencesWindow
        onStartPluginScan: {
            EventSender.startPluginScan();
        }
        onSystemFontRenderingChanged: {
            if(opened) {
                EventSender.setSystemFontRendering(systemFontRendering);
                restartMessageDialog.focusButton(0);
                restartMessageDialog.showNormal();
            }
        }
        onSystemFontRenderingWhileDebuggingChanged: {
            if(opened) {
                EventSender.setSystemFontRenderingWhileDebugging(systemFontRenderingWhileDebugging);
                restartMessageDialog.focusButton(0);
                restartMessageDialog.showNormal();
            }
        }
    }
    TapTempoWindow {
        id: tapTempoWindow
    }
    Window {
        id: audioIOSelectorWindow
        width: audioIOSelector.width
        height: audioIOSelector.height
        color: Colors.background
        flags: Qt.Tool | Qt.FramelessWindowHint
        readonly property AudioIOSelector audioIOSelector: audioIOSelector
        signal accepted()
        signal cancelled()
        signal resetted()
        AudioIOSelector {
            id: audioIOSelector
            onAboutToHide: { // Handle closing on pressing Esc correctly
                audioIOSelectorWindow.hide();
            }
            onAccepted: {
                audioIOSelectorWindow.accepted();
                audioIOSelectorWindow.hide();
            }
            onCancelled: {
                audioIOSelectorWindow.cancelled();
                audioIOSelectorWindow.hide();
            }
            onResetted: {
                audioIOSelectorWindow.resetted();
                audioIOSelectorWindow.hide();
            }
            onActiveFocusChanged: {
                if(!activeFocus) {
                    cancelled();
                }
            }
        }
        onVisibleChanged: {
            if(visible) {
                audioIOSelector.open();
                audioIOSelector.forceActiveFocus();
                requestActivate();
            }
            else {
                audioIOSelector.cancelled();
            }
        }
    }
    Window {
        id: pluginSelectorWindow
        width: pluginSelector.width
        height: pluginSelector.height
        color: Colors.background
        flags: Qt.Tool | Qt.FramelessWindowHint
        readonly property PluginSelector pluginSelector: pluginSelector
        signal accepted()
        signal cancelled()
        signal resetted()
        PluginSelector {
            id: pluginSelector
            pluginListProxyModel.sourceModel: root.pluginListModel
            onAboutToHide: { // Handle closing on pressing Esc correctly
                pluginSelectorWindow.hide();
            }
            onAccepted: {
                pluginSelectorWindow.accepted();
                pluginSelectorWindow.hide();
            }
            onCancelled: {
                pluginSelectorWindow.cancelled();
                pluginSelectorWindow.hide();
            }
            onResetted: {
                pluginSelectorWindow.resetted();
                pluginSelectorWindow.hide();
            }
            onActiveFocusChanged: {
                if(!activeFocus) {
                    cancelled();
                }
            }
        }
        onVisibleChanged: {
            if(visible) {
                pluginSelector.open();
                pluginSelector.forceActiveFocus();
                requestActivate();
            }
            else {
                pluginSelector.close();
            }
        }
    }
    AddTrackWindow {
        id: addTrackWindow
        audioInputList: root.audioInputBusConfigurationModel
        audioOutputList: root.audioOutputBusConfigurationModel
        pluginSelectorWindow: pluginSelectorWindow
        function openWindow(position: int, type: int) {
            addTrackWindow.trackType = type;
            addTrackWindow.position = position;
            addTrackWindow.showNormal();
        }
        onAccepted: {
            EventSender.prepareBatchUpdate();
            let trackName = name? name: placeholderName;
            mixerChannelModel.insert(position, count, trackType, channelConfig);
            if(count === 1) {
                let index = mixerChannelModel.index(position, 0);
                mixerChannelModel.setData(
                    index,
                    trackName,
                    IMixerChannelListModel.Name
                );
                mixerChannelModel.setData(
                    index,
                    Qt.rgba(Math.random(), Math.random(), Math.random(), 1),
                    IMixerChannelListModel.Color
                );
            }
            else {
                for(let i = 0; i < count; ++i) {
                    let index = mixerChannelModel.index(position + i, 0);
                    mixerChannelModel.setData(
                        index,
                        trackName + " " + (i + 1).toString(),
                        IMixerChannelListModel.Name
                    );
                    mixerChannelModel.setData(
                        index,
                        Qt.rgba(Math.random(), Math.random(), Math.random(), 1),
                        IMixerChannelListModel.Color
                    );
                }
            }
            if(trackType === AddTrackWindow.Instrument && instrumentEnabled) {
                for(let i = 0; i < count; ++i) {
                    mixerChannelModel.setInstrument(position + i, instrument);
                }
            }
            else if(trackType === AddTrackWindow.AudioEffect && audioEffectEnabled) {
                for(let i = 0; i < count; ++i) {
                    let index = mixerChannelModel.index(position + i, 0);
                    // See comments in ctor of `MixerChannelInsertListModel`
                    mixerChannelModel.data(
                        index, IMixerChannelListModel.Inserts
                    ).append(audioEffect);
                }
            }
            EventSender.commitBatchUpdate();
        }
    }
    Window {
        id: pluginRouteEditorWindow
        color: Colors.background
        width: 500
        height: 300
        property PluginRouteEditor pluginRouteEditor: pluginRouteEditor
        property int padding: 10
        PluginRouteEditor {
            id: pluginRouteEditor
            width: pluginRouteEditorWindow.width - pluginRouteEditorWindow.padding * 2
            height: pluginRouteEditorWindow.height - pluginRouteEditorWindow.padding * 2
            x: pluginRouteEditorWindow.padding
            y: pluginRouteEditorWindow.padding
            audioIOSelectorWindow: audioIOSelectorWindow
        }
    }
    PluginScanProgressWindow {
        id: pluginScanProgressWindow
        transientParent: preferencesWindow
    }
    Window {
        id: clapAudioBusConfigurationSelectorWindow
        width: clapAudioBusConfigurationSelector.width
        height: clapAudioBusConfigurationSelector.height
        color: Colors.background
        flags: Qt.Tool | Qt.FramelessWindowHint
        signal accepted()
        signal tryDefault()
        signal cancelled()
        CLAPAudioBusConfigurationSelector {
            id: clapAudioBusConfigurationSelector
            // audioBusConfigurationListProxyModel.sourceModel: null /*FIXME*/
            onAccepted: {
                clapAudioBusConfigurationSelectorWindow.accepted();
                clapAudioBusConfigurationSelectorWindow.hide();
            }
            onCancelled: {
                clapAudioBusConfigurationSelectorWindow.cancelled();
                clapAudioBusConfigurationSelectorWindow.hide();
            }
            onTryDefault: {
                clapAudioBusConfigurationSelectorWindow.tryDefault();
                clapAudioBusConfigurationSelectorWindow.hide();
            }
            onActiveFocusChanged: {
                if(!activeFocus) {
                    cancelled();
                }
            }
        }
        onVisibleChanged: {
            if(visible) {
                clapAudioBusConfigurationSelector.open();
                clapAudioBusConfigurationSelector.forceActiveFocus();
                requestActivate();
            }
            else {
                clapAudioBusConfigurationSelector.close();
            }
        }
    }
}
