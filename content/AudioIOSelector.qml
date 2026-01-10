import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts

import YADAW.Entities
import YADAW.Models

QC.Popup {
    id: root

    property alias audioHardwareInputChannelListModel: audioHardwareInputPositionProxyModel.sourceModel
    property alias regularChannelListModel: audioChannelProxyModel.sourceModel
    property alias audioHardwareOutputChannelListModel: audioHardwareOutputPositionProxyModel.sourceModel
    property alias pluginAuxInModel: pluginAuxInTreeView.model
    property alias pluginAuxOutModel: pluginAuxOutTreeView.model

    property bool showAudioHardwareInput: true
    property bool showAudioHardwareOutput: true
    property bool showAudioChannel: true
    property bool showAudioGroupChannel: true
    property bool showAudioEffectChannel: true
    property bool showPluginAuxIn: true
    property bool showPluginAuxOut: true
    property alias showResetButton: resetButton.visible

    property int audioChannelConfig: -1
    property alias currentIndex: stackLayout.currentIndex
    property IAudioIOPosition currentPosition: null

    readonly property var audioIOTypes: [
       IAudioIOPosition.AudioHardwareIOChannel,
       IAudioIOPosition.AudioHardwareIOChannel,
       IAudioIOPosition.AudioChannelInput,
       IAudioIOPosition.BusAndFXChannelInput,
       IAudioIOPosition.BusAndFXChannelInput,
       IAudioIOPosition.PluginAuxIO,
       IAudioIOPosition.PluginAuxIO
    ]

    signal accepted()
    signal cancelled()
    signal resetted()

    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0
    padding: 10

    QtObject {
        id: impl
        property int contentWidth: 350
        function initProxyModel(proxyModel) {
            proxyModel.setFilter(IMixerChannelListModel.NameWithIndex, true, Qt.CaseInsensitive);
        }
    }

    SortFilterProxyListModel {
        id: audioHardwareInputPositionProxyModel
        filterString: searchTextField.text
    }
    SortFilterProxyListModel {
        id: audioHardwareOutputPositionProxyModel
        filterString: searchTextField.text
    }
    SortFilterProxyListModel {
        id: audioChannelProxyModel
        filterString: searchTextField.text
        Component.onCompleted: {
            setValueOfFilter(
                IMixerChannelListModel.ChannelType,
                IMixerChannelListModel.ChannelTypeAudio
            );
        }
    }
    SortFilterProxyListModel {
        id: audioGroupChannelProxyModel
        filterString: searchTextField.text
        sourceModel: regularChannelListModel
        Component.onCompleted: {
            setValueOfFilter(
                IMixerChannelListModel.ChannelType,
                IMixerChannelListModel.ChannelTypeBus
            );
        }
    }
    SortFilterProxyListModel {
        id: audioEffectChannelProxyModel
        filterString: searchTextField.text
        sourceModel: regularChannelListModel
        Component.onCompleted: {
            setValueOfFilter(
                IMixerChannelListModel.ChannelType,
                IMixerChannelListModel.ChannelTypeAudioFX
            );
        }
    }

    Component {
        id: audioIOPositionComponent
        ItemDelegate {
            width: parent.width
            property var position: ListView.view === audioHardwareInputListView? mclm_output: mclm_input
            text: position? position.completeName: ""
            highlighted: ListView.view.currentIndex === index
            onClicked: {
                ListView.view.currentIndex = index;
                root.currentPosition = position;
            }
            onDoubleClicked: {
                ListView.view.currentIndex = index;
                root.currentPosition = position;
                root.accepted();
            }
        }
    }
    Component {
        id: audioIOPositionTreeComponent
        ItemDelegate {
            required property TreeView treeView
            required property bool isTreeNode
            required property bool expanded
            required property int hasChildren
            required property int depth
            leftPadding: depth * height + indicator.width
            text: aiopim_position? aiopim_position.completeName + " (" + aiopim_tree_name + ")": aiopim_tree_name
            Label {
                id: indicator
                x: depth * parent.height
                width: height
                height: parent.height
                verticalAlignment: Label.AlignVCenter
                horizontalAlignment: Label.AlignHCenter
                visible: parent.hasChildren
                text: parent.expanded? "\u25bc": "\u25b6"
            }
            onClicked: {
                if(hasChildren) {
                    treeView.toggleExpanded(index);
                }
                else if(aiopim_position) {
                    root.currentPosition = aiopim_position;
                }
            }
            onDoubleClicked: {
                if(aiopim_position) {
                    root.currentPosition = aiopim_position;
                    root.accepted();
                }
            }
        }
    }

    background: Rectangle {
        anchors.fill: parent
        anchors.topMargin: root.topInset
        anchors.bottomMargin: root.bottomInset
        anchors.leftMargin: root.leftInset
        anchors.rightMargin: root.rightInset
        color: Colors.background
        border.color: Colors.border
    }

    contentItem: Column {
        spacing: 5
        TextField {
            id: searchTextField
            placeholderText: "<i>" + qsTr("Search (Ctrl+F)") + "</i>"
            width: impl.contentWidth
            onTextChanged: {
                console.log(text);
            }
        }
        Rectangle {
            width: impl.contentWidth
            height: 300
            color: "transparent"
            border.color: Colors.border
            SplitView {
                anchors.fill: parent
                anchors.margins: parent.border.width
                orientation: Qt.Horizontal
                handle: Item {
                    implicitWidth: 5
                    implicitHeight: 5
                    Rectangle {
                        width: 1
                        height: parent.height
                        anchors.right: parent.right
                        color: Colors.controlBorder
                    }
                }
                Item {
                    id: categoryList
                    property int currentIndex: 0
                    SplitView.preferredWidth: 150
                    Item {
                        ListView {
                            id: leftList
                            x: 5
                            y: 5
                            width: categoryList.width
                            height: contentHeight
                            model: ListModel {
                                id: leftListModel
                                dynamicRoles: true
                                Component.onCompleted: {
                                    append({
                                        "name": qsTr("Audio Input Bus"),
                                        "iconSource": "IOIcon.qml",
                                        "show": true
                                    });
                                    append({
                                        "name": qsTr("Audio Output Bus"),
                                        "iconSource": "IOIcon.qml",
                                        "show": true
                                    });
                                    append({
                                        "name": qsTr("Audio Channel"),
                                        "iconSource": "AudioIcon.qml",
                                        "show": true
                                    });
                                    append({
                                        "name": qsTr("Audio Group Channel"),
                                        "iconSource": "GroupIcon.qml",
                                        "show": true
                                    });
                                    append({
                                        "name": qsTr("Audio Effect Channel"),
                                        "iconSource": "AudioFXIcon.qml",
                                        "show": true
                                    });
                                    append({
                                        "name": qsTr("Plugin Aux Input"),
                                        "iconSource": "AuxInputIcon.qml",
                                        "show": true
                                    });
                                    append({
                                        "name": qsTr("Plugin Aux Output"),
                                        "iconSource": "AuxOutputIcon.qml",
                                        "show": true
                                    });
                                }

                            }
                            delegate: ItemDelegate {
                                width: parent.width - 3 * 2
                                height: implicitHeight
                                enabled: show
                                text: name
                                leftPadding: height
                                rightPadding: 2
                                topPadding: 2
                                bottomPadding: 2
                                highlighted: stackLayout.currentIndex === index
                                clip: true
                                Item {
                                    id: enableLayerForIcons
                                    width: height
                                    height: parent.height
                                    layer.enabled: true
                                    layer.smooth: true
                                    layer.textureSize: Qt.size(width * 2, height * 2)
                                    Loader {
                                        id: loader
                                        source: iconSource
                                        onLoaded: {
                                            item.parent = enableLayerForIcons;
                                            item.scale = 16 / item.originalHeight;
                                            item.anchors.centerIn = enableLayerForIcons;
                                            item.path.strokeColor = "transparent";
                                            item.path.fillColor = Colors.secondaryContent;
                                        }
                                    }
                                }
                                onClicked: {
                                    stackLayout.currentIndex = index;
                                }
                            }
                            onCurrentIndexChanged: {
                                audioHardwareInputListView.currentIndex = -1;
                                audioHardwareOutputListView.currentIndex = -1;
                                audioChannelListView.currentIndex = -1;
                                audioGroupChannelListView.currentIndex = -1;
                                audioEffectChannelListView.currentIndex = -1;
                                // pluginAuxInListView.currentIndex = -1;
                                // pluginAuxOutListView.currentIndex = -1;
                                root.currentPosition = null;
                            }
                        }
                    }
                }
                StackLayout {
                    id: stackLayout
                    ListView {
                        id: audioHardwareInputListView
                        clip: true
                        model: audioHardwareInputPositionProxyModel
                        delegate: audioIOPositionComponent
                    }
                    ListView {
                        id: audioHardwareOutputListView
                        clip: true
                        model: audioHardwareOutputPositionProxyModel
                        delegate: audioIOPositionComponent
                    }
                    ListView {
                        id: audioChannelListView
                        clip: true
                        model: audioChannelProxyModel
                        delegate: audioIOPositionComponent
                    }
                    ListView {
                        id: audioGroupChannelListView
                        clip: true
                        model: audioGroupChannelProxyModel
                        delegate: audioIOPositionComponent
                    }
                    ListView {
                        id: audioEffectChannelListView
                        clip: true
                        model: audioEffectChannelProxyModel
                        delegate: audioIOPositionComponent
                    }
                    TreeView {
                        id: pluginAuxInTreeView
                        clip: true
                        columnWidthProvider: function (column) {
                            return pluginAuxInTreeView.width;
                        }
                        delegate: audioIOPositionTreeComponent
                    }
                    TreeView {
                        id: pluginAuxOutTreeView
                        clip: true
                        columnWidthProvider: function (column) {
                            return pluginAuxOutTreeView.width;
                        }
                        delegate: audioIOPositionTreeComponent
                    }
                    onCurrentIndexChanged: {
                        root.currentPosition = null;
                    }
                }
            }
        }
        Item {
            width: impl.contentWidth
            height: cancelButton.height
            Button {
                id: resetButton
                text: qsTr("&Disconnect")
                onClicked: {
                    root.resetted();
                }
            }
            Row {
                anchors.right: parent.right
                spacing: 5
                Button {
                    id: okButton
                    text: Constants.okTextWithMnemonic
                    onClicked: {
                        root.accepted();
                    }
                }
                Button {
                    id: cancelButton
                    text: Constants.cancelTextWithMnemonic
                    onClicked: {
                        root.cancelled();
                    }
                }
            }
        }
    }
    onShowAudioHardwareInputChanged: {
        leftListModel.setProperty(0, "show", showAudioHardwareInput);
    }
    onShowAudioHardwareOutputChanged: {
        leftListModel.setProperty(1, "show", showAudioHardwareOutput);
    }
    onShowAudioChannelChanged: {
        leftListModel.setProperty(2, "show", showAudioChannel);
    }
    onShowAudioGroupChannelChanged: {
        leftListModel.setProperty(3, "show", showAudioGroupChannel);
    }
    onShowAudioEffectChannelChanged: {
        leftListModel.setProperty(4, "show", showAudioEffectChannel);
    }
    onShowPluginAuxInChanged: {
        leftListModel.setProperty(5, "show", showPluginAuxIn);
    }
    onShowPluginAuxOutChanged: {
        leftListModel.setProperty(6, "show", showPluginAuxOut);
    }
    Component.onCompleted: {
        impl.initProxyModel(audioHardwareInputPositionProxyModel);
        impl.initProxyModel(audioHardwareOutputPositionProxyModel);
        impl.initProxyModel(audioChannelProxyModel);
        impl.initProxyModel(audioGroupChannelProxyModel);
        impl.initProxyModel(audioEffectChannelProxyModel);
        // TODO: Init aux input/output model once `SortFilterProxyItemModel` is used
    }
    onAudioChannelConfigChanged: {
        audioHardwareInputPositionProxyModel.setValueOfFilter(IMixerChannelListModel.ChannelConfig, audioChannelConfig);
        audioHardwareOutputPositionProxyModel.setValueOfFilter(IMixerChannelListModel.ChannelConfig, audioChannelConfig);
        audioChannelProxyModel.setValueOfFilter(IMixerChannelListModel.ChannelConfig, audioChannelConfig);
        audioGroupChannelProxyModel.setValueOfFilter(IMixerChannelListModel.ChannelConfig, audioChannelConfig);
        audioEffectChannelProxyModel.setValueOfFilter(IMixerChannelListModel.ChannelConfig, audioChannelConfig);
        // TODO: Set value filter of input/output model once `SortFilterProxyItemModel` is used
    }
}
