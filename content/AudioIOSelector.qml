import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts

import YADAW.Entities
import YADAW.Models

QC.Popup {
    id: root

    readonly property alias audioHardwareInputPositionProxyModel: audioHardwareInputPositionProxyModel
    readonly property alias audioHardwareOutputPositionProxyModel: audioHardwareOutputPositionProxyModel
    readonly property alias audioGroupChannelProxyModel: audioGroupChannelProxyModel
    readonly property alias audioEffectChannelProxyModel: audioEffectChannelProxyModel
    readonly property alias pluginAuxInProxyModel: pluginAuxInProxyModel
    readonly property alias pluginAuxOutProxyModel: pluginAuxOutProxyModel

    property alias audioHardwareInputPositionModel: audioHardwareInputPositionProxyModel.sourceModel
    property alias audioHardwareOutputPositionModel: audioHardwareOutputPositionProxyModel.sourceModel
    property alias audioGroupChannelModel: audioGroupChannelProxyModel.sourceModel
    property alias audioEffectChannelModel: audioEffectChannelProxyModel.sourceModel
    property alias pluginAuxInModel: pluginAuxInProxyModel.sourceModel
    property alias pluginAuxOutModel: pluginAuxOutProxyModel.sourceModel

    property bool showAudioHardwareInput: true
    property bool showAudioHardwareOutput: true
    property bool showAudioGroupChannel: true
    property bool showAudioEffectChannel: true
    property bool showPluginAuxIn: true
    property bool showPluginAuxOut: true

    property alias currentIndex: stackLayout.currentIndex
    property string currentId: ""
    property int channelConfig

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
            proxyModel.setFilter(IAudioIOPositionModel.Name, true, Qt.CaseInsensitive);
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
        id: audioGroupChannelProxyModel
        filterString: searchTextField.text
    }
    SortFilterProxyListModel {
        id: audioEffectChannelProxyModel
        filterString: searchTextField.text
    }
    SortFilterProxyListModel {
        id: pluginAuxInProxyModel
        filterString: searchTextField.text
    }
    SortFilterProxyListModel {
        id: pluginAuxOutProxyModel
        filterString: searchTextField.text
    }

    Component {
        id: audioIOPositionComponent
        ItemDelegate {
            width: parent.width
            text: aiopm_name
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
                        }
                    }
                }
                StackLayout {
                    id: stackLayout
                    ListView {
                        id: audioHardwareInputListView
                        model: audioHardwareInputPositionProxyModel
                        delegate: audioIOPositionComponent
                    }
                    ListView {
                        id: audioHardwareOutputListView
                        model: audioHardwareOutputPositionProxyModel
                        delegate: audioIOPositionComponent
                    }
                    ListView {
                        id: audioGroupChannelListView
                        model: audioGroupChannelProxyModel
                        delegate: audioIOPositionComponent
                    }
                    ListView {
                        id: audioEffectChannelListView
                        model: audioEffectChannelProxyModel
                        delegate: audioIOPositionComponent
                    }
                    ListView {
                        id: pluginAuxInListView
                        model: pluginAuxInProxyModel
                        delegate: audioIOPositionComponent
                    }
                    ListView {
                        id: pluginAuxOutListView
                        model: pluginAuxOutProxyModel
                        delegate: audioIOPositionComponent
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
    onShowAudioGroupChannelChanged: {
        leftListModel.setProperty(2, "show", showAudioGroupChannel);
    }
    onShowAudioEffectChannelChanged: {
        leftListModel.setProperty(3, "show", showAudioEffectChannel);
    }
    onShowPluginAuxInChanged: {
        leftListModel.setProperty(4, "show", showPluginAuxIn);
    }
    onShowPluginAuxOutChanged: {
        leftListModel.setProperty(5, "show", showPluginAuxOut);
    }
    Component.onCompleted: {
        impl.initProxyModel(audioHardwareInputPositionProxyModel);
        impl.initProxyModel(audioHardwareOutputPositionProxyModel);
        impl.initProxyModel(audioGroupChannelProxyModel);
        impl.initProxyModel(audioEffectChannelProxyModel);
        impl.initProxyModel(pluginAuxInProxyModel);
        impl.initProxyModel(pluginAuxOutProxyModel);
    }
}
