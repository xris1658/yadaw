import QtQml
import QtQuick
import QtQuick.Layouts

import YADAW.Models

Item {
    id: root

    property alias model: list.model
    property IAudioDeviceIOGroupListModel outputListModel: null
    property Window audioIOSelectorWindow: null

    QtObject {
        id: impl
        property bool usingPopup: false
        property ComboBoxButton usingPopupButton: null
        property int index: -1
    }
    Connections {
        id: connectToAudioIOSelector
        target: impl.usingPopup? audioIOSelectorWindow: null
        function onAccepted() {
            impl.usingPopupButton.checked = false;
            impl.usingPopup = false;
            impl.usingPopupButton = null;
            model.setData(
                model.index(impl.index, 0),
                audioIOSelectorWindow.audioIOSelector.currentPosition,
                IAuxIOTargetListModel.Target
            );
        }
        function onResetted() {
            impl.usingPopupButton.checked = false;
            impl.usingPopup = false;
            impl.usingPopupButton = null;
            model.setData(
                model.index(impl.index, 0),
                null,
                IAuxIOTargetListModel.Target
            );
        }
        function onCancelled() {
            impl.usingPopupButton.checked = false;
            impl.usingPopup = false;
            impl.usingPopupButton = null;
        }
    }
    SplitView {
        anchors.fill: parent
        handle: Item {
            implicitWidth: 2
            implicitHeight: 2
            Rectangle {
                width: 2
                height: parent.height
                anchors.right: parent.right
                color: Colors.controlBorder
            }
        }
        ListView {
            id: list
            SplitView.preferredWidth: 125
            height: root.height
            model: outputListModel
            delegate: ItemDelegate {
                text: (aiotlm_channel_index + 1) + ". " + outputListModel.data(
                    outputListModel.index(aiotlm_channel_index, 0),
                    IAudioDeviceIOGroupListModel.Name
                )
                width: list.width
                checkable: true
                checked: list.currentIndex === index
                onClicked: {
                    list.currentIndex = index;
                    destList.model = aiotlm_target
                }
            }
        }
        Item {
            id: rightPanePlaceholder
            height: root.height
            Item {
                id: buttonsPlaceholder
                property int spacing: 5
                height: addButton.height + spacing * 2 + splitter.height
                width: parent.width
                Row {
                    anchors.left: parent.left
                    anchors.leftMargin: parent.spacing
                    anchors.top: parent.top
                    anchors.topMargin: parent.spacing
                    spacing: parent.spacing
                    Button {
                        id: addButton
                        text: qsTr("&Add...")
                    }
                    Button {
                        id: locateButton
                        text: qsTr("&Locate")
                    }
                    Button {
                        id: disconnectButton
                        text: qsTr("&Disconnect")
                    }
                }
                Button {
                    anchors.right: parent.right
                    anchors.rightMargin: parent.spacing
                    anchors.top: parent.top
                    anchors.topMargin: parent.spacing
                    id: disconnectAllButton
                    text: qsTr("Disconnect All")
                    enabled: destList.count != 0
                }
                Label {
                    anchors.right: disconnectAllButton.left
                    anchors.rightMargin: parent.spacing
                    anchors.verticalCenter: disconnectAllButton.verticalCenter
                    text: destList.count == 1? destList.count + qsTr(" target"):
                          destList.count > 1? destList.count + qsTr(" targets"):
                          qsTr("No target")
                }
                Rectangle {
                    id: splitter
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: Colors.border
                }
            }
            ListView {
                id: destList
                anchors.fill: parent
                anchors.topMargin: buttonsPlaceholder.height
            }
        }
    }
}