import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts

QC.Popup {
    id: root

    property alias audioHardwareInputPositionModel: audioHardwareInputListView.model
    property alias audioHardwareOutputPositionModel: audioHardwareOutputListView.model
    property alias audioGroupChannelModel: audioGroupChannelListView.model
    property alias audioEffectChannelModel: audioEffectChannelListView.model
    property alias pluginAuxInModel: pluginAuxInListView.model
    property alias pluginAuxOutModel: pluginAuxOutListView.model

    property bool showAudioHardwareInput: true
    property bool showAudioHardwareOutput: true
    property bool showAudioGroupChannel: true
    property bool showAudioEffectChannel: true
    property bool showPluginAuxIn: true
    property bool showPluginAuxOut: true

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
        property int contentWidth: 400
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
                    SplitView.preferredWidth: 100
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
                    }
                    ListView {
                        id: audioHardwareOutputListView
                    }
                    ListView {
                        id: audioGroupChannelListView
                    }
                    ListView {
                        id: audioEffectChannelListView
                    }
                    ListView {
                        id: pluginAuxInListView
                    }
                    ListView {
                        id: pluginAuxOutListView
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
}
